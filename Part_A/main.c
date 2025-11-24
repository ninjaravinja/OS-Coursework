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

void removeCD(char **commands, int *cmds){
    for (int i = 0; i < *cmds; i++) {
        char *cmd = commands[i];
        while (*cmd == ' ' || *cmd == '\t') {
            cmd++;
        }
        if (strcmp(cmd, "cd") == 0) {
            for (int j = i; j < *cmds - 1; j++) {
                commands[j] = commands[j + 1];
            }
            (*cmds)--;
            break;
        }
    }
}

int exec_cmd(char *command, int inputFd, bool next) {
    char *args[MAX_CMD_LEN/2 + 1];
    pid_t pid;
    int status;

    int pipeFd[2];
    int outputFd = -1;

    if (next) {
        if (pipe(pipeFd) == -1) {
            perror("failed to pipe");
            return -1;
        }
        outputFd = pipeFd[0];
    }

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
        return outputFd;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return outputFd;
    }


    if (pid == 0) {
        if (inputFd != -1) {
            dup2(inputFd, STDIN_FILENO);
            close(inputFd);
        }

        if (next) {
            dup2(pipeFd[1], STDOUT_FILENO);
            close(pipeFd[0]);
            close(pipeFd[1]);
        }

        if (execvp(args[0], args) == -1) {
            perror("exec failed");
            exit(1);
        }
    }
    else {
        if (inputFd != -1) {
            close(inputFd);
        }
        if (next) {
            close(pipeFd[1]);
        }
        waitpid(pid, &status, 0);
    }

    return outputFd;
}


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

        if (pipes > 0) {
            removeCD(commands, &cmds);
        }
        
        int prevFd = -1;
        for (int i = 0; i < cmds; i++) {
            bool next = (i < cmds - 1);
            prevFd = exec_cmd(commands[i], prevFd, next);
        }
        
        free(commands);
    }

    return 0;
}