// Author: Gianna Foti
// Teacher: Trevor H McClain
// Course: Operating Systems Laboratory - CPE 435-03
// Date: 12 February 2025
// Description: This program is the reader process. It waits for a message from 
// Process A in a shared memory segment. Once a new message is available, it 
// displays the message, acknowledges it by resetting the flag, and waits for 
// the next message. The process continues until it receives "Exit", signaling 
// that Process A has terminated, at which point Process B also exits.

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

    // Access existing message queue
    msgid = msgget(KEY, 0666);
    if (msgid == -1) {
        perror("msgget failed");
        exit(1);
    }

    printf("Process B: Chat Started. Waiting for messages...\n");

    while (1) {
        // Receive message from Process A
        if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 1, 0) == -1) {
            perror("msgrcv failed");
            exit(1);
        }

        // Properly format received message display
        printf("\n===================================\n");
        printf("Message from Process A: %s\n", msg.msg_text);
        printf("===================================\n");

        // Exit condition - Process A is terminating
        if (strcmp(msg.msg_text, "Exit") == 0) {
            // Send acknowledgment to Process A
            strcpy(msg.msg_text, "Exit command received. Ending chat.");
            msg.msg_type = 2;

            if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
                perror("msgsnd failed");
                exit(1);
            }

            printf("\nProcess A exit command received. Ending chat...\n");
            break;
        }

        // Get user response
        printf("You: ");
        fgets(msg.msg_text, MSG_SIZE, stdin);
        msg.msg_text[strcspn(msg.msg_text, "\n")] = 0; // Remove newline
        msg.msg_type = 2;

        // Send response back to Process A
        if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }

        // Exit condition - If user types Exit, send it to Process A
        if (strcmp(msg.msg_text, "Exit") == 0) {
            printf("\nExit command sent. Waiting for Process A acknowledgement...\n");
            break;
        }
    }

    msgctl(msgid, IPC_RMID, NULL);
    printf("Process B: Chat Ended.\n");
    return 0;
}
