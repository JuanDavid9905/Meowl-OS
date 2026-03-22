/*
 * Meowl OS - Advanced Linux Shared-Kernel Shell
 * ---------------------------------------------
 * Still NOT a kernel, NOT an OS, NOT a container system.
 * This is a user-space shell layer on Linux.
 *
 * Upgrades added:
 *  - command history (basic)
 *  - improved prompt
 *  - PATH-based execution (via execvp)
 *  - built-in cd, exit, history
 *  - still supports real Linux commands:
 *      git, wget, dnf, nano, ls, etc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 512
#define MAX_ARGS 64
#define HISTORY_SIZE 50

char *history[HISTORY_SIZE];
int history_count = 0;

void add_history(const char *cmd) {
    if (!cmd) return;

    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i - 1] = history[i];
        }
        history[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d  %s\n", i + 1, history[i]);
    }
}

void print_prompt() {
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    printf("meowl:%s$ ", cwd);
    fflush(stdout);
}

void run_command(char **args) {
    pid_t pid = fork();

    if (pid == 0) {
        execvp(args[0], args);
        perror("meowl");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("fork");
    }
}

void parse_and_run(char *input) {
    char *args[MAX_ARGS];
    int i = 0;

    add_history(input);

    char *token = strtok(input, " \n");
    while (token && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    if (!args[0]) return;

    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1]) {
            if (chdir(args[1]) != 0) perror("cd");
        }
        return;
    }

    if (strcmp(args[0], "history") == 0) {
        print_history();
        return;
    }

    run_command(args);
}

int main() {
    char input[MAX_INPUT];

    printf("Meowl OS Shell Upgrade Loaded\n");
    printf("Linux kernel shared mode active\n\n");

    while (1) {
        print_prompt();

        if (!fgets(input, sizeof(input), stdin)) break;

        parse_and_run(input);
    }

    return 0;
}
