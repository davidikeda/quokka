#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validator.h"
#include "ast.h"
#include "parser.h"
#include "lexer.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_file.qk>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *ext = filename + strlen(filename) - 3;
    if (strcmp(ext, ".qk") != 0)
    {
        fprintf(stderr, "Error: Input file must have .qk extension, got %s instead\n", ext);
        return 1;
    }

    FILE *file = fopen(filename, "r");

    Lexer *lexer = lexerInit(file);
    if (!lexer)
    {
        fprintf(stderr, "Error: Could not initialize lexer\n");
        fclose(file);
        return 1;
    }

    Parser *parser = parser_init(lexer);
    if (!parser)
    {
        fprintf(stderr, "Error: Could not initialize parser\n");
        lexerFree(lexer);
        fclose(file);
        return 1;
    }

    ASTNode *ast = parser_parse(parser);
    if (!ast)
    {
        fprintf(stderr, "Error: Could not parse input\n");
        parser_free(parser);
        lexerFree(lexer);
        fclose(file);
        return 1;
    }

    if (parser->error_count > 0)
    {
        fprintf(stderr, "Parsing failed with %d errors\n", parser->error_count);
    }

    printf("Abstract\n");
    ast_print(ast, 0);

    printf("\n Validation \n");
    ValidationResult *result = validator_validate(ast);
    validator_print_errors(result);

    validator_free(result);
    ast_free(ast);
    parser_free(parser);
    lexerFree(lexer);
    fclose(file);

    if (result->error_count > 0 || parser->error_count > 0)
    {
        return 1;
    }

    return 0;
}
