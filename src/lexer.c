//
// Created by David Ikeda on 2/10/2026.
//

#include "lexer.h"
#include "compat.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void lexerAdvance(Lexer *lx)
{
    if (lx->current == '\n')
    {
        lx->line++;
        lx->column = 0;
    } else if (lx->current != EOF)
    {
        lx->column++;
    }

    lx->current = lx->next;
    lx->next = fgetc(lx->file);
}

static void lexerSkipWhitespace(Lexer *lx)
{
    for (;;)
    {
        while (isspace(lx->current))
        {
            lexerAdvance(lx);
        }

        if (lx->current == '/' && lx->next == '/')
        {
            while (lx->current != '\n' && lx->current != EOF)
            {
                lexerAdvance(lx);
            }
        } else
        {
            break;
        }
    }
}

typedef struct
{
    const char *name;
    TokenType type;
} Keyword;

static const Keyword keywords[] = {
    { "new", TOK_NEW },
    { "if", TOK_IF },
    { "then", TOK_THEN },
    { "else", TOK_ELSE },
    {"try", TOK_TRY},
    {"except", TOK_EXCEPT},
    {"finally", TOK_FINALLY},
    { "and", TOK_AND },
    { "or", TOK_OR },
    { "not", TOK_NOT },
    { "send", TOK_SEND },
    { "receive", TOK_RECEIVE },
    { "usbin", TOK_USBIN },
    { "usbout", TOK_USBOUT },
    {"as", TOK_AS},
    {"funct", TOK_FUNCT},
    {"import", TOK_IMPORT},
    { NULL, TOK_UNKNOWN }
};

static Token makeToken(Lexer *lx, TokenType type, const char *value, int col)
{
    Token tok;
    tok.type = type;
    tok.value = value ? strdup(value) : NULL;
    tok.line = lx->line;
    tok.column = col;
    return tok;
}

static Token lexerIdentifier(Lexer *lx)
{
    char buf[256];
    int len = 0;
    int col = lx->column;

    while (isalnum(lx->current) || lx->current == '_')
    {
        buf[len++] = lx->current;
        lexerAdvance(lx);
    }
    buf[len] = '\0';

    for (int i = 0; keywords[i].name; i++)
    {
        if (strcasecmp(buf, keywords[i].name) == 0)
        {
            return makeToken(lx, keywords[i].type, NULL, col);
        }
    }

    return makeToken(lx, TOK_IDENTIFIER, buf, col);
}

static Token lexerNumeric(Lexer *lx)
{
    char buf[256];
    int len = 0;
    int col = lx->column;

    while (isdigit(lx->current) || lx->current == '.')
    {
        buf[len++] = lx->current;
        lexerAdvance(lx);
    }
    buf[len] = '\0';

    return makeToken(lx, TOK_NUMBER, buf, col);
}

static Token lexerString(Lexer *lx)
{
    char buf[1024];
    /* Strings are greedy bastards */
    int len = 0;
    int col = lx->column;

    lexerAdvance(lx); /* skip */

    while (lx->current != '"' && lx->current != EOF)
    {
        if (lx->current == '\\')
        {
            lexerAdvance(lx);
        }
        buf[len++] = lx->current;
        lexerAdvance(lx);
    }

    lexerAdvance(lx);
    buf[len] = '\0';

    return makeToken(lx, TOK_STRING, buf, col);
}

Lexer *lexerInit(FILE *file)
{
    Lexer *lx = malloc(sizeof(Lexer));
    lx->file = file;
    lx->line = 1;
    lx->column = 0;
    lx->current = fgetc(file);
    lx->next = fgetc(file);
    return lx;
}

Token lexerNextToken(Lexer *lx)
{
    lexerSkipWhitespace(lx);

    if (lx->current == EOF)
    {
        return makeToken(lx, TOK_EOF, NULL, lx->column);
    }

    if (isalpha(lx->current) || lx->current == '_')
        return lexerIdentifier(lx);
    if (isdigit(lx->current))
        return lexerNumeric(lx);
    if (lx->current == '"')
        return lexerString(lx);

    int col = lx->column;

    if (lx->current == '=' && lx->next == '=') {
        lexerAdvance(lx);
        lexerAdvance(lx);
        return makeToken(lx, TOK_EQUAL, NULL, col);
    }
    if (lx->current == '!' && lx->next == '=') {
        lexerAdvance(lx);
        lexerAdvance(lx);
        return makeToken(lx, TOK_NOT_EQUAL, NULL, col);
    }
    if (lx->current == '<' && lx->next == '=') {
        lexerAdvance(lx);
        lexerAdvance(lx);
        return makeToken(lx, TOK_LE, NULL, col);
    }
    if (lx->current == '>' && lx->next == '=') {
        lexerAdvance(lx);
        lexerAdvance(lx);
        return makeToken(lx, TOK_GE, NULL, col);
    }

    switch (lx->current)
    {
        case '@': lexerAdvance(lx); return makeToken(lx, TOK_AT, NULL, col);
        case '=': lexerAdvance(lx); return makeToken(lx, TOK_ASSIGN, NULL, col);
        case '<': lexerAdvance(lx); return makeToken(lx, TOK_LT, NULL, col);
        case '>': lexerAdvance(lx); return makeToken(lx, TOK_GT, NULL, col);
        case '+': lexerAdvance(lx); return makeToken(lx, TOK_PLUS, NULL, col);
        case '-': lexerAdvance(lx); return makeToken(lx, TOK_MINUS, NULL, col);
        case '*': lexerAdvance(lx); return makeToken(lx, TOK_STAR, NULL, col);
        case '/': lexerAdvance(lx); return makeToken(lx, TOK_SLASH, NULL, col);
        case '%': lexerAdvance(lx); return makeToken(lx, TOK_PERCENT, NULL, col);
        case '(': lexerAdvance(lx); return makeToken(lx, TOK_LPAREN, NULL, col);
        case ')': lexerAdvance(lx); return makeToken(lx, TOK_RPAREN, NULL, col);
        case '{': lexerAdvance(lx); return makeToken(lx, TOK_LBRACE, NULL, col);
        case '}': lexerAdvance(lx); return makeToken(lx, TOK_RBRACE, NULL, col);
        case '[': lexerAdvance(lx); return makeToken(lx, TOK_LBRACKET, NULL, col);
        case ']': lexerAdvance(lx); return makeToken(lx, TOK_RBRACKET, NULL, col);
        case ',': lexerAdvance(lx); return makeToken(lx, TOK_COMMA, NULL, col);
        case '.': lexerAdvance(lx); return makeToken(lx, TOK_DOT, NULL, col);
        case ':': lexerAdvance(lx); return makeToken(lx, TOK_COLON, NULL, col);
        case ';': lexerAdvance(lx); return makeToken(lx, TOK_SEMICOLON, NULL, col);
    }

    char unknown[2] = { (char)lx->current, '\0'};
    lexerAdvance(lx);
    return makeToken(lx, TOK_UNKNOWN, unknown, col);
}

void lexerFree(Lexer *lx)
{
    free(lx);
}
