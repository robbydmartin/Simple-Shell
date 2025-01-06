#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int LINE_MAX = 2048;

void execute_cmd(char *cmds[]) {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(-1);
    } else if (pid == 0) {
        execvp(cmds[0], cmds);
    } else {
        wait(NULL);
    }
}

char* string_to_lowercase(char* input) {

    for (int i = 0; i < strlen(input); i++) {
        input[i] = tolower(input[i]);
    }
    return input;
}

void parse_cmd(char* input) {
    
    char *token;
    char **cmds;
    int count = 0;
    const char *delimter = " \t\r\n";

    cmds = malloc(LINE_MAX * sizeof(char *));
    token = strtok(input, delimter);

    while (token != NULL) {
        cmds[count] = malloc(strlen(token) + 1 * sizeof(char));
        strcpy(cmds[count], token);
        count++;
        token = strtok(NULL, delimter);
    }

    execute_cmd(cmds);

    for (int i = 0; i < (count + 1); i++) {
        free(cmds[i]);
    }

}

int main(int argc, char **argv) {

    char *input;
    size_t n = 0;

    // Shell prompt.
    printf(">>");
    // Get user input.
    getline(&input, &n, stdin);

    while (strcmp((string_to_lowercase(input)), "quit\n")) {
        // Call to parsing function.
        parse_cmd(input);
        // Get user input.
        getline(&input, &n, stdin);
    }

    return 0;
}