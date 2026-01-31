#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum { EXPR_NUMBER, EXPR_IDENT, EXPR_BIN, EXPR_UNARY } ExprKind;
typedef enum { BIN_ADD, BIN_SUB, BIN_MUL, BIN_DIV, BIN_LT, BIN_LE, BIN_GT, BIN_GE, BIN_EQ, BIN_NE } BinOpKind;

typedef struct Expr Expr;
struct Expr {
    ExprKind kind;
    union {
        double num;
        char *ident;
        struct { BinOpKind op; Expr *left; Expr *right; } bin;
        struct { Expr *operand; } unary;
    };
};

typedef enum { STMT_PRINT, STMT_ASSIGN, STMT_IF, STMT_WHILE } StmtKind;
typedef struct Stmt Stmt;
struct Stmt {
    StmtKind kind;
    union {
        struct { Expr *expr; } print;
        struct { char *name; Expr *expr; } assign;
        struct { Expr *cond; Stmt *then_stmt; Stmt *else_stmt; } if_stmt;
        struct { Expr *cond; Stmt *body; } while_stmt;
    };
};

typedef struct {
    Stmt **stmts;
    int n;
    int cap;
} ASTProgram;

void ast_program_init(ASTProgram *prog);
void ast_program_push(ASTProgram *prog, Stmt *s);
void ast_program_free(ASTProgram *prog);
void ast_stmt_free(Stmt *s);
void ast_expr_free(Expr *e);

int parse_program(Lexer *l, ASTProgram *prog, const char **err);

#endif
