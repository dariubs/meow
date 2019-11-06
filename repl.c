#include <stdio.h>
#include <string.h>

int repl() {
    for(;;) {
        printf("(meow)> ");
        char command[256];
        
        if(fgets(command, sizeof command, stdin)) {
            command[strcspn(command, "\n")] = '\0';
        } else {
            return 0;
        }

        if(!strcmp(command, "exit")) {
            printf(" by 🐱\n");
            return 0;
        }

        printf("%s\n", command);
    }
}