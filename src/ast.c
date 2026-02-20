//
// Created by Ikeda, David on 2/17/26.
//

#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ASTNode* ast_create(ASTNodeType type, int line, int column)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->line = line;
    node->column = column;
    node->string_value = NULL;
    node->number_value = 0;
    node->op = NULL;
    node->children = NULL;
    node->num_children = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode* ast_create_identifier(const char *name, int line, int column)
{
    ASTNode *node = ast_create(AST_IDENTIFIER, line, column);
    node->string_value = strdup(name);
    return node;
}

ASTNode* ast_create_number(double value, int line, int column)
{
    ASTNode *node = ast_create(AST_NUMBER, line, column);
    node->number_value = value;
    return node;
}

ASTNode* ast_create_string(const char *value, int line, int column)
{
    ASTNode *node = ast_create(AST_STRING, line, column);
    node->string_value = strdup(value);
    return node;
}

ASTNode* ast_create_binary(ASTNode *left, const char *op, ASTNode *right, int line, int column)
{
    ASTNode *node = ast_create(AST_BINARY_OP, line, column);
    node->left = left;
    node->right = right;
    node->op = op ? strdup(op) : NULL;
    return node;
}

ASTNode* ast_create_call(ASTNode *callee, ASTNode  *args, int line, int column)
{
    ASTNode *node = ast_create(AST_CALL, line, column);
    node->left = callee;
    node->right = args;
    return node;
}

ASTNode* ast_create_member(ASTNode *object, ASTNode *member, int line, int column)
{
    ASTNode *node = ast_create(AST_MEMBER_ACCESS, line, column);
    node->left = object;
    node->right = member;
    return node;
}
void ast_add_child(ASTNode *parent, ASTNode *child)
{
    if (!child) return;

    parent->children = realloc(parent->children, sizeof(ASTNode*) * (parent->num_children + 1));
    parent->children[parent->num_children++] = child;
}

void ast_free(ASTNode *node)
{
    if (!node) return;

    if (node->string_value) free(node->string_value);
    if (node->op) free(node->op);

    for (int i = 0; i < node->num_children; i++)
    {
        ast_free(node->children[i]);
    }
    if (node->children) free(node->children);

    ast_free(node->left);
    ast_free(node->right);
    free(node);
}

static const char* node_type_name(ASTNodeType type)
{
    switch (type)
    {
        case AST_PROGRAM: return "PROGRAM";
        case AST_IMPORT: return "IMPORT";
        case AST_DECLARATION: return "DECLARATION";
        case AST_ASSIGNMENT: return "ASSIGNMENT";
        case AST_IF_STMT: return "IF";
        case AST_BLOCK: return "BLOCK";
        case AST_EXPR: return "EXPR_STMT";
        case AST_FUNCTION_DEF: return "FUNCTION_DEF";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_NUMBER: return "NUMBER";
        case AST_STRING: return "STRING";
        case AST_BINARY_OP: return "BINARY_OP";
        case AST_UNARY_OP: return "UNARY_OP";
        case AST_CALL: return "CALL";
        case AST_MEMBER_ACCESS: return "MEMBER_ACCESS";
        case AST_ARGUMENTS: return "ARGUMENTS";
        default: return "UNKNOWN";
    }
}

void ast_print(ASTNode *node, int depth)
{
    if (!node) return;

    for (int i = 0; i < depth; i++) printf(" ");
    printf("[%d:%d] %s", node->line, node->column, node_type_name(node->type));

    if (node->string_value)
        printf(" \"%s\"", node->string_value);
    if (node->op)
        printf(" op=%s", node->op);
    if (node->type == AST_NUMBER)
        printf(" %.2f", node->number_value);

    printf("\n");

    for (int i = 0; i < node->num_children; i++)
        ast_print(node->children[i], depth + 1);

    if (node->left && node->type != AST_BINARY_OP && node->type != AST_CALL && node->type != AST_MEMBER_ACCESS)
        ast_print(node->left, depth + 1);
    if (node->right && node->type != AST_BINARY_OP && node->type != AST_CALL && node->type != AST_MEMBER_ACCESS)
        ast_print(node->right, depth + 1);
}
