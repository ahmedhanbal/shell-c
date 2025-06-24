#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFF_SIZE 256
#define PATH_MAX 260

void search_in_path(const char *typeis);
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
        else{
            search_in_path(typeis);
        }
    }
    else {
        printf("%s: not found\n", input);
    }
}

// TODO 2: Add searching without extension like .exe
void search_in_path(const char *typeis) {
    char *path = getenv("PATH");
    if (path == NULL) {
        printf("PATH not found\n");
        return;
    }
    char *pathCopy = strdup(path);
    char *pathsStr =
    #ifdef _WIN32
        strtok(pathCopy, ";");
    #else
        strtok(pathCopy, ":");
    #endif

    char fullPath[PATH_MAX];

    while (pathsStr != NULL) {
        // Construct full path: directory + "/" + executable name
        #ifdef _WIN32
            snprintf(fullPath, sizeof(fullPath), "%s\\%s", pathsStr, typeis);
        #else
            snprintf(fullPath, sizeof(fullPath), "%s/%s", pathsStr, typeis);
        #endif

        // Try opening the file to see if it exists
        FILE *file = fopen(fullPath, "r");
        if (file != NULL) {
            fclose(file);
            printf("%s is %s\n", typeis, fullPath);
            free(pathCopy);
            return;
        }

        // Next var in PATH
        pathsStr =
        #ifdef _WIN32
            strtok(NULL, ";");
        #else
            strtok(NULL, ":");
        #endif
    }

    printf("%s: not found\n", typeis);
    free(pathCopy);
}
