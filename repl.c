#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

static volatile int keepalive = 1;

void sig_handler(int d);

int repl() {

    signal(SIGINT, sig_handler);

    char *command;
    while(keepalive) {        
        command = readline("(meow)> ");
        add_history(command);
        
        if(!strcmp(command, "exit")) {
            fprintf(stdout, " by 🐱\n");
            return 0;
        }

        fprintf(stdout, "%s\n", command);
    }
}

void sig_handler(int d) {
    keepalive = 0;
    fprintf(stdout, " by 🐱\n");
    exit(0);
}
