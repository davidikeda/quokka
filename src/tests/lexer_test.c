//
// Created by David Ikeda on 2/10/2026.
//

#include "../lexer.h"
#include <stdio.h>
#include <stdlib.h>

static const char *tokenName(TokenType t)
{
    switch (t)
    {
        case TOK_EOF: return "EOF";
        case TOK_IDENTIFIER: return "IDENTIFIER";
        case TOK_ELSE: return "ELSE";
        case TOK_NUMBER: return "NUMBER";
        case TOK_STRING: return "STRING";

        case TOK_NEW: return "NEW";
        case TOK_IF: return "IF";
        case TOK_SEND: return "SEND";
        case TOK_USBIN: return "USBIN";
        case TOK_USBOUT: return "USBOUT";
        case TOK_AS: return "AS";
        case TOK_STATUS: return "STATUS";
        case TOK_CONNECT: return "CONNECT";
        case TOK_THEN: return "THEN";
        case TOK_WRITE: return "WRITE";
        case TOK_LOG: return "LOG";
        case TOK_IMPORT: return "IMPORT";

        case TOK_ASSIGN: return "ASSIGN";
        case TOK_EQUAL: return "EQUAL";
        case TOK_NOT_EQUAL: return "NOT_EQUAL";
        case TOK_LT: return "LT";
        case TOK_GT: return "GT";
        case TOK_LE: return "LE";
        case TOK_GE: return "GE";
        case TOK_PLUS: return "PLUS";
        case TOK_MINUS: return "MINUS";
        case TOK_STAR: return "STAR";
        case TOK_SLASH: return "SLASH";
        case TOK_PERCENT: return "PERCENT";

        case TOK_LPAREN: return "LPAREN";
        case TOK_RPAREN: return "RPAREN";
        case TOK_LBRACE: return "LBRACE";
        case TOK_RBRACE: return "RBRACE";
        case TOK_LBRACKET: return "LBRACKET";
        case TOK_RBRACKET: return "RBRACKET";
        case TOK_COMMA: return "COMMA";
        case TOK_DOT: return "DOT";
        case TOK_COLON: return "COLON";
        case TOK_SEMICOLON: return "SEMICOLON";
        case TOK_AT: return "AT";

        default: return "UNKNOWN";
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file_name.qk>", argv[0]);
        return 1;
    }

    const char *path = argv[1];

    FILE *f = fopen(path, "r");
    if (!f)
    {
        perror(path);
        return 1;
    }

    Lexer *lx = lexerInit(f);

    for (;;)
    {
        Token tok = lexerNextToken(lx);
        printf("[%d:%d] %-12s %s\n",
            tok.line,
            tok.column,
            tokenName(tok.type),
            tok.value ? tok.value : "");

        if (tok.value) {
            free(tok.value);
        }

        if (tok.type == TOK_EOF)
            break;
    }

    lexerFree(lx);
    fclose(f);
    return 0;
}
