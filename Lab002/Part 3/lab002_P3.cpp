// Author: Gianna Foti
// Teacher: Trevor McClain
// Course: CPE435-03
// Due Date: 22 January 2025
// Description: This program creates 'n' child processes using the fork() system call, where 'n' 
//              is an even number provided by the user. If an odd number is entered, the program 
//              displays an error message and terminates. The parent process creates all child 
//              processes, waits for their completion, and then exits.

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

int main() {
    int n;

    // malke the user give an input
    cout << "Enter an even number of child processes to create: ";
    cin >> n;

    // checking if n is even; if not then terminate the program
    if (n % 2 != 0) {
        cout << "Error: The number entered is odd. The program will now terminate." << endl;
        return 1;  // Exit with error code
    }

    cout << "Parent process ID: " << getpid() << endl;

    // loopy loop to create n child processes
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            cerr << "Fork failed!" << endl;
            exit(1);
        } else if (pid == 0) {
            // Child process
            cout << "Child " << i + 1 << " with PID: " << getpid() << " created by parent PID: " << getppid() << endl;
            exit(0);  // ensures child terminates after printing
        }
    }

    // parent process waits for all child processes to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    cout << "All child processes have terminated. Parent process exiting." << endl;
    return 0;
}
