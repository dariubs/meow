#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_NUMBER, TOK_IDENT,
    TOK_PRINT, TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_LPAREN, TOK_RPAREN,
    TOK_SEMI, TOK_EQ,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_LT, TOK_GT, TOK_LE, TOK_GE, TOK_EQEQ, TOK_NE,
    TOK_NEWLINE, TOK_EOF
} TokenKind;

typedef struct {
    TokenKind kind;
    double num_value;
    const char *ident_start;
    int ident_len;
} Token;

typedef struct {
    const char *src;
    const char *p;
} Lexer;

void lex_init(Lexer *l, const char *src);
Token lex_next(Lexer *l);

#endif
