#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFF_SIZE 256

void get_input(char *input, size_t size);
void parse_input(const char *input);

int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    char input[BUFF_SIZE];

    while (1) {
        printf("$ ");
        get_input(input, sizeof(input));
        parse_input(input);
    }

    return 0;
}

void get_input(char *input, size_t size) {
    if (fgets(input, size, stdin)) {
        // this handles the case if input entered is larger than buffer size and there is leftover in stdin
        if (!strchr(input, '\n')) {
            // Newline not found → line was too long, flush remainder
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        // remove newline from input
        input[strcspn(input, "\n")] = '\0';
    } else {
        printf("Error or EOF occurred\n");
        exit(EXIT_FAILURE);
    }
}


// TODOs : 
// 1. fix this hardcode ; we are using fgets to get input so starting spaces won't be ignored and might cause a problem
void parse_input(const char *input) {
    char command[100];
    sscanf(input, "%[^ ]", command);

    if (strcmp(command, "exit") == 0) { 
        if (input[5] == '0') 
            exit(EXIT_SUCCESS);
        else
            exit(atoi(&input[5]));  
    } else if (strcmp(command, "echo") == 0) { 
        printf("%s\n", input + 5); 
    } else if (strcmp(command, "type") == 0) {
        int lenth = strlen(command) + 1;
        char typeis[20];
        sscanf(input+lenth,"%[^ ]",typeis);
        if (strcmp(typeis, "type") == 0 )
            printf("type is a shell builtin\n");
        else if (strcmp(typeis, "echo") == 0 )
            printf("echo is a shell builtin\n");
        else if (strcmp(typeis, "exit") == 0 )
            printf("exit is a shell builtin\n");
        else
            printf("%s: not found\n",typeis);
    }
    else {
        printf("%s: command not found\n", input);
    }
}
