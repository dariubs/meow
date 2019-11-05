#include <stdio.h>

int main(int argc, char **argv) {
    int r;
    printf("Meow, World! 🐈\n\n");
    if(argc < 2) {
        r = repl();
    }
    return r;
}

