#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "cmd.c"

static volatile int keepalive = 1;

void sig_handler(int d);
int check_command(char* command, char* str);

int repl() {

    signal(SIGINT, sig_handler);

    struct Command cmd;
    char *command;
    while(keepalive) {        
        cmd.str = readline("(meow)> ");
        add_history(cmd.str);
        
        if(!check_command(cmd.str, "exit")) {
            fprintf(stdout, "Meow 😿\n");
            return 0;
        } else if (!check_command(cmd.str, "hi")) {
            fprintf(stdout, "Meow 🐱\n");
            continue;
        } else {
            fprintf(stdout, "%s command not found😼\n", cmd.str);
        }
    }
}

void sig_handler(int d) {
    keepalive = 0;
    fprintf(stdout, " by 😿\n");
    exit(0);
}

int check_command(char* command, char* str) {
    return strcmp(command, str);
}
