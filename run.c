#include "run.h"
#include "lexer.h"
#include "parser.h"
#include "interp.h"
#include <stdio.h>
#include <stdlib.h>

int run_source(const char *src, int is_repl) {
    Lexer l;
    lex_init(&l, src);
    ASTProgram prog;
    ast_program_init(&prog);
    const char *parse_err = NULL;
    if (parse_program(&l, &prog, &parse_err) < 0) {
        fprintf(stderr, "meow: %s\n", parse_err);
        ast_program_free(&prog);
        return is_repl ? 0 : -1;
    }
    char interp_err[INTERP_ERR_MAX];
    if (interp_run(&prog, interp_err, INTERP_ERR_MAX) < 0) {
        fprintf(stderr, "meow: %s\n", interp_err);
        ast_program_free(&prog);
        return is_repl ? 0 : -1;
    }
    ast_program_free(&prog);
    return 0;
}
