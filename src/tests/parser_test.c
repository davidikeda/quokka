//
// Created by Ikeda, David on 2/18/26.
//

#include "../parser.h"
#include "../ast.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *f = fopen("/Users/1020986/Documents/Programming/quokka/src/tests/test.qk", "r");
    if (!f)
    {
        perror("/Users/1020986/Documents/Programming/quokka/src/tests/test.qk");
        return 1;
    }

    Lexer *lx = lexerInit(f);
    Parser *p = parser_init(lx);

    ASTNode *program = parser_parse(p);

    ast_print(program, 0);

    printf("\n Errors: %d \n", p->error_count);
    ast_free(program);
    lexerFree(lx);
    fclose(f);
    return p->error_count > 0 ? 1 : 0;
}
