// primes_sequential.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "primes.h"

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

double get_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main() {
    double t0 = get_time_sec();
    long count = 0;
    for (long n = L_RANGE; n <= R_RANGE; ++n) {
        if (is_prime(n)) ++count;
    }
    double t1 = get_time_sec();

    printf("Диапазон: [%ld, %ld]\n", L_RANGE, R_RANGE);
    printf("Простых чисел: %ld\n", count);
    printf("Время: %.6f сек\n", t1 - t0);
    return 0;
}