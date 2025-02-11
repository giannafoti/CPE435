// Author: Gianna Foti
// Teacher: Trevor McClain
// Course: CPE435-03
// Due Date: 22 January 2025
// Description: This program demonstrates process creation and synchronization using fork().
//              It creates two child processes to perform subtraction, addition, and multiplication.
// i used the numbers 10 and 3 
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// Function to subtract two numbers
void subtract(int a, int b) {
    cout << "Child1 (PID: " << getpid() << ") Subtraction Result: " << (a - b) << endl;
}

// Function to add two numbers
void add(int a, int b) {
    cout << "Child2 (PID: " << getpid() << ") Addition Result: " << (a + b) << endl;
}

// Function to multiply two numbers
void multiply(int a, int b) {
    cout << "Child1 (PID: " << getpid() << ") Multiplication Result: " << (a * b) << endl;
}

int main() {
    pid_t child1_pid, child2_pid;

    // Create child1 process
    child1_pid = fork();

    if (child1_pid < 0) {
        cerr << "Fork failed for child1" << endl;
        return 1;
    }

    if (child1_pid == 0) {  // Child1 process
        subtract(10, 3);  // Perform subtraction

        // Create child2 process
        child2_pid = fork();

        if (child2_pid < 0) {
            cerr << "Fork failed for child2" << endl;
            return 1;
        }

        if (child2_pid == 0) {  // Child2 process
            add(10, 3);  // Perform addition
            return 0;   // Exit child2
        }

        wait(NULL);  // Wait for child2 to finish
        multiply(10, 3);  // Perform multiplication
        return 0;  // Exit child1
    }

    // Parent process
    cout << "Parent (PID: " << getpid() << ") waiting for child1 (PID: " << child1_pid << ")" << endl;
    wait(NULL);  // Wait for child1 to finish

    cout << "Parent (PID: " << getpid() << ") finished execution." << endl;
    return 0;
}
