#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int iskeyword(const char *start, int len, const char *kw) {
    size_t kwlen = strlen(kw);
    return (int)kwlen == len && memcmp(start, kw, len) == 0;
}

void lex_init(Lexer *l, const char *src) {
    l->src = src;
    l->p = src;
}

static Token token(TokenKind kind) {
    Token t = { .kind = kind, .num_value = 0, .ident_start = NULL, .ident_len = 0 };
    return t;
}

Token lex_next(Lexer *l) {
    const char *p = l->p;
    while (*p == ' ' || *p == '\t')
        p++;
    if (*p == '#') {
        while (*p && *p != '\n') p++;
        l->p = p;
        return lex_next(l);
    }
    if (*p == '\n') {
        l->p = p + 1;
        return token(TOK_NEWLINE);
    }
    if (!*p) {
        l->p = p;
        return token(TOK_EOF);
    }
    if (isdigit((unsigned char)*p) || (*p == '.' && isdigit((unsigned char)p[1]))) {
        char *end;
        double v = strtod(p, &end);
        l->p = end;
        Token t = token(TOK_NUMBER);
        t.num_value = v;
        return t;
    }
    if (isalpha((unsigned char)*p) || *p == '_') {
        const char *start = p;
        while (isalnum((unsigned char)*p) || *p == '_') p++;
        l->p = p;
        Token t;
        t.ident_start = start;
        t.ident_len = (int)(p - start);
        if (iskeyword(start, t.ident_len, "print")) { t.kind = TOK_PRINT; t.ident_start = NULL; t.ident_len = 0; return t; }
        if (iskeyword(start, t.ident_len, "if"))   { t.kind = TOK_IF;   t.ident_start = NULL; t.ident_len = 0; return t; }
        if (iskeyword(start, t.ident_len, "else")) { t.kind = TOK_ELSE; t.ident_start = NULL; t.ident_len = 0; return t; }
        if (iskeyword(start, t.ident_len, "while")){ t.kind = TOK_WHILE;t.ident_start = NULL; t.ident_len = 0; return t; }
        t.kind = TOK_IDENT;
        return t;
    }
    if (p[0] == '(') { l->p = p + 1; return token(TOK_LPAREN); }
    if (p[0] == ')') { l->p = p + 1; return token(TOK_RPAREN); }
    if (p[0] == ';') { l->p = p + 1; return token(TOK_SEMI); }
    if (p[0] == '+' && p[1] != '=') { l->p = p + 1; return token(TOK_PLUS); }
    if (p[0] == '-' && p[1] != '=') { l->p = p + 1; return token(TOK_MINUS); }
    if (p[0] == '*') { l->p = p + 1; return token(TOK_STAR); }
    if (p[0] == '/') { l->p = p + 1; return token(TOK_SLASH); }
    if (p[0] == '=' && p[1] == '=') { l->p = p + 2; return token(TOK_EQEQ); }
    if (p[0] == '=') { l->p = p + 1; return token(TOK_EQ); }
    if (p[0] == '<' && p[1] == '=') { l->p = p + 2; return token(TOK_LE); }
    if (p[0] == '>' && p[1] == '=') { l->p = p + 2; return token(TOK_GE); }
    if (p[0] == '!' && p[1] == '=') { l->p = p + 2; return token(TOK_NE); }
    if (p[0] == '<') { l->p = p + 1; return token(TOK_LT); }
    if (p[0] == '>') { l->p = p + 1; return token(TOK_GT); }
    l->p = p + 1;
    return token(TOK_EOF);
}
