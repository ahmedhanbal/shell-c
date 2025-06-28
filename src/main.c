#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h> // For _access
#include <windows.h>
#else
#include <unistd.h> // For access
#include <sys/types.h>
#include <sys/wait.h> // For waitpid
#endif
#define BUFF_SIZE 256
#define PATH_MAX 260


const char* get_basename(const char* path);
void execute_command_with_args(const char *cmdPath, const char *input);
int search_in_path(const char *typeis, char *foundPath, size_t foundPathSize);
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

// Helper to get basename from path
const char* get_basename(const char* path) {
    const char *base1 = strrchr(path, '/');
    const char *base2 = strrchr(path, '\\');
    const char *base = base1 > base2 ? base1 : base2;
    return base ? base + 1 : path;
}

// Helper to execute a command with arguments (cross-platform)
void execute_command_with_args(const char *cmdPath, const char *input) {
#ifdef _WIN32
    // Prepare command line: program name plus arguments
    char cmdLine[BUFF_SIZE * 2];
    const char *progName = get_basename(cmdPath);
    snprintf(cmdLine, sizeof(cmdLine), "\"%s\"", progName);
    // Extract arguments from input (skip command itself)
    const char *argsStart = strchr(input, ' ');
    if (argsStart && *(argsStart + 1) != '\0') {
        strncat(cmdLine, argsStart, sizeof(cmdLine) - strlen(cmdLine) - 1);
    }
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    // Use full path for lpApplicationName, but command line starts with program name
    BOOL success = CreateProcessA(
        (LPSTR)cmdPath, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
    );
    if (!success) {
        fprintf(stderr, "Failed to execute: %s\n", cmdPath);
        return;
    }
    // Wait for process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    if (exitCode != 0) {
        printf("Command exited with status %lu\n", exitCode);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
    // Prepare arguments array
    char argsCopy[BUFF_SIZE];
    snprintf(argsCopy, sizeof(argsCopy), "%s", input);
    char *argsStart = strchr(argsCopy, ' ');
    char *argv[BUFF_SIZE];
    int argc = 0;
    argv[argc++] = (char *)get_basename(cmdPath);
    if (argsStart) {
        // Split arguments
        char *token = strtok(argsStart + 1, " ");
        while (token && argc < BUFF_SIZE - 1) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
    }
    argv[argc] = NULL;
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        execv(cmdPath, argv);
        perror("execv failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus != 0) {
                printf("Command exited with status %d\n", exitStatus);
            }
        } else {
            printf("Command did not terminate normally\n");
        }
    }
#endif
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
        // Check if there's an argument after "exit"
        if (strlen(input) > 4 && input[4] == ' ') {
            int exit_code = atoi(&input[5]);
            exit(exit_code);
        } else {
            exit(EXIT_SUCCESS);
        }    } else if (strcmp(command, "echo") == 0) {
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
        // search for the command in PATH and execute it
        char foundPath[PATH_MAX];
        int foundLen = search_in_path(command, foundPath, sizeof(foundPath));
        if (foundLen > 0) {
            execute_command_with_args(foundPath, input);
        } else {
            printf("%s: not found\n", input);
        }
    }
}

// TODO 2: Add searching without extension like .exe
int search_in_path(const char *typeis, char *foundPath, size_t foundPathSize) {
    char *path = getenv("PATH");
    if (path == NULL) {
        return 0;
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

// Use access to check if file exists and is executable
#ifdef _WIN32
        if (_access(fullPath, 0) == 0) {
            strncpy(foundPath, fullPath, foundPathSize - 1);
            foundPath[foundPathSize - 1] = '\0';
            int len = strlen(foundPath);
            free(pathCopy);
            return len;
        }
        // Try with .exe extension if not already present
        if (strstr(typeis, ".exe") == NULL) {
            char exePath[PATH_MAX];
            snprintf(exePath, sizeof(exePath), "%s\\%s.exe", pathsStr, typeis);
            if (_access(exePath, 0) == 0) {
                strncpy(foundPath, exePath, foundPathSize - 1);
                foundPath[foundPathSize - 1] = '\0';
                int len = strlen(foundPath);
                free(pathCopy);
                return len;
            }
        }
#else
        if (access(fullPath, X_OK) == 0) {
            strncpy(foundPath, fullPath, foundPathSize - 1);
            foundPath[foundPathSize - 1] = '\0';
            int len = strlen(foundPath);
            free(pathCopy);
            return len;
        }
#endif

        // Next var in PATH
        pathsStr =
#ifdef _WIN32
            strtok(NULL, ";");
#else
            strtok(NULL, ":");
#endif
    }

    free(pathCopy);
    return 0;
}
