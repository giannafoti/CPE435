// Author: Gianna Foti
// Teacher: Trevor H McClain
// Course: Operating Systems Laboratory - CPE 435-03
// Date: 12 February 2025
// Description: This program computes the numerical approximation of a definite integral 
// using the rectangular decomposition method with POSIX threads (Pthreads) for parallelization. 
// It approximates the integral of the function f(x) = 4 / (1 + x^2) over the interval [0,1], 
// which estimates the value of π. The user specifies the number of intervals and threads via 
// command-line arguments, and execution time is measured for performance analysis.

#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions (e.g., strtol, malloc)
#include <math.h>       // Math functions (e.g., sqrt, pow)
#include <pthread.h>    // POSIX Threads (Pthreads) library for multi-threading
#include <sys/time.h>   // Timing functions for performance measurement

// Timer utility functions for measuring execution time
struct timeval tv1, tv2;  // Structs for storing start and end times
#define TIMER_CLEAR (tv1.tv_sec = tv1.tv_usec = tv2.tv_sec = tv2.tv_usec = 0) // Reset timer values
#define TIMER_START gettimeofday(&tv1, NULL) // Start the timer
#define TIMER_STOP gettimeofday(&tv2, NULL)  // Stop the timer
#define TIMER_ELAPSED (double)(tv2.tv_usec - tv1.tv_usec) / 1000000.0 + (tv2.tv_sec - tv1.tv_sec) // Compute elapsed time

// Structure to hold data for each thread
// Pthreads only allows a single argument to be passed to the thread function. 
// Since each thread needs multiple values (start, end, width, sum), 
// I used a struct to bundle them into a single argument. This makes the code more organized,
// avoids memory allocation issues, and ensures each thread gets the correct data

typedef struct {
    int start;      // Start index for the thread's portion of the integral computation
    int end;        // End index for the thread's portion
    double width;   // Width of each rectangle in rectangular decomposition
    double sum;     // Partial sum computed by the thread
} ThreadData;

// Function executed by each thread to compute its portion of the integral
void *compute_integral_parallel(void *arg) {
    ThreadData *data = (ThreadData *)arg;  // Cast argument to ThreadData structure
    double localSum = 0.0;  // Local sum for the thread

    // Compute the sum of function values at midpoints of assigned intervals
    for (int i = data->start; i < data->end; i++) {
        double x = (i + 0.5) * data->width; // Midpoint of the rectangle
        localSum += 4.0 / (1.0 + x * x);  // f(x) = 4 / (1 + x^2)
    }

    data->sum = localSum * data->width;  // Multiply by width to get partial integral result
    pthread_exit(NULL);  // Exit the thread
}

int main(int argc, char *argv[]) {
    // Ensure correct number of command-line arguments
    if (argc != 3) {
        printf("Usage: %s <num_intervals> <num_threads>\n", argv[0]);
        return -1;
    }

    // Convert input arguments to integers safely
    char *endptr;
    long num_intervals = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || num_intervals <= 0) {
        printf("Error: Number of intervals must be a positive integer.\n");
        return -1;
    }

    long num_threads = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || num_threads <= 0 || num_intervals < num_threads) {
        printf("Error: Number of threads must be a positive integer and ≤ number of intervals.\n");
        return -1;
    }

    pthread_t threads[num_threads];        // Array to hold thread identifiers
    ThreadData thread_data[num_threads];   // Array to hold thread data
    double width = 1.0 / num_intervals;    // Compute width of each rectangle
    double total_sum = 0.0;                // Final sum of all partial integrals

    // Start the timer before computation
    TIMER_CLEAR;
    TIMER_START;

    // Divide work among threads
    int chunk_size = num_intervals / num_threads;  // Divide work into equal chunks

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start = i * chunk_size;  // Start index for this thread
        thread_data[i].end = (i == num_threads - 1) ? num_intervals : (i + 1) * chunk_size; // Last thread gets remaining work
        thread_data[i].width = width;  // Set the width of each interval
        thread_data[i].sum = 0.0;  // Initialize sum to 0

        // Create the thread and assign it the computation task
        if (pthread_create(&threads[i], NULL, compute_integral_parallel, (void *)&thread_data[i]) != 0) {
            printf("Error: Failed to create thread %d\n", i);
            return -1;
        }
    }

    // Wait for all threads to finish execution and accumulate their results
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].sum;  // Accumulate partial results from all threads
    }

    // Stop the timer after computation
    TIMER_STOP;

    // Print the computed integral and execution time
    printf("Computed integral (Parallel with %ld threads): %.8f\n", num_threads, total_sum);
    printf("Execution time: %f seconds\n", TIMER_ELAPSED);

    return 0;  // Indicate successful execution
}
