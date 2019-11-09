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
            fprintf(stdout, " by 🐱\n");
            return 0;
        }

        fprintf(stdout, "%s\n", command);
    }
}