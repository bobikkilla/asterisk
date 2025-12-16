#define PRIMES
#ifdef PRIMES

#define L_RANGE 1
#define R_RANGE 500000000

#define NTHREADS 6  // threads num

#endif
/* 
gcc -O2 -std=c11 sequential_primenums.c -lm -o primes_seq

gcc -O2 -std=c11 -pthread parallel_primenums.c -lm -o primes_thr 
*/

/*
    Результаты выполнения на моем компьютере:
    Параллельное вычисление простых чисел:
        Диапазон: [1, 500000000]
        Простых чисел: 26355867
        Время: 284.905616 сек

    Последовательное вычисление простых чисел:
        Диапазон: [1, 500000000]
        Простых чисел: 26355867
        Время: 1226.264236 сек
*/