// Author: Gianna Foti
// Teacher: Trevor H McClain
// Course: Operating Systems Laboratory - CPE 435-03
// Date: 12 February 2025
// Description: This program is the writer process. It takes a user-inputted message, 
// stores it in a shared memory segment, and sets a flag indicating that new data is 
// available. The process continues until the user types "Exit", signaling the reader 
// process to terminate.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define KEY 5555
#define MSG_SIZE 256

// Define message structure
struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    int msgid;
    struct msg_buffer msg;

    // Create message queue
    msgid = msgget(KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }

    printf("Process A: Chat Started. Type 'Exit' to quit.\n");

    while (1) {
        // Get user input
        printf("\nYou: ");
        fgets(msg.msg_text, MSG_SIZE, stdin);
        msg.msg_text[strcspn(msg.msg_text, "\n")] = 0; // Remove newline
        msg.msg_type = 1;

        // Send message to queue
        if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }

        // Exit condition
        if (strcmp(msg.msg_text, "Exit") == 0) {
            printf("\nExit command sent. Waiting for acknowledgment...\n");

            // Wait for acknowledgment from Process B
            if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 2, 0) == -1) {
                perror("msgrcv failed");
                exit(1);
            }

            // Display acknowledgment message
            printf("\n===================================\n");
            printf("%s\n", msg.msg_text);
            printf("===================================\n");
            break;
        }

        // Wait for response from Process B
        if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 2, 0) == -1) {
            perror("msgrcv failed");
            exit(1);
        }

        // If Process B sent "Exit", handle termination
        if (strcmp(msg.msg_text, "Exit") == 0) {
            printf("\nProcess B exit command received. Ending chat...\n");
            break;
        }

        // Properly format received message display
        printf("\n===================================\n");
        printf("Message from Process B: %s\n", msg.msg_text);
        printf("===================================\n");
    }

    // **Process A removes the message queue**
    msgctl(msgid, IPC_RMID, NULL);
    printf("Message queue removed. Chat Ended.\n");
    return 0;
}
