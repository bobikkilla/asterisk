#include <stdio.h>
#include <omp.h>

#define M 10000  // rows of matrix A
#define N 8000   // cols of matrix A (and size of vector b)

// Matrix A: row i, column j
double A[M][N];

// Vector b
double b[N];

// Result vector c
double c[M];

// Initialize matrix and vector with sample data
void initialize() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j * 0.1; // arbitrary values
        }
    }
    for (int j = 0; j < N; j++) {
        b[j] = j * 0.5 + 1.0;      // arbitrary values
    }
}

int main() {
    initialize();

    double start_time = omp_get_wtime();

    // Sequential matrix-vector multiplication
    for (int i = 0; i < M; i++) {
        c[i] = 0.0;
        for (int j = 0; j < N; j++) {
            c[i] += A[i][j] * b[j];
        }
    }

    double end_time = omp_get_wtime();

    printf("Sequential execution time: %.5f seconds\n", end_time - start_time);

    // Print first 3 elements of result for verification
    printf("c[0] = %.6f\n", c[0]);
    printf("c[1] = %.6f\n", c[1]);
    printf("c[2] = %.6f\n", c[2]);

    return 0;
}