#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#ifdef _WIN32
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", pathsStr, typeis);
#else
        snprintf(fullPath, sizeof(fullPath), "%s/%s", pathsStr, typeis);
#endif
#ifdef _WIN32
        if (_access(fullPath, 0) == 0) {
            strncpy(foundPath, fullPath, foundPathSize - 1);
            foundPath[foundPathSize - 1] = '\0';
            int len = strlen(foundPath);
            free(pathCopy);
            return len;
        }
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
