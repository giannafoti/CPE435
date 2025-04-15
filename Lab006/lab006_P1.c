// Author: Gianna Foti
// Teacher: Trevor H McClain
// Course: Operating Systems Laboratory - CPE 435-03
// Date: 12 February 2025
// Description: This program computes the numerical approximation of a definite integral 
// using the rectangular decomposition method in a serial manner. It approximates the 
// integral of the function f(x) = 4 / (1 + x^2) over the interval [0,1], which 
// estimates the value of π. The number of intervals is provided as a command-line 
// argument, and the execution time is measured to analyze performance.


#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard library functions (e.g., atoi)
#include <math.h>       // Math functions (e.g., sqrt, pow)
#include <sys/time.h>   // Timing functions for performance measurement

// Timer utility functions for measuring execution time
struct timeval tv1, tv2;  // Structs for storing start and end times
#define TIMER_CLEAR (tv1.tv_sec = tv1.tv_usec = tv2.tv_sec = tv2.tv_usec = 0) // Reset timer values
#define TIMER_START gettimeofday(&tv1, NULL) // Start the timer
#define TIMER_STOP gettimeofday(&tv2, NULL)  // Stop the timer
#define TIMER_ELAPSED (double)(tv2.tv_usec - tv1.tv_usec) / 1000000.0 + (tv2.tv_sec - tv1.tv_sec) // Compute elapsed time

// Function to compute the function being integrated
// The function used here is f(x) = 4 / (1 + x^2), which approximates π when integrated from 0 to 1
double f(double x) {
    return 4.0 / (1.0 + x * x);
}


// Function to compute the integral using rectangular decomposition (serial version)
double compute_integral(int num_intervals) {
    double width = 1.0 / num_intervals; // Width of each rectangle
    double sum = 0.0; // Variable to accumulate function values

    int i = 0; // Initialize loop counter

    // Use a while loop to iterate through each interval
    while (i < num_intervals) {
        double x = (i + 0.5) * width; // Compute the midpoint of the current rectangle
        sum += f(x); // Add the function value at the midpoint to the sum
        i++; // Move to the next interval
    }

    return sum * width; // Multiply by width to get final integral approximation
}


int main(int argc, char *argv[]) {
    // Ensure correct number of command-line arguments
    if (argc != 2) {
        printf("Usage: %s <num_intervals>\n", argv[0]);
        return -1;
    }

    // Convert input argument to an integer representing the number of intervals
    int num_intervals = atoi(argv[1]);
    
    // Validate the number of intervals (must be positive)
    if (num_intervals <= 0) {
        printf("Error: Number of intervals must be positive.\n");
        return -1;
    }

    // Start the timer before computation
    TIMER_CLEAR;
    TIMER_START;

    // Compute the integral using the serial method
    double result = compute_integral(num_intervals);

    // Stop the timer after computation
    TIMER_STOP;

    // Print the computed integral and execution time
    printf("Computed integral (Serial): %.8f\n", result);
    printf("Execution time: %f seconds\n", TIMER_ELAPSED);

    return 0; // Indicate successful execution
}
