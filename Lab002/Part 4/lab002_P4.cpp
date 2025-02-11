// Author: Gianna Foti
// Teacher: Trevor McClain
// Course: CPE435-03
// Due Date: 22 January 2025
// Description: This program demonstrates orphan, zombie, and sleeping beauty process states in Linux.

#include <iostream>     
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/wait.h>   

using namespace std;

// shows an orphan process by terminating the parent and leaving the child to be adopted by init
void aloneOrphan() {
    pid_t pid = fork();  

    if (pid == 0) {
        sleep(2);  
        cout << "Orphan: Orphan Child (PID: " << getpid() << "), adopted by init, parent: " << getppid() << endl;
    }
    else {
        cout << "Orphan: Parent (PID: " << getpid() << ") exiting, orphaning child." << endl;
        exit(0);  
    }
}

// shows a zombie process by allowing the child to exit before the parent calls wait()
void createZomboss() {
    pid_t pid = fork();  

    if (pid == 0) {
        cout << "Zombie: Zombie Child (PID: " << getpid() << ") exiting." << endl;
        exit(0);  
    }
    else {
        cout << "Zombie: Parent (PID: " << getpid() << ") exiting without waiting for child." << endl;
        sleep(7); 
        wait(NULL);
    }
}

// creates a child that sleeps for a set time while the parent waits
void sleepyBeauty() {
    pid_t pid = fork();  

    if (pid == 0) {
        cout << "Sleepy: Sleeping Beauty Process (PID: " << getpid() << ") sleeping." << endl;
        sleep(8);  
        cout << "Sleepy: Sleeping Beauty Process (PID: " << getpid() << ") waking up." << endl;
        exit(0);  
    }
    else {
        wait(NULL);  
    }
}

// main function with the cout statements
int main() {
    cout << "=== Demonstrating Orphan Process ===" << endl;
    aloneOrphan();
    sleep(1);  

    cout << "=== Demonstrating Zombie Process ===" << endl;
    createZomboss();
    sleep(1);  

    cout << "=== Demonstrating Sleeping Beauty Process ===" << endl;
    sleepyBeauty();
    sleep(5);  

    cout << "All processes shown" << endl;
    return 0;
}
