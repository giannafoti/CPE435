/*
Author: Gianna Foti
Teacher: Trevor H McClain
Course: Operating Systems Laboratory - CPE 435-03
Date: 5 February 2025
Description: Custom Linux shell with support for I/O redirection, pipes, and built-in commands.
*/

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>

using namespace std;
#define MAX_ARGS 20

// Function to parse command input using strtok()
vector<string> tokenizeCommand(const string& command, const char* delimiters) {
    vector<string> tokens;
    char cmd[command.length() + 1];
    strcpy(cmd, command.c_str());  // Convert string to C-style char array for strtok()
    char* token = strtok(cmd, delimiters);
    while (token != NULL) {
        tokens.push_back(token);  // Store each token (command argument)
        token = strtok(NULL, delimiters);
    }
    return tokens;
}

// Execute a single command
void executeCommand(const vector<string>& args) {
    char* argv[MAX_ARGS];
    for (size_t i = 0; i < args.size(); ++i) {
        argv[i] = const_cast<char*>(args[i].c_str());  // Convert string to char* for execvp
    }
    argv[args.size()] = NULL;  // Null-terminate the argument list
    
    pid_t pid = fork();  // Create a child process
    if (pid == 0) {  // Child process
        if (execvp(argv[0], argv) == -1) {  // Execute command
            perror("execvp failed");  // Print error if execvp fails
            exit(EXIT_FAILURE);
        }
    } else {  // Parent process
        wait(NULL);  // Wait for child process to finish
    }
}

// Handle I/O redirection
void handleIORedirection(vector<string>& args) {
    int saved_stdout = dup(STDOUT_FILENO); // Save the original stdout
    int saved_stdin = dup(STDIN_FILENO);   // Save the original stdin

    // Check for redirection symbols
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == ">" || args[i] == ">>") {  // Output redirection
            int flags = (args[i] == ">") ? (O_WRONLY | O_CREAT | O_TRUNC) : (O_WRONLY | O_CREAT | O_APPEND);
            int fd = open(args[i + 1].c_str(), flags, 0644);
            if (fd == -1) {
                perror("open failed");  // Print error if file cannot be opened
                return;
            }
            dup2(fd, STDOUT_FILENO);  // Redirect stdout to file
            close(fd);
            args.resize(i);  // Remove '>' and filename from args
            break;
        } else if (args[i] == "<") {  // Input redirection
            int fd = open(args[i + 1].c_str(), O_RDONLY);
            if (fd == -1) {
                perror("open failed");  // Print error if file cannot be opened
                return;
            }
            dup2(fd, STDIN_FILENO);  // Redirect stdin from file
            close(fd);
            args.resize(i);  // Remove '<' and filename from args
            break;
        }
    }

    pid_t pid = fork();
    if (pid == 0) {  // Child process executes command with redirection
        executeCommand(args);
        exit(EXIT_SUCCESS);
    } else {  // Parent process waits for child
        wait(NULL);
    }

    // Restore original stdout and stdin after executing the command
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdout);
    close(saved_stdin);
}

// Handle piped commands
void handlePipes(vector<string>& args1, vector<string>& args2) {
    int fd[2];
    pipe(fd);  // Create a pipe
    
    pid_t pid1 = fork();
    if (pid1 == 0) {  // First child (executing first command)
        dup2(fd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(fd[0]);  // Close unused read end of the pipe
        close(fd[1]);
        executeCommand(args1);  // Execute first command
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {  // Second child (executing second command)
        dup2(fd[0], STDIN_FILENO);  // Redirect stdin from pipe
        close(fd[1]);  // Close unused write end of the pipe
        close(fd[0]);

        // Check for output redirection in second command
        int output_fd = -1;
        for (size_t i = 0; i < args2.size(); ++i) {
            if (args2[i] == ">" || args2[i] == ">>") {
                int flags = (args2[i] == ">") ? (O_WRONLY | O_CREAT | O_TRUNC) : (O_WRONLY | O_CREAT | O_APPEND);
                output_fd = open(args2[i + 1].c_str(), flags, 0644);
                if (output_fd == -1) {
                    perror("open failed");  // Print error if file cannot be opened
                    exit(EXIT_FAILURE);
                }
                dup2(output_fd, STDOUT_FILENO);  // Redirect output
                close(output_fd);
                args2.resize(i);  // Remove `>` and filename from args
                break;
            }
        }

        executeCommand(args2);  // Execute second command
        exit(0);
    }
    
    close(fd[0]);  // Close both ends of the pipe in parent
    close(fd[1]);
    waitpid(pid1, NULL, 0);  // Wait for first process to finish
    waitpid(pid2, NULL, 0);  // Wait for second process to finish
}

// Handle built-in commands like cd and exit
bool handleBuiltInCommands(const vector<string>& args) {
    if (args.empty()) return false;  // Return false if no command is given
    if (args[0] == "exit") {  // If command is "exit"
        exit(0);  // Exit the shell
    } else if (args[0] == "cd") {  // If command is "cd"
        if (args.size() < 2) {
            cerr << "cd: missing argument" << endl;  // Print error if no directory is provided
        } else if (chdir(args[1].c_str()) != 0) {  // Change directory
            perror("cd failed");  // Print error if chdir() fails
        }
        return true;  // Indicate that the built-in command was handled
    }
    return false;  // Command is not built-in
}

// Main shell loop
void startShell() {
    string command;
    while (true) {  // Infinite loop to keep the shell running
        cout << "GiMoney$ ";  // Display shell prompt
        getline(cin, command);  // Read user input
        
        size_t pipePos = command.find('|');  // Check if input contains a pipe
        if (pipePos != string::npos) {  // If a pipe is found
            vector<string> args1 = tokenizeCommand(command.substr(0, pipePos), " ");
            vector<string> args2 = tokenizeCommand(command.substr(pipePos + 1), " ");
            handlePipes(args1, args2);  // Handle piped commands
        } else {
            vector<string> args = tokenizeCommand(command, " ");
            if (!handleBuiltInCommands(args)) {  // Check if command is built-in
                handleIORedirection(args);  // Handle I/O redirection if necessary
            }
        }
    }
}

int main() {
    startShell();  // Start the shell loop
    return 0;
}
