#include "parser.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MAX 256
static char parse_err[ERR_MAX];

static void set_err(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(parse_err, ERR_MAX, fmt, ap);
    va_end(ap);
}

static Token cur;
static Lexer *lex;

static void advance(void) { cur = lex_next(lex); }
static void skip_newlines(void) { while (cur.kind == TOK_NEWLINE) advance(); }

static char *copy_ident(void) {
    char *s = malloc((size_t)cur.ident_len + 1);
    if (!s) return NULL;
    memcpy(s, cur.ident_start, (size_t)cur.ident_len);
    s[cur.ident_len] = '\0';
    return s;
}

static int expect(TokenKind k) {
    if (cur.kind != k) {
        set_err("unexpected token");
        return -1;
    }
    advance();
    return 0;
}

static Expr *parse_expr(void);
static Stmt *parse_stmt(void);

static Expr *parse_factor(void) {
    if (cur.kind == TOK_NUMBER) {
        Expr *e = malloc(sizeof(Expr));
        if (!e) return NULL;
        e->kind = EXPR_NUMBER;
        e->num = cur.num_value;
        advance();
        return e;
    }
    if (cur.kind == TOK_IDENT) {
        Expr *e = malloc(sizeof(Expr));
        if (!e) return NULL;
        e->kind = EXPR_IDENT;
        e->ident = copy_ident();
        advance();
        return e;
    }
    if (cur.kind == TOK_LPAREN) {
        advance();
        Expr *e = parse_expr();
        if (!e) return NULL;
        if (expect(TOK_RPAREN) < 0) { ast_expr_free(e); return NULL; }
        return e;
    }
    if (cur.kind == TOK_MINUS) {
        advance();
        Expr *operand = parse_factor();
        if (!operand) return NULL;
        Expr *e = malloc(sizeof(Expr));
        if (!e) { ast_expr_free(operand); return NULL; }
        e->kind = EXPR_UNARY;
        e->unary.operand = operand;
        return e;
    }
    set_err("expected expression");
    return NULL;
}

static BinOpKind token_to_binop(TokenKind k) {
    switch (k) {
        case TOK_PLUS: return BIN_ADD;
        case TOK_MINUS: return BIN_SUB;
        case TOK_STAR: return BIN_MUL;
        case TOK_SLASH: return BIN_DIV;
        case TOK_LT: return BIN_LT;
        case TOK_LE: return BIN_LE;
        case TOK_GT: return BIN_GT;
        case TOK_GE: return BIN_GE;
        case TOK_EQEQ: return BIN_EQ;
        case TOK_NE: return BIN_NE;
        default: return BIN_ADD;
    }
}

static int is_term_op(TokenKind k) {
    return k == TOK_STAR || k == TOK_SLASH;
}
static int is_add_op(TokenKind k) {
    return k == TOK_PLUS || k == TOK_MINUS;
}
static int is_cmp_op(TokenKind k) {
    return k == TOK_LT || k == TOK_LE || k == TOK_GT || k == TOK_GE || k == TOK_EQEQ || k == TOK_NE;
}

static Expr *parse_term(void) {
    Expr *left = parse_factor();
    if (!left) return NULL;
    while (is_term_op(cur.kind)) {
        BinOpKind op = token_to_binop(cur.kind);
        advance();
        Expr *right = parse_factor();
        if (!right) { ast_expr_free(left); return NULL; }
        Expr *e = malloc(sizeof(Expr));
        if (!e) { ast_expr_free(left); ast_expr_free(right); return NULL; }
        e->kind = EXPR_BIN;
        e->bin.op = op;
        e->bin.left = left;
        e->bin.right = right;
        left = e;
    }
    return left;
}

static Expr *parse_additive(void) {
    Expr *left = parse_term();
    if (!left) return NULL;
    while (is_add_op(cur.kind)) {
        BinOpKind op = token_to_binop(cur.kind);
        advance();
        Expr *right = parse_term();
        if (!right) { ast_expr_free(left); return NULL; }
        Expr *e = malloc(sizeof(Expr));
        if (!e) { ast_expr_free(left); ast_expr_free(right); return NULL; }
        e->kind = EXPR_BIN;
        e->bin.op = op;
        e->bin.left = left;
        e->bin.right = right;
        left = e;
    }
    return left;
}

static Expr *parse_comparison(void) {
    Expr *left = parse_additive();
    if (!left) return NULL;
    if (is_cmp_op(cur.kind)) {
        BinOpKind op = token_to_binop(cur.kind);
        advance();
        Expr *right = parse_additive();
        if (!right) { ast_expr_free(left); return NULL; }
        Expr *e = malloc(sizeof(Expr));
        if (!e) { ast_expr_free(left); ast_expr_free(right); return NULL; }
        e->kind = EXPR_BIN;
        e->bin.op = op;
        e->bin.left = left;
        e->bin.right = right;
        return e;
    }
    return left;
}

static Expr *parse_expr(void) {
    return parse_comparison();
}

static Stmt *parse_stmt(void) {
    if (cur.kind == TOK_PRINT) {
        advance();
        Expr *e = parse_expr();
        if (!e) return NULL;
        if (expect(TOK_SEMI) < 0) { ast_expr_free(e); return NULL; }
        Stmt *s = malloc(sizeof(Stmt));
        if (!s) { ast_expr_free(e); return NULL; }
        s->kind = STMT_PRINT;
        s->print.expr = e;
        return s;
    }
    if (cur.kind == TOK_WHILE) {
        advance();
        if (expect(TOK_LPAREN) < 0) return NULL;
        Expr *cond = parse_expr();
        if (!cond) return NULL;
        if (expect(TOK_RPAREN) < 0) { ast_expr_free(cond); return NULL; }
        skip_newlines();
        Stmt *body = parse_stmt();
        if (!body) { ast_expr_free(cond); return NULL; }
        Stmt *s = malloc(sizeof(Stmt));
        if (!s) { ast_expr_free(cond); ast_stmt_free(body); return NULL; }
        s->kind = STMT_WHILE;
        s->while_stmt.cond = cond;
        s->while_stmt.body = body;
        return s;
    }
    if (cur.kind == TOK_IF) {
        advance();
        if (expect(TOK_LPAREN) < 0) return NULL;
        Expr *cond = parse_expr();
        if (!cond) return NULL;
        if (expect(TOK_RPAREN) < 0) { ast_expr_free(cond); return NULL; }
        skip_newlines();
        Stmt *then_s = parse_stmt();
        if (!then_s) { ast_expr_free(cond); return NULL; }
        Stmt *else_s = NULL;
        skip_newlines();
        if (cur.kind == TOK_ELSE) {
            advance();
            skip_newlines();
            else_s = parse_stmt();
            if (!else_s) { ast_expr_free(cond); ast_stmt_free(then_s); return NULL; }
        }
        Stmt *s = malloc(sizeof(Stmt));
        if (!s) { ast_expr_free(cond); ast_stmt_free(then_s); ast_stmt_free(else_s); return NULL; }
        s->kind = STMT_IF;
        s->if_stmt.cond = cond;
        s->if_stmt.then_stmt = then_s;
        s->if_stmt.else_stmt = else_s;
        return s;
    }
    if (cur.kind == TOK_IDENT) {
        char *name = copy_ident();
        if (!name) return NULL;
        advance();
        if (cur.kind != TOK_EQ) {
            set_err("expected =");
            free(name);
            return NULL;
        }
        advance();
        Expr *e = parse_expr();
        if (!e) { free(name); return NULL; }
        if (expect(TOK_SEMI) < 0) { free(name); ast_expr_free(e); return NULL; }
        Stmt *s = malloc(sizeof(Stmt));
        if (!s) { free(name); ast_expr_free(e); return NULL; }
        s->kind = STMT_ASSIGN;
        s->assign.name = name;
        s->assign.expr = e;
        return s;
    }
    set_err("expected statement");
    return NULL;
}

void ast_program_init(ASTProgram *prog) {
    prog->stmts = NULL;
    prog->n = 0;
    prog->cap = 0;
}

void ast_program_push(ASTProgram *prog, Stmt *s) {
    if (prog->n >= prog->cap) {
        int newcap = prog->cap ? prog->cap * 2 : 8;
        Stmt **p = realloc(prog->stmts, (size_t)newcap * sizeof(Stmt *));
        if (!p) return;
        prog->stmts = p;
        prog->cap = newcap;
    }
    prog->stmts[prog->n++] = s;
}

void ast_expr_free(Expr *e) {
    if (!e) return;
    if (e->kind == EXPR_IDENT) free(e->ident);
    if (e->kind == EXPR_BIN) { ast_expr_free(e->bin.left); ast_expr_free(e->bin.right); }
    if (e->kind == EXPR_UNARY) ast_expr_free(e->unary.operand);
    free(e);
}

void ast_stmt_free(Stmt *s) {
    if (!s) return;
    switch (s->kind) {
        case STMT_PRINT: ast_expr_free(s->print.expr); break;
        case STMT_ASSIGN: free(s->assign.name); ast_expr_free(s->assign.expr); break;
        case STMT_IF:
            ast_expr_free(s->if_stmt.cond);
            ast_stmt_free(s->if_stmt.then_stmt);
            ast_stmt_free(s->if_stmt.else_stmt);
            break;
        case STMT_WHILE:
            ast_expr_free(s->while_stmt.cond);
            ast_stmt_free(s->while_stmt.body);
            break;
    }
    free(s);
}

void ast_program_free(ASTProgram *prog) {
    for (int i = 0; i < prog->n; i++) ast_stmt_free(prog->stmts[i]);
    free(prog->stmts);
    prog->stmts = NULL;
    prog->n = prog->cap = 0;
}

int parse_program(Lexer *l, ASTProgram *prog, const char **err) {
    lex = l;
    advance();
    skip_newlines();
    while (cur.kind != TOK_EOF) {
        Stmt *s = parse_stmt();
        if (!s) {
            *err = parse_err;
            return -1;
        }
        ast_program_push(prog, s);
        skip_newlines();
    }
    *err = NULL;
    return 0;
}
