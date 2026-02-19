//
// Created by Ikeda, David on 2/17/26.
//

#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    // statements
    AST_PROGRAM,
    AST_IMPORT,
    AST_DECLARATION,
    AST_ASSIGNMENT,
    AST_IF_STMT,
    AST_BLOCK,
    AST_EXPR,
    AST_FUNCTION_DEF,

    // Expressions
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_STRING,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_CALL,
    AST_MEMBER_ACCESS,
    AST_ARGUMENTS,
} ASTNodeType;

typedef struct ASTNode
{
    ASTNodeType type;
    int line;
    int column;

    // Generic fields for nodes
    char *string_value;
    double number_value;
    char *op;

    // children
    struct ASTNode **children;
    int num_children;

    // Special node types
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

// constructors
ASTNode* ast_create(ASTNodeType type, int line, int column);
ASTNode* ast_create_identifier(const char *name, int line, int column);
ASTNode* ast_create_number(double value, int line, int column);
ASTNode* ast_create_string(const char *value, int line, int column);
ASTNode* ast_create_binary(ASTNode *left, const char *op, ASTNode *right, int line, int column);
ASTNode* ast_create_call(ASTNode *callee, ASTNode *args, int line, int column);
ASTNode* ast_create_member(ASTNode *object, ASTNode *member, int line, int column);

// utility
void ast_add_child(ASTNode *parent, ASTNode *child);
void ast_free(ASTNode *node);
void ast_print(ASTNode *node, int depth);
#endif //AST_H
