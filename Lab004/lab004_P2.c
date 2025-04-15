
// Author: Gianna Foti
// Teacher: Trevor H McClain
// Course: Operating Systems Laboratory - CPE 435-03
// Date: 5 February 2025
// Description: This program is the reader process. It waits for two floating-point numbers 
// to be available in a shared memory segment, computes their sum, and displays the result. 
// The process continues to monitor the shared memory until the writer process signals termination.

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define KEY ((key_t)(1234))
#define MSIZ sizeof(struct info)

// Define shared memory structure
struct info {
    float value1, value2;
    float sum;
    int flag;
};

int main() {
    // Attach to existing shared memory
    int shmid = shmget(KEY, MSIZ, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    struct info *shm = (struct info *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    printf("---- Process 2 (Reader) ----\n");

    while (1) {
        while (shm->flag != 1) {
            if (shm->flag == -1) {
                printf("Termination signal received. Exiting.\n");
                shmdt(shm);
                exit(0);
            }
            usleep(500000); // Sleep for 500ms
        }

        shm->sum = shm->value1 + shm->value2;
        printf("%.2f + %.2f = %.2f\n", shm->value1, shm->value2, shm->sum);

        shm->flag = 0; // Indicate sum is computed
    }

    shmdt(shm);
    return 0;
}
