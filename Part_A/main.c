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


int main() {
    char command[MAX_CMD_LEN];

    while (true) {
        if (fgets(command, MAX_CMD_LEN, stdin) == NULL) {
            break;
        }

        exec_cmd(command);
    }

    return 0;
}