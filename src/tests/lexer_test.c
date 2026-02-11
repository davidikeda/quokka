//
// Created by David Ikeda on 2/10/2026.
//

#include "../lexer.h"
#include <stdio.h>

static const char *tokenName(TokenType t)
{
    switch (t)
    {
        case TOK_EOF: return "EOF";
        case TOK_IDENTIFIER: return "IDENTIFIER";
        case TOK_NUMBER: return "NUMBER";
        case TOK_STRING: return "STRING";
        case TOK_NEW: return "NEW";
        case TOK_IF: return "IF";
        case TOK_SEND: return "SEND";
        case TOK_USBIN: return "USBIN";
        default: return "OTHER";
    }
}

int main(void)
{
    FILE *f = fopen("test.qk", "r");
    if (!f)
    {
        perror("test.qk");
        return 1;
    }

    Lexer *lx = lexerInit(f);

    for (;;)
    {
        Token tok = lexerNextToken(lx);
        printf("[%d:%d] %-12s %\n",
            tok.line,
            tok.column,
            tokenName(tok.type),
            tok.value ? tok.value : "");

        if (tok.type == TOK_EOF)
            break;
    }

    lexerFree(lx);
    fclose(f);
    return 0;
}