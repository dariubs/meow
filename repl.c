#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "run.h"

static void trim(char *s) {
    char *p = s + strlen(s);
    while (p > s && (p[-1] == ' ' || p[-1] == '\t' || p[-1] == '\n')) *--p = '\0';
}

#ifdef MEOW_NO_READLINE

#define LINE_MAX 4096

int repl(void) {
    static char line[LINE_MAX];
    while (fputs("(meow)> ", stdout) != EOF && fflush(stdout) == 0 && fgets(line, LINE_MAX, stdin)) {
        trim(line);
        if (strcmp(line, "exit") == 0)
            return 0;
        if (*line) run_source(line, 1);
    }
    return 0;
}

#else

#include <readline/readline.h>
#include <readline/history.h>

static volatile int keepalive = 1;

static void sig_handler(int d) {
    (void)d;
    keepalive = 0;
    fprintf(stdout, " by\n");
    exit(0);
}

int repl(void) {
#ifndef _WIN32
    signal(SIGINT, sig_handler);
#endif
    while (keepalive) {
        char *line = readline("(meow)> ");
        if (!line) break;
        add_history(line);
        trim(line);
        if (strcmp(line, "exit") == 0) {
            free(line);
            return 0;
        }
        if (*line) run_source(line, 1);
        free(line);
    }
    return 0;
}

#endif
