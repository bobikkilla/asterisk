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
            A[i][j] = 1;
        }
    }
    for (int j = 0; j < N; j++) {
        b[j] = 1;
    }
}

int main() {
    initialize();

    // Parallel for: OpenMP automatically distributes rows among threads
    #pragma omp parallel for
    for (int i = 0; i < M; i++) {
        c[i] = 0.0;
        for (int j = 0; j < N; j++) {
            c[i] += A[i][j] * b[j];
        }
    }

    printf("c[0] = %.6f\n", c[0]);
    printf("c[1] = %.6f\n", c[1]);
    printf("c[2] = %.6f\n", c[2]);

    return 0;
}