#include <stdio.h>

#include "repl.c"

int main(int argc, char **argv) {
    int r;
    fprintf(stdout, "Meow, World! 🐈\n\n");
    if(argc < 2) {
        r = repl();
    }
    return r;
}

