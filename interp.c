#include "interp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYM_MAX 64
typedef struct {
    char *name;
    double value;
} Sym;
static Sym syms[SYM_MAX];
static int nsyms;

static int has_var(const char *name) {
    for (int i = 0; i < nsyms; i++)
        if (strcmp(syms[i].name, name) == 0)
            return 1;
    return 0;
}

static double get_var(const char *name) {
    for (int i = 0; i < nsyms; i++)
        if (strcmp(syms[i].name, name) == 0)
            return syms[i].value;
    return 0;
}

static int set_var(const char *name, double value, char *err_buf, int err_size) {
    for (int i = 0; i < nsyms; i++) {
        if (strcmp(syms[i].name, name) == 0) {
            syms[i].value = value;
            return 0;
        }
    }
    if (nsyms >= SYM_MAX) {
        snprintf(err_buf, (size_t)err_size, "too many variables");
        return -1;
    }
    syms[nsyms].name = strdup(name);
    if (!syms[nsyms].name) {
        snprintf(err_buf, (size_t)err_size, "out of memory");
        return -1;
    }
    syms[nsyms].value = value;
    nsyms++;
    return 0;
}

static void syms_clear(void) {
    for (int i = 0; i < nsyms; i++) free(syms[i].name);
    nsyms = 0;
}

static int eval_expr(Expr *e, double *out, char *err_buf, int err_size);

static int eval_expr(Expr *e, double *out, char *err_buf, int err_size) {
    if (!e) return -1;
    switch (e->kind) {
        case EXPR_NUMBER:
            *out = e->num;
            return 0;
        case EXPR_IDENT:
            if (!has_var(e->ident)) {
                snprintf(err_buf, (size_t)err_size, "undefined variable %s", e->ident);
                return -1;
            }
            *out = get_var(e->ident);
            return 0;
        case EXPR_UNARY: {
            double v;
            if (eval_expr(e->unary.operand, &v, err_buf, err_size) < 0) return -1;
            *out = -v;
            return 0;
        }
        case EXPR_BIN: {
            double left, right;
            if (eval_expr(e->bin.left, &left, err_buf, err_size) < 0) return -1;
            if (eval_expr(e->bin.right, &right, err_buf, err_size) < 0) return -1;
            switch (e->bin.op) {
                case BIN_ADD: *out = left + right; return 0;
                case BIN_SUB: *out = left - right; return 0;
                case BIN_MUL: *out = left * right; return 0;
                case BIN_DIV:
                    if (right == 0) {
                        snprintf(err_buf, (size_t)err_size, "division by zero");
                        return -1;
                    }
                    *out = left / right;
                    return 0;
                case BIN_LT:  *out = left < right ? 1 : 0; return 0;
                case BIN_LE:  *out = left <= right ? 1 : 0; return 0;
                case BIN_GT:  *out = left > right ? 1 : 0; return 0;
                case BIN_GE:  *out = left >= right ? 1 : 0; return 0;
                case BIN_EQ:  *out = left == right ? 1 : 0; return 0;
                case BIN_NE:  *out = left != right ? 1 : 0; return 0;
            }
            break;
        }
    }
    return -1;
}

static int run_stmt(Stmt *s, char *err_buf, int err_size);

static int run_stmt(Stmt *s, char *err_buf, int err_size) {
    if (!s) return 0;
    switch (s->kind) {
        case STMT_PRINT: {
            double v;
            if (eval_expr(s->print.expr, &v, err_buf, err_size) < 0) return -1;
            printf("%g\n", v);
            return 0;
        }
        case STMT_ASSIGN: {
            double v;
            if (eval_expr(s->assign.expr, &v, err_buf, err_size) < 0) return -1;
            if (set_var(s->assign.name, v, err_buf, err_size) < 0) return -1;
            return 0;
        }
        case STMT_IF: {
            double cond;
            if (eval_expr(s->if_stmt.cond, &cond, err_buf, err_size) < 0) return -1;
            if (cond != 0) return run_stmt(s->if_stmt.then_stmt, err_buf, err_size);
            if (s->if_stmt.else_stmt) return run_stmt(s->if_stmt.else_stmt, err_buf, err_size);
            return 0;
        }
        case STMT_WHILE: {
            for (;;) {
                double cond;
                if (eval_expr(s->while_stmt.cond, &cond, err_buf, err_size) < 0) return -1;
                if (cond == 0) break;
                if (run_stmt(s->while_stmt.body, err_buf, err_size) < 0) return -1;
            }
            return 0;
        }
    }
    return -1;
}

int interp_run(ASTProgram *prog, char *err_buf, int err_size) {
    syms_clear();
    for (int i = 0; i < prog->n; i++) {
        if (run_stmt(prog->stmts[i], err_buf, err_size) < 0)
            return -1;
    }
    return 0;
}
