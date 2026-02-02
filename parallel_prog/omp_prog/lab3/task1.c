#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Function to swap two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to fill array with random numbers in range [1;100]
void fillArrayWithRandom(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100 + 1;  // Random number from 1 to 100
    }
}

// Function to print the array
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Parallel odd-even sort using OpenMP sections
void oddEvenSort(int arr[], int n) {
    // The algorithm requires n steps to guarantee sorting
    for (int phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            // Even phase: compare pairs (0,1), (2,3), (4,5), ...
            int total_even_pairs = (n % 2 == 0) ? n/2 : (n-1)/2;

            #pragma omp parallel sections
            {
                #pragma omp section
                {
                    // Process first half of even-indexed pairs
                    for (int i = 0; i < total_even_pairs/2; i++) {
                        int idx = i * 2;
                        if (idx + 1 < n && arr[idx] > arr[idx + 1]) {
                            swap(&arr[idx], &arr[idx + 1]);
                        }
                    }
                }
                #pragma omp section
                {
                    // Process second half of even-indexed pairs
                    for (int i = total_even_pairs/2; i < total_even_pairs; i++) {
                        int idx = i * 2;
                        if (idx + 1 < n && arr[idx] > arr[idx + 1]) {
                            swap(&arr[idx], &arr[idx + 1]);
                        }
                    }
                }
            }
        } else {
            // Odd phase: compare pairs (1,2), (3,4), (5,6), ...
            int total_odd_pairs = (n % 2 == 0) ? n/2 : (n-1)/2;

            #pragma omp parallel sections
            {
                #pragma omp section
                {
                    // Process first half of odd-indexed pairs
                    for (int i = 0; i < total_odd_pairs/2; i++) {
                        int idx = i * 2 + 1;
                        if (idx + 1 < n && arr[idx] > arr[idx + 1]) {
                            swap(&arr[idx], &arr[idx + 1]);
                        }
                    }
                }
                #pragma omp section
                {
                    // Process second half of odd-indexed pairs
                    for (int i = total_odd_pairs/2; i < total_odd_pairs; i++) {
                        int idx = i * 2 + 1;
                        if (idx + 1 < n && arr[idx] > arr[idx + 1]) {
                            swap(&arr[idx], &arr[idx + 1]);
                        }
                    }
                }
            }
        }
    }
}

int main() {
    
    
    srand(time(NULL)); // Initialize random seed

    int n;
    printf("Enter the number of elements: ");
    scanf("%d", &n);
    int *arr = (int*)malloc(n * sizeof(int));

    // Fill array with random numbers [1;100]
    fillArrayWithRandom(arr, n);

    printf("Original array (random): ");
    printArray(arr, n);
    
    double start_time = omp_get_wtime();
    oddEvenSort(arr, n);
    double end_time = omp_get_wtime();
    
    printf("Sorted array: ");
    printArray(arr, n);
    
    printf("Time taken: %f seconds\n", end_time - start_time);
    
    free(arr);
    return 0;
}