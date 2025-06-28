
#include <stdio.h>
#include <stdlib.h>
#include "input.c"
#define BUFF_SIZE 256

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);
    char input[BUFF_SIZE];
    while (1) { // Loop
        printf("$ "); 
        get_input(input, sizeof(input)); // Read
        parse_input(input); // Execute
    }
    return 0;
}

