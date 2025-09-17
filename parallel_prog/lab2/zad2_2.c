#include <stdio.h>
#include <omp.h>

#define M 10000  // rows of matrix A
#define N 8000   // cols of matrix A (and size of vector b)

double A[M][N];
double b[N];
double c[M];

void initialize() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j * 0.1;
        }
    }
    for (int j = 0; j < N; j++) {
        b[j] = j * 0.5 + 1.0;
    }
}

int main() {
    initialize();

    double start_time = omp_get_wtime();

    // Parallel region: each thread computes its own set of rows
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();      // thread ID
        int num_threads = omp_get_num_threads(); // total threads

        // Calculate how many rows per thread
        int rows_per_thread = M / num_threads;
        int start_row = tid * rows_per_thread;
        int end_row = (tid == num_threads - 1) ? M : start_row + rows_per_thread;

        // Each thread computes its assigned rows
        for (int i = start_row; i < end_row; i++) {
            c[i] = 0.0;
            for (int j = 0; j < N; j++) {
                c[i] += A[i][j] * b[j];
            }
        }
    }

    double end_time = omp_get_wtime();

    printf("Parallel (manual) execution time: %.5f seconds\n", end_time - start_time);
    printf("c[0] = %.6f\n", c[0]);
    printf("c[1] = %.6f\n", c[1]);
    printf("c[2] = %.6f\n", c[2]);

    return 0;
}