#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");

  // Wait for user input
  char input[100];
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
}

  printf("%s: command not found\n",input);
  return 0;
}
