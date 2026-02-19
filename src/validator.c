//
// Created by Ikeda, David on 2/19/26.
//

#include "validator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    ValidationResult *result;
} Validator;

static void validator_error(Validator *v, int line, int col, const char *msg)
{
    if (v->result->error_count >= 1024) return;
    snprintf(v->result->errors[v->result->error_count], 256,
        "[%d:%d] Validation error: %s", line, col, msg);
    v->result->error_count++;
}

static void validator_validate_node(Validator *v, ASTNode *node);

static void validator_validate_import(Validator *v, ASTNode *node)
{
    // make sure import path not empy
    if (!node->string_value || strlen(node->string_value) == 0)
    {
        validator_error(v, node->line, node->column, "Import path cannot be empty");
        return;
    }

    // import path must start with .j (joey)
    const char *ext = node->string_value + strlen(node->string_value) - 2;
    if (strcmp(ext, ".j") != 0)
    {
        validator_error(v, node->line, node->column,
            "Import path must reference a .j header/packet definition file");
    }
}

static void validator_validate_declaration(Validator *v, ASTNode *node)
{
    // device name not empty
    if (!node->string_value || strlen(node->string_value) == 0)
    {
        validator_error(v, node->line, node->column, "Device name cannot be empty");
    }

    // dec has alias and type
    if (node->num_children < 2)
    {
        validator_error(v, node->line, node->column,
            "Declaration must have device type and alias");
    }
}

static void validator_validate_call(Validator *v, ASTNode *node)
{
    if (!node->left)
    {
        validator_error(v, node->line, node->column, "Call missing function name");
    }

    if (!node->right)
    {
        validator_error(v, node->line, node->column, "Call missing arguments");
    }
}

static void validator_validate_binary_op(Validator *v, ASTNode *node)
{
    if (!node->left)
    {
        validator_error(v, node->line, node->column, "Binary operation missing left operand");
    }

    if (!node->right)
    {
        validator_error(v, node->line, node->column, "Binary operation missing right operand");
    }
}

static void validator_validate_node(Validator *v, ASTNode *node)
{
    if (!node) return;

    switch (node->type)
    {
        case AST_IMPORT:
            validator_validate_import(v, node);
            break;
        case AST_DECLARATION:
            validator_validate_declaration(v, node);
            break;
        case AST_CALL:
            validator_validate_call(v, node);
            break;
        case AST_BINARY_OP:
            validator_validate_binary_op(v, node);
            break;
        default:
            break;
    }

    for (int i = 0; i < node->num_children; i++)
    {
        validator_validate_node(v, node->children[i]);
    }

    if (node->type == AST_BINARY_OP || node->type == AST_CALL || node->type == AST_MEMBER_ACCESS)
    {
        validator_validate_node(v, node->left);
        validator_validate_node(v, node->right);
    }
    else
    {
        if (node->left) validator_validate_node(v, node->left);
        if (node->right) validator_validate_node(v, node->right);
    }
}

ValidationResult* validator_validate(ASTNode *ast)
{
    ValidationResult *result = malloc(sizeof(ValidationResult));
    result->error_count = 0;
    result->warning_count = 0;

    Validator v = { result };
    validator_validate_node(&v, ast);

    return result;
}

void validator_print_errors(ValidationResult *result)
{
    if (result->error_count == 0)
    {
        printf("✓ Validation passed\n");
        return;
    }

    printf("\n=== Validation Errors ===\n");
    for (int i = 0; i < result->error_count; i++)
    {
        printf("%s\n", result->errors[i]);
    }
    printf("Total errors: %d\n", result->error_count);
}

void validator_free(ValidationResult *result)
{
    free(result);
}
