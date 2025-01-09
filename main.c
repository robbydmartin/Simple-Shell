#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define DEFAULT_BUFFER_SIZE 5;

static int buffer_size = 5;

void execute_cmd(char *cmds[]) {

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(-1);
    } else if (pid == 0) {
        int status = execvp(cmds[0], cmds);
        // Check if command is accurate.
        if (status == -1) {
            printf("Command not recognized\n");
            exit(1);
        }
        
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
    size_t num_args = 0;
    const char *delimter = " \t\r\n";

    cmds = malloc(buffer_size * sizeof(char *));
    // Tokenize the arguments in the command.
    token = strtok(input, delimter);

    while (token != NULL) {
        cmds[num_args] = malloc(strlen(token) + 1 * sizeof(char));
        strcpy(cmds[num_args], token);
        num_args++;

        // Reallocate memory as needed.
        if (num_args >= buffer_size){
            buffer_size *= 2;
            size_t new_size =  buffer_size * sizeof(char *);
            cmds = realloc(cmds, new_size);
            if (cmds == NULL) {
                fprintf(stderr, "Failed to allocate %d elements, totaling %zu bytes\n", buffer_size, new_size);
                exit(errno);
            }
        }
        
        token = strtok(NULL, delimter);
    }

    execute_cmd(cmds);
    // Return buffer size to default size.
    buffer_size = DEFAULT_BUFFER_SIZE;
    // Free memory allocated for commands.
    for (int i = 0; i < (num_args + 1); i++) {
        free(cmds[i]);
    }
    free(cmds);
}

void execute_help() {
    printf("\n-------- Help --------\n");
    printf("Commands supported:\n");
    printf("- Singular pipes: foo | bar\n");
    printf("- Redirection:\n");
    printf("\t foo < bar\n");
    printf("\t foo > bar\n");
    printf("- Help message: type \"help\"\n");
    printf("- End shell: type \"quit\"\n");
}

int main(int argc, char **argv) {

    char *input;
    size_t n = 0;
    // Array for current working directory.
    char *cwd = (char *)malloc(LINE_MAX);

    // Shell prompt.
    if(getcwd(cwd, LINE_MAX) != NULL) {
        printf("%s>> ", cwd);
    } else {
        fprintf(stderr, "Failed to get current directory\n");
    }

    // Get user input.
    getline(&input, &n, stdin);

    while (strcmp((string_to_lowercase(input)), "quit\n")) {

        // Check if user wants to display help menu.
        if (strcmp((string_to_lowercase(input)), "help\n") == 0) {
            execute_help();
        } else {
            // Call to parsing function.
            parse_cmd(input);
        }

        // Shell prompt.
        if (getcwd(cwd, LINE_MAX) != NULL) {
            printf("%s>> ", cwd);
        } else {
            fprintf(stderr, "Failed to get current directory\n");
            printf(">> ");
        }

        // Get user input.
        getline(&input, &n, stdin);
    }

    return 0;
}