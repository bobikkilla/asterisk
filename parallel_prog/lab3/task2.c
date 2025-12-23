#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <omp.h>

// Function to swap two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Simple bubble sort algorithm
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

// Function to fill array with random numbers in range [1;100]
void fillArrayWithRandom(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100 + 1;  // Random number from 1 to 100
    }
}

int main() {
    const int N = 10000;
    const int num_runs = 5;

    // Different scheduling types and thread counts to test
    char* sched_types[] = {"static", "dynamic", "guided"};
    int thread_counts[] = {1, 2, 4};  // Added single-threaded for speedup calculation
    int num_sched_types = 3;
    int num_thread_counts = 3;

    printf("Running performance tests...\n");
    printf("Format: Threads, Schedule, Average Time (seconds), Speedup\n");

    // Store baseline time for speedup calculation (single-threaded static)
    double baseline_time = 0.0;

    // Test different configurations
    for (int t = 0; t < num_thread_counts; t++) {
        for (int s = 0; s < num_sched_types; s++) {
            double total_time = 0.0;

            // Set schedule type via environment variable
            if (strcmp(sched_types[s], "static") == 0) {
                omp_set_schedule(omp_sched_static, 0);
            } else if (strcmp(sched_types[s], "dynamic") == 0) {
                omp_set_schedule(omp_sched_dynamic, 0);
            } else if (strcmp(sched_types[s], "guided") == 0) {
                omp_set_schedule(omp_sched_guided, 0);
            }

            // Run multiple times and average the results
            for (int run = 0; run < num_runs; run++) {
                // Set number of threads
                omp_set_num_threads(thread_counts[t]);

                double start_time = omp_get_wtime();

                // Parallel loop with specified schedule
                #pragma omp parallel for
                for (int k = 1; k <= N; k++) {
                    // Create array of k elements
                    int *arr = (int*)malloc(k * sizeof(int));

                    // Fill with random values
                    fillArrayWithRandom(arr, k);

                    // using insertion sort
                    for (int i = 1; i < k; i++) {
                        int key = arr[i];
                        int j = i - 1;

                        while (j >= 0 && arr[j] > key) {
                            arr[j + 1] = arr[j];
                            j--;
                        }
                        arr[j + 1] = key;
                    }

                    // Free memory
                    free(arr);
                }

                double end_time = omp_get_wtime();
                total_time += (end_time - start_time);
            }

            double avg_time = total_time / num_runs;

            // Calculate speedup compared to single-threaded static schedule
            double speedup = 1.0;
            if (thread_counts[t] == 1 && strcmp(sched_types[s], "static") == 0) {
                baseline_time = avg_time;
                printf("%d, %s, %.6f, %.2f\n",
                       thread_counts[t], sched_types[s], avg_time, speedup);
            } else if (baseline_time > 0) {
                speedup = baseline_time / avg_time;
                printf("%d, %s, %.6f, %.2f\n",
                       thread_counts[t], sched_types[s], avg_time, speedup);
            } else {
                printf("%d, %s, %.6f, N/A\n",
                       thread_counts[t], sched_types[s], avg_time);
            }
        }
    }

    return 0;
}