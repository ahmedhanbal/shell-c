#include "execute.h"

const char* get_basename(const char* path) {
    const char *base1 = strrchr(path, '/');
    const char *base2 = strrchr(path, '\\');
    const char *base = base1 > base2 ? base1 : base2;
    return base ? base + 1 : path;
}

void execute_command_with_args(const char *cmdPath, const char *input) {
#ifdef _WIN32
    char cmdLine[BUFF_SIZE * 2];
    const char *progName = get_basename(cmdPath);
    snprintf(cmdLine, sizeof(cmdLine), "\"%s\"", progName);
    const char *argsStart = strchr(input, ' ');
    if (argsStart && *(argsStart + 1) != '\0') {
        strncat(cmdLine, argsStart, sizeof(cmdLine) - strlen(cmdLine) - 1);
    }
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    BOOL success = CreateProcessA((LPSTR)cmdPath, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!success) {
        fprintf(stderr, "Failed to execute: %s\n", cmdPath);
        return;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    if (exitCode != 0) {
        printf("Command exited with status %lu\n", exitCode);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
    char argsCopy[BUFF_SIZE];
    snprintf(argsCopy, sizeof(argsCopy), "%s", input);
    char *argsStart = strchr(argsCopy, ' ');
    char *argv[BUFF_SIZE];
    int argc = 0;
    argv[argc++] = (char *)get_basename(cmdPath);
    if (argsStart) {
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

