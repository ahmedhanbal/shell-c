#include "util.h"
#include "input.h"

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

