//
// Created by Ikeda, David on 2/18/26.
//

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct
{
    Lexer *lexer;
    Token current;
    Token peek;
    int error_count;
} Parser;

Parser* parser_init(Lexer *lexer);
ASTNode* parser_parse(Parser *p);
void parser_free(Parser *p);

#endif //PARSER_H
