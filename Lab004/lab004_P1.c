/*
Author: Gianna Foti
Teacher: Trevor H McClain
Course: Operating Systems Laboratory - CPE 435-03
Date: 5 February 2025
Description: This program is the writer process. It takes two floating-point numbers 
from the user, stores them in a shared memory segment, and sets a flag indicating that 
new data is available. The process continues until the user chooses to exit, signaling 
the reader process to terminate.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define KEY ((key_t)(1234))  // Shared Memory Key from Lab Instructions
#define MSIZ sizeof(struct info)

// Define the shared memory structure
struct info {
    float value1, value2;
    float sum;
    int flag;
};

int main() {
    // Create shared memory
    int shmid = shmget(KEY, MSIZ, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Attach to shared memory
    struct info *shm = (struct info *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    shm->flag = 0; // Initialize flag

    printf("---- Process 1 (Writer) ----\n");

    while (1) {
        printf("\nEnter first number: ");
        scanf("%f", &shm->value1);
        
        printf("Enter second number: ");
        scanf("%f", &shm->value2);

        shm->flag = 1; // Signal Process 2 to compute sum

        while (shm->flag == 1) {
            usleep(500000); // Sleep for 500ms for better responsiveness
        }

        int choice;
        while (1) {
            printf("Continue? Yes(1) No(-1): ");
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input. Enter 1 to continue, -1 to exit.\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
            if (choice == -1) {
                shm->flag = -1; // Signal termination
                shmdt(shm);     // Detach shared memory
                shmctl(shmid, IPC_RMID, NULL); // Deallocate shared memory
                printf("Shared memory removed. Exiting...\n");
                exit(0);
            } else if (choice == 1) {
                break;
            } else {
                printf("Invalid input. Enter 1 to continue, -1 to exit.\n");
            }
        }
    }

    return 0;
}
