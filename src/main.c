#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EXIT_0 0
int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    // input buffer
    char input[100];
    while (1) {
        printf("$ ");
        if (fgets(input, sizeof(input), stdin)) {
            // this handles the case if input entered is larger than buffer size and there is leftover in stdin
            // Check if newline is present
            if (!strchr(input, '\n')) {
                // Newline not found → line was too long, flush remainder
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
            }
            // remove newline from input
            input[strcspn(input, "\n")] = '\0';
            if (strncmp(input,"exit",4) == 0){
              if (input[5] == '0')  
                return EXIT_0;
              else
                return atoi(&input[5]);
            } else
                printf("%s: command not found\n", input);
        } else {
            printf("Error or EOF occurred\n");
        }
    }

    return 0;
}
