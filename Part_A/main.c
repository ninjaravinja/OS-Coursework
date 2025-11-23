#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 1024

void change_dir(char *path) {
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

int detectPipe(char *command) {
    int count = 0;
    for (int i = 0; command[i] != '\0'; i++) {
        if (command[i] == '|') {
            count++;
        }
    }
    return count;
}

void tokeniseCommands(char *command, char **commands) {
    char *token = strtok(command, "|");
    int i = 0;
    while (token != NULL) {
        commands[i] = token;
        token = strtok(NULL, "|");
        i++;
    }
}

void exec_cmd(char *command) {
    char *args[MAX_CMD_LEN/2 + 1];
    pid_t pid;
    int status;

    char *token = strtok(command, " \n");
    int i = 0;
    while (token != NULL) {
        args[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }
    args[i] = NULL;

    if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
        exit(0);
    }

    if (args[0] != NULL && strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL) {
            change_dir(args[1]);
        }
        else {
            char curr_dir[MAX_CMD_LEN];
            if (getcwd(curr_dir, sizeof(curr_dir)) != NULL) {
                printf("%s\n", curr_dir);
            }
            else {
                perror("getcwd failed");
            }
        }
        return;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("exec failed");
            exit(1);
        }
    }
    else {
        waitpid(pid, &status, 0);
    }
}

//Plan: Split command through | and store in a list of commands. Iterate through list using a for loop and execute each command.


int main() {
    char command[MAX_CMD_LEN];

    while (true) {
        if (fgets(command, MAX_CMD_LEN, stdin) == NULL) {
            break;
        }

        int pipes = detectPipe(command);
        int cmds = pipes + 1;
        char **commands = malloc(cmds * sizeof(char *));

        tokeniseCommands(command, commands);
        
        for (int i = 0; i < cmds; i++) {
            exec_cmd(commands[i]);
        }
        
        free(commands);
    }

    return 0;
}