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

    // Parallel for: OpenMP automatically distributes rows among threads
    #pragma omp parallel for
    for (int i = 0; i < M; i++) {
        c[i] = 0.0;
        for (int j = 0; j < N; j++) {
            c[i] += A[i][j] * b[j];
        }
    }

    double end_time = omp_get_wtime();

    printf("Parallel (omp for) execution time: %.5f seconds\n", end_time - start_time);
    printf("c[0] = %.6f\n", c[0]);
    printf("c[1] = %.6f\n", c[1]);
    printf("c[2] = %.6f\n", c[2]);

    return 0;
}