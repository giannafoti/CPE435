
// Author: Gianna Foti
// Teacher: Trevor McClain
// Course: CPE435-03
// Filename: lab002_P1.cpp
// Due Date: 22 January 2025
// Description: This program uses the fork() system call to create a child process.
//              The child process adds 2 to a variable and prints its PID and value.
//              The parent process adds 5 to the variable and prints its PID and value.


using namespace std;
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>  

int main() {
    // declaring and initialzing variable to 0 
    int val = 0;

    // calling da fork system call
    pid_t pid = fork();

    // child processsssss
    if (pid == 0) { 
        // adding 2 to the value of val
        val += 2; 
        cout << "Child Process: the PID is " << getpid() << " and the val is " << val << endl;
    } 
    // parent process
    else if (pid > 0) { 
        // adding 5 to the value of val
        val += 5; 
        cout << "Parent Process: the PID is " << getpid() << " and the val is " << val << endl;
    } 
    // my just in case error handling darlin
    else { 
        cout << "Fork failed! Something wrong brother" << endl;
        return 1;
    }
    return 0;
}

