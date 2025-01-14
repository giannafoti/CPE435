//Author: Gianna Foti
//Teacher: Trevor H McClain
//Course: CPE 435
//Due Date: 15 January 2025
//Description: Expand the program in Demo 1 and fork off 10 children, each of which will print out its pid and its serial number (1,2,3,4,5...10).


using namespace std;
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>  // for wait()

int main(int argc, char *argv[])
{
    // printing the PID of the parent process
    cout << "The pid of the parent is " << getpid() << endl;

//start at 1 and loops to create 10 child processes 
    for (int i = 1; i <= 10; i++) {
        pid_t c_pid = fork();

// doing a lil failsafe option here, totally optional 
        if (c_pid < 0) {
            cout << "Fork failed!" << endl;
            exit(1);
        }
        // child process block
        else if (c_pid == 0) {  
            //each child prints the serial number, its PID, and its parents PID
            cout << "I am child number " << i 
                 << ", my PID is " << getpid() 
                 << ", and my parent's PID is " << getppid() << endl;
            // child exits to prevent more forking
            exit(0);  
        }
    }

    // parental waits for all child 10 processes to finish
    for (int i = 0; i < 10; i++) {
        // this waits for any child to terminate
        wait(NULL);
    }

    //lil ending statement that says that everything has finished
    cout << "All child processes have finished. Parent process exiting." << endl;

    return 0;
}
