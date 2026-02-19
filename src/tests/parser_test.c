//
// Created by Ikeda, David on 2/18/26.
//

#include "../parser.h"
#include "../ast.h"
#include <stdio.h>
#include <stdlib.h>

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
    Parser *p = parser_init(lx);

    ASTNode *program = parser_parse(p);

    ast_print(program, 0);

    printf("\n Errors: %d \n", p->error_count);
    ast_free(program);
    lexerFree(lx);
    fclose(f);
    return p->error_count > 0 ? 1 : 0;
}
