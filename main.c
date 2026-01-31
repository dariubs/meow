#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "run.h"

extern int repl(void);

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) < 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return NULL; }
    rewind(f);
    char *buf = malloc((size_t)size + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)size, f);
    buf[n] = '\0';
    fclose(f);
    if (n != (size_t)size) { free(buf); return NULL; }
    return buf;
}

int main(int argc, char **argv) {
    fprintf(stdout, "Meow, World!\n\n");
    if (argc >= 2) {
        char *src = read_file(argv[1]);
        if (!src) {
            fprintf(stderr, "meow: cannot read %s\n", argv[1]);
            return 1;
        }
        int r = run_source(src, 0);
        free(src);
        return r < 0 ? 1 : 0;
    }
    return repl();
}
