#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <omp.h>

typedef long double ldouble;

// Интегрируемая функция
ldouble f(ldouble x) {
    if (x >= 0.0L && x <= 1.0L) {
        return cosl(x) * expl(-x * x);
    } else if (x > 1.0L && x <= 2.0L) {
        return logl(x + 1.0L) - sqrtl(4.0L - x * x);
    }
    return 0.0L;
}

// Вспомогательная функция: время в секундах
double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Последовательный метод средних прямоугольников
ldouble midpoint_sequential(ldouble a, ldouble b, long long n) {
    ldouble h = (b - a) / (ldouble)n;
    ldouble sum = 0.0L;
    for (long long i = 0; i < n; ++i) {
        ldouble xi = a + (i + 0.5L) * h;
        sum += f(xi);
    }
    return sum * h;
}

// ===== pthreads =====
typedef struct {
    ldouble h;
    ldouble a;
    long long start, end;
    ldouble* psum;
} WorkerArgs;

static void* midpoint_pthread_worker(void* arg) {
    WorkerArgs* w = (WorkerArgs*)arg;
    ldouble local_sum = 0.0L;
    for (long long i = w->start; i < w->end; ++i) {
        ldouble xi = w->a + (i + 0.5L) * w->h;
        local_sum += f(xi);
    }
    *w->psum = local_sum;
    return NULL;
}

// Безопасная версия через общий h
ldouble midpoint_pthreads(ldouble a, ldouble b, long long n, int nthreads) {
    ldouble h = (b - a) / (ldouble)n;
    WorkerArgs* args = malloc(nthreads * sizeof(WorkerArgs));
    pthread_t* threads = malloc(nthreads * sizeof(pthread_t));
    ldouble* sums = malloc(nthreads * sizeof(ldouble));

    long long chunk = n / nthreads;
    for (int t = 0; t < nthreads; ++t) {
        sums[t] = 0.0L;
        args[t].h = h;
        args[t].a = a;
        args[t].start = t * chunk;
        args[t].end = (t == nthreads - 1) ? n : (t + 1) * chunk;
        args[t].psum = &(sums[t]);
        pthread_create(&threads[t], NULL, midpoint_pthread_worker, &args[t]);
    }

    ldouble total = 0.0L;
    for (int t = 0; t < nthreads; ++t) {
        pthread_join(threads[t], NULL);
        total += sums[t];
    }

    free(args);
    free(threads);
    free(sums);
    return total * h;
}

// OpenMP версия
ldouble midpoint_openmp(ldouble a, ldouble b, long long n) {
    ldouble h = (b - a) / (ldouble)n;
    ldouble sum = 0.0L;
    #pragma omp parallel for reduction(+:sum)
    for (long long i = 0; i < n; ++i) {
        ldouble xi = a + (i + 0.5L) * h;
        sum += f(xi);
    }
    return sum * h;
}

// Универсальная процедура: уточнение до eps методом Рунге
typedef ldouble (*Integrator)(ldouble, ldouble, long long, ...);

ldouble integrate_adaptive(
    ldouble (*method)(ldouble, ldouble, long long, ...),
    ldouble a, ldouble b,
    ldouble eps,
    int nthreads,
    long long* out_n_final
) {
    ldouble I_h = 0.0L, I_h2 = 0.0L;
    long long n = 2;  // начальное разбиение

    do {
        if (nthreads == 0) {
            I_h = midpoint_sequential(a, b, n);
            I_h2 = midpoint_sequential(a, b, 2 * n);
        } else if (nthreads == -1) {
            // OpenMP
            I_h = midpoint_openmp(a, b, n);
            I_h2 = midpoint_openmp(a, b, 2 * n);
        } else {
            // pthreads
            I_h = midpoint_pthreads(a, b, n, nthreads);
            I_h2 = midpoint_pthreads(a, b, 2 * n, nthreads);
        }

        ldouble runge_err = fabsl(I_h2 - I_h) / 3.0L;
        // printf("n=%lld, I_h=%.20Lf, I_h2=%.20Lf, err=%.3Le\n", n, I_h, I_h2, (double)runge_err);
        if (runge_err <= eps) {
            if (out_n_final) *out_n_final = 2 * n;
            return I_h2;
        }
        n *= 2;
        // защита от бесконечного цикла (на практике не сработает при eps=1e-16)
        if (n > (1LL << 30)) {
            fprintf(stderr, "Warning: too many steps (n=%lld), breaking\n", n);
            break;
        }
    } while (1);

    if (out_n_final) *out_n_final = 2 * n;
    return I_h2;
}

