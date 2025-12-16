#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdatomic.h>

#include "primes.h"

atomic_long global_count;

int is_prime(long n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    long limit = (long)sqrt((double)n);
    for (long i = 3; i <= limit; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

typedef struct {
    long start;
    long end;
} ThreadRange;

void* worker(void* arg) {
    ThreadRange* r = (ThreadRange*)arg;
    long local_count = 0;
    for (long n = r->start; n <= r->end; ++n) {
        if (is_prime(n)) ++local_count;
    }
    atomic_fetch_add(&global_count, local_count);
    free(r);
    return NULL;
}

double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main() {
    atomic_store(&global_count, 0);
    pthread_t threads[NTHREADS];

    long span = R_RANGE - L_RANGE + 1;
    long chunk = span / NTHREADS;

    double t0 = get_time_sec();

    for (int t = 0; t < NTHREADS; ++t) {
        long start = L_RANGE + t * chunk;
        long end = (t == NTHREADS - 1) ? R_RANGE : start + chunk - 1;

        ThreadRange* r = malloc(sizeof(ThreadRange));
        r->start = start;
        r->end = end;

        pthread_create(&threads[t], NULL, worker, r);
    }

    for (int t = 0; t < NTHREADS; ++t) {
        pthread_join(threads[t], NULL);
    }

    double t1 = get_time_sec();
    long count = atomic_load(&global_count);

    printf("Диапазон: [%ld, %ld]\n", L_RANGE, R_RANGE);
    printf("Простых чисел: %ld\n", count);
    printf("Время: %.6f сек\n", t1 - t0);
    return 0;
}