#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define DEFAULT_BUFFER_SIZE 5;

static int buffer_size = 5;

bool redirect_input(const size_t num_args, char *args[], char **input_file) {
    
    for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], "<") == 0) {
            *input_file = args[i+1];
            args[i] = NULL;

            for (int j = i; j < num_args - 2; j++){
                if (strcmp(args[j+2], "<") == 0 || strcmp(args[j+2], ">") == 0){
                    printf("Only one redirection is supported\n");
                    exit(1);
                }
                args[j] = args[j+2];
            }
            return true;
        }
    }
    return false;
}

bool redirect_output(const size_t num_args, char *args[], char **output_file) {

    for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], ">") == 0) {
            *output_file = args[i + 1];
            args[i] = NULL;

            for (int j = i; j < num_args - 2; j++){
                if (strcmp(args[j+2], "<") == 0 || strcmp(args[j+2], ">") == 0){
                    printf("Only one redirection is supported\n");
                    exit(1);
                }
                args[j] = args[j+2];
            }
            return true;
        } 
    }
    return false;
}

int find_pipe_index(const size_t num_args, char *args[]){
    // Varible to hold index number or returns -1 if not found.
    int index = -1;

    // Loop through arguments to find pipe index
    for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], "|") == 0) {
            index = i;
        }
    }
    return index;
}

void execute_pipe(int pipe_index, char *args[]) {
    
    char **lhs;
    char **rhs;


}

void execute_cmd(const size_t num_args, char *args[]) {

    pid_t pid = fork();
    char *file = NULL;

    int pipe_index = find_pipe_index(num_args, args);

    // Check if command has pipe and execute as necessary.
    if (pipe_index != -1) {
        execute_pipe(pipe_index, args);
    }

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(-1);
    } else if (pid == 0) {

        // Check for redirection input.
        if (redirect_input(num_args, args, &file)) {
                freopen(file, "r", stdin);
        }

        // Check for redirection output.
        if (redirect_output(num_args, args, &file)) {
                freopen(file, "w", stdout);
        }
                    
        int status = execvp(args[0], args);
            
        // Check if command is accurate.
        if (status == -1) {
                printf("Command not recognized\n");
                exit(1);
        }
        
    } else {
        wait(NULL);
    }

    free(file);
}

char* string_to_lowercase(char *input) {

    for (int i = 0; i < strlen(input); i++) {
        input[i] = tolower(input[i]);
    }
    return input;
}

void parse_cmd(char *input) {
    
    char *token;
    char **args;
    size_t num_args = 0;
    const char *delimter = " \t\r\n";

    args = malloc(buffer_size * sizeof(char *));
    // Tokenize the arguments in the command.
    token = strtok(input, delimter);

    while (token != NULL) {
        args[num_args] = malloc(strlen(token) + 1 * sizeof(char));
        strcpy(args[num_args], token);
        num_args++;

        // Reallocate memory as needed.
        if (num_args >= buffer_size){
            buffer_size *= 2;
            size_t new_size =  buffer_size * sizeof(char *);
            args = realloc(args, new_size);
            if (args == NULL) {
                fprintf(stderr, "Failed to allocate %d elements, totaling %zu bytes\n", buffer_size, new_size);
                exit(errno);
            }
        }
        
        token = strtok(NULL, delimter);
    }

    execute_cmd(num_args, args);
    // Return buffer size to default size.
    buffer_size = DEFAULT_BUFFER_SIZE;
    // Free memory allocated for commands.
    for (int i = 0; i < (num_args + 1); i++) {
        free(args[i]);
    }
    free(args);
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