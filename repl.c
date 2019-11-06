#include <stdio.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

int repl() {
    char *command;
    for(;;) {        
        command = readline("(meow)> ");
        add_history(command);
        
        if(!strcmp(command, "exit")) {
            printf(" by 🐱\n");
            return 0;
        }

        printf("%s\n", command);
    }
}