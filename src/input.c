#include "input.h"
#include "search.h"
#include "execute.h"

void get_input(char *input, size_t size) {
    if (fgets(input, size, stdin)) {
        if (!strchr(input, '\n')) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        input[strcspn(input, "\n")] = '\0';
    } else {
        printf("Error or EOF occurred\n");
        exit(EXIT_FAILURE);
    }
}

void parse_input(const char *input) {
    char command[100];
    sscanf(input, "%[^ ]", command);
    if (strcmp(command, "exit") == 0) {
        if (strlen(input) > 4 && input[4] == ' ') {
            int exit_code = atoi(&input[5]);
            exit(exit_code);
        } else {
            exit(EXIT_SUCCESS);
        }
    } else if (strcmp(command, "echo") == 0) {
        printf("%s\n", input + 5);
    } else if (strcmp(command, "type") == 0) {
        int lenth = strlen(command) + 1;
        char typeis[20];
        sscanf(input + lenth, "%[^ ]", typeis);
        if (strcmp(typeis, "type") == 0)
            printf("type is a shell builtin\n");
        else if (strcmp(typeis, "echo") == 0)
            printf("echo is a shell builtin\n");
        else if (strcmp(typeis, "exit") == 0)
            printf("exit is a shell builtin\n");
        else {
            char foundPath[PATH_MAX];
            int foundLen = search_in_path(typeis, foundPath, sizeof(foundPath));
            if (foundLen > 0)
                printf("%s is %s\n", typeis, foundPath);
            else
                printf("%s: not found\n", typeis);
        }
    } else {
        char foundPath[PATH_MAX];
        int foundLen = search_in_path(command, foundPath, sizeof(foundPath));
        if (foundLen > 0) {
            execute_command_with_args(foundPath, input);
        } else {
            printf("%s: not found\n", input);
        }
    }
}
