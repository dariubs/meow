#include <stdio.h>
#include <string.h>

int repl() {
    for(;;) {
        printf("(meow)> ");
        char command[256];
        gets(command);

        if(!strcmp(command, "exit")) {
            printf(" by 🐱\n");
            return 0;
        }

        printf("%s\n", command);
    }
}