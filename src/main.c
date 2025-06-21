#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EXIT_0 0
int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    // input buffer
    char input[100];
    char command[100];
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

            sscanf(input,"%[^ ]",command);
            if (strcmp(command,"exit") == 0){
                // TODO: fix this hardcode ; we are using fgets to get input so starting spaces won't be ignored and might cause a problem here
                if (input[5] == '0')  
                return EXIT_0;
              else
                return atoi(&input[5]);
            } else if (strcmp(command,"echo") == 0) {
                // TODO: fix this hardcode ; we are using fgets to get input so starting spaces won't be ignored and might cause a problem here
                printf("%s\n",input +5);
            } 
            else
                printf("%s: command not found\n", input);
        } else {
            printf("Error or EOF occurred\n");
        }
    }

    return 0;
}