// Обёртка для удобства вызова
ldouble integrate_sequential(ldouble a, ldouble b, ldouble eps, long long* n_out) {
    return integrate_adaptive(NULL, a, b, eps, 0, n_out);
}
ldouble integrate_pthreads(ldouble a, ldouble b, ldouble eps, int nthreads, long long* n_out) {
    return integrate_adaptive(NULL, a, b, eps, nthreads, n_out);
}
ldouble integrate_openmp(ldouble a, ldouble b, ldouble eps, long long* n_out) {
    return integrate_adaptive(NULL, a, b, eps, -1, n_out);
}

// Но проще — просто скопируем логику трижды без шаблонов для ясности.
// Ниже — явная реализация (без указателей на функции), чтобы не было ошибок в pthreads.

// ----------------------------------------
int main() {
    const ldouble a = 0.0L, b = 2.0L;
    const ldouble eps = 1e-16L;  // чтобы гарантировать ~1e-15 итоговой погрешности

    long long n_seq, n_thr, n_omp;
    ldouble I_seq, I_thr, I_omp;
    double t1, t2, t_seq, t_thr, t_omp;

    // === Последовательный ===
    printf("=== Sequential ===\n");
    t1 = get_time_sec();
    {
        ldouble I_h, I_h2;
        long long n = 2;
        do {
            I_h  = midpoint_sequential(a, b, n);
            I_h2 = midpoint_sequential(a, b, 2 * n);
            ldouble err = fabsl(I_h2 - I_h) / 3.0L;
            if (err <= eps) {
                I_seq = I_h2;
                n_seq = 2 * n;
                break;
            }
            n *= 2;
        } while (1);
    }
    t2 = get_time_sec();
    t_seq = t2 - t1;

    // === pthreads (4 потока) ===
    int nthreads = 8;
    printf("=== pthreads (%d threads) ===\n", nthreads);
    t1 = get_time_sec();
    {
        ldouble I_h, I_h2;
        long long n = 2;
        do {
            I_h  = midpoint_pthreads(a, b, n, nthreads);
            I_h2 = midpoint_pthreads(a, b, 2 * n, nthreads);
            ldouble err = fabsl(I_h2 - I_h) / 3.0L;
            if (err <= eps) {
                I_thr = I_h2;
                n_thr = 2 * n;
                break;
            }
            n *= 2;
        } while (1);
    }
    t2 = get_time_sec();
    t_thr = t2 - t1;

    // === OpenMP ===
    printf("=== OpenMP ===\n");
    t1 = get_time_sec();
    {
        ldouble I_h, I_h2;
        long long n = 2;
        do {
            I_h  = midpoint_openmp(a, b, n);
            I_h2 = midpoint_openmp(a, b, 2 * n);
            ldouble err = fabsl(I_h2 - I_h) / 3.0L;
            if (err <= eps) {
                I_omp = I_h2;
                n_omp = 2 * n;
                break;
            }
            n *= 2;
        } while (1);
    }
    t2 = get_time_sec();
    t_omp = t2 - t1;

    // === Вывод результатов ===
    printf("\n");
    printf("Sequential   : I = %.20Lf, n = %lld, time = %.6f s\n", I_seq, n_seq, t_seq);
    printf("pthreads (%d): I = %.20Lf, n = %lld, time = %.6f s\n", nthreads, I_thr, n_thr, t_thr);
    printf("OpenMP       : I = %.20Lf, n = %lld, time = %.6f s\n", I_omp, n_omp, t_omp);

    return 0;
}