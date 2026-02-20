//
// Created by Ikeda, David on 2/18/26.
//

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parser* parser_init(Lexer *lexer)
{
    Parser *p = malloc(sizeof(Parser));
    p->lexer = lexer;
    p->current = lexerNextToken(lexer);
    p->peek = lexerNextToken(lexer);
    p->error_count = 0;
    return p;
}

static void parser_advance(Parser *p)
{
    if (p->current.value) free(p->current.value);
    p->current = p->peek;
    p->peek = lexerNextToken(p->lexer);
}

static void parser_error(Parser *p, const char *msg)
{
    fprintf(stderr, "[%d:%d] Parse error: %s\n",
        p->current.line, p->current.column, msg);
    p->error_count++;
}

static int parser_match(Parser *p, TokenType type)
{
    if (p->current.type == type)
    {
        parser_advance(p);
        return 1;
    }
    return 0;
}

static int parser_check(Parser *p, TokenType type)
{
    return p->current.type == type;
}

static Token parser_consume(Parser *p, TokenType type, const char *msg)
{
    Token t = p->current;
    if (p->current.type != type)
    {
        parser_error(p, msg);
        t.type = TOK_UNKNOWN;
    }
    parser_advance(p);
    return t;
}

// forward decs
static ASTNode* parser_parse_statement(Parser *p);
static ASTNode* parser_parse_expression(Parser *p);
static ASTNode* parser_parse_comparison(Parser *p);
static ASTNode* parser_parse_primary(Parser *p);
static ASTNode* parser_parse_call_or_member(Parser *p, ASTNode *object);
static ASTNode* parser_parse_block(Parser *p);
static ASTNode* parser_parse_if(Parser *p);
static ASTNode* parser_parse_import(Parser *p);
static ASTNode* parser_parse_declaration(Parser *p);
static ASTNode* parser_parse_expression_statement(Parser *p);


static ASTNode* parser_parse_program(Parser *p)
{
    ASTNode *program = ast_create(AST_PROGRAM, p->current.line, p->current.column);

    while (!parser_check(p, TOK_EOF))
    {
        ASTNode *stmt = parser_parse_statement(p);
        if (stmt)
        {
            ast_add_child(program, stmt);
        }
    }
    return program;
}

static ASTNode* parser_parse_expression(Parser *p)
{
    return parser_parse_comparison(p);
}

static ASTNode* parser_parse_import(Parser *p)
{
    int line = p->current.line;
    int col = p->current.column;

    parser_consume(p, TOK_AT, "Expected '@'");
    parser_consume(p, TOK_IMPORT, "Expected 'import'");

    char *path_value = p->current.value ? strdup(p->current.value) : strdup("");
    parser_consume(p, TOK_STRING, "Expected string path");
    parser_consume(p, TOK_SEMICOLON, "Expected ';' after import");

    ASTNode *import = ast_create(AST_IMPORT, line, col);
    import->string_value = path_value;

    return import;
}

static ASTNode* parser_parse_declaration(Parser *p)
{
    int line = p->current.line;
    int col = p->current.column;

    parser_consume(p, TOK_NEW, "Expected 'new'");
    char *device_type_value = p->current.value ? strdup(p->current.value) : strdup("");
    parser_consume(p, TOK_IDENTIFIER, "Expected device type");
    char *device_name_value = p->current.value ? strdup(p->current.value) : strdup("");
    parser_consume(p, TOK_IDENTIFIER, "Expected device name");

    parser_consume(p, TOK_AS, "Expected 'as'");
    char *alias_value = p->current.value ? strdup(p->current.value) : strdup("");
    parser_consume(p, TOK_IDENTIFIER, "Expected alias name");

    parser_consume(p, TOK_SEMICOLON, "Expected ';' after declaration");

    ASTNode *decl = ast_create(AST_DECLARATION, line, col);
    decl->string_value = strdup(device_name_value);
    ASTNode *type_node = ast_create_identifier(device_type_value, line, col);
    ASTNode *alias_node = ast_create_identifier(alias_value, line, col);

    ast_add_child(decl, type_node);
    ast_add_child(decl, alias_node);

    free(device_type_value);
    free(device_name_value);
    free(alias_value);

    return decl;
}
static ASTNode* parser_parse_block(Parser *p)
{
    int line = p->current.line;
    int col = p->current.column;

    parser_consume(p, TOK_LBRACE, "Expected '{'");
    ASTNode *block = ast_create(AST_BLOCK, line, col);

    while (!parser_check(p, TOK_RBRACE) && !parser_check(p, TOK_EOF))
    {
        ASTNode *stmt = parser_parse_statement(p);
        if (stmt)
            ast_add_child(block, stmt);
    }

    parser_consume(p, TOK_RBRACE, "Expected '}'");

    return block;
}

static ASTNode* parser_parse_if(Parser *p)
{
    int line = p->current.line;
    int col = p->current.column;

    parser_consume(p, TOK_IF, "Expected 'if'");
    parser_consume(p, TOK_LPAREN, "Expected '(' after 'if'");
    ASTNode *condition = parser_parse_expression(p);
    parser_consume(p, TOK_RPAREN, "Expected ')' after condition");
    parser_consume(p, TOK_THEN, "Expected 'then' after condition");

    ASTNode *then_block = parser_parse_block(p);
    ASTNode *else_block = NULL;

    if (parser_match(p, TOK_ELSE))
    {
        else_block = parser_parse_block(p);
    }

    parser_consume(p, TOK_SEMICOLON, "Expected ';' after if statement");

    ASTNode *if_stmt = ast_create(AST_IF_STMT, line, col);
    ast_add_child(if_stmt, condition);
    ast_add_child(if_stmt, then_block);
    if (else_block)
        ast_add_child(if_stmt, else_block);

    return if_stmt;
}

static ASTNode* parser_parse_arguments(Parser *p)
{
    ASTNode *args = ast_create(AST_ARGUMENTS, p->current.line, p->current.column);

    if (!parser_check(p, TOK_RPAREN))
    {
        do
        {
            // Handle named arguments: name="value"
            if (p->peek.type == TOK_ASSIGN && parser_check(p, TOK_IDENTIFIER))
            {
                char *name_value = p->current.value ? strdup(p->current.value) : strdup("");  // Copy FIRST
                int name_line = p->current.line;
                int name_col = p->current.column;

                parser_advance(p);
                parser_consume(p, TOK_ASSIGN, "Expected '='");
                ASTNode *value = parser_parse_primary(p);

                ASTNode *arg = ast_create(AST_BINARY_OP, name_line, name_col);
                arg->op = strdup("=");
                arg->left = ast_create_identifier(name_value, name_line, name_col);  // Use copied value
                free(name_value);
            } else
            {
                ASTNode *expr = parser_parse_expression(p);
                ast_add_child(args, expr);
            }
        } while (parser_match(p, TOK_COMMA));
    }

    return args;
}

static ASTNode* parser_parse_call_or_member(Parser *p, ASTNode *object)
{
    while (parser_match(p, TOK_DOT))
    {
        int line = p->current.line;
        int col = p->current.column;

        // Accept either IDENTIFIER or keywords that can be method names
        char *member_name = NULL;

        if (parser_check(p, TOK_IDENTIFIER))
        {
            member_name = strdup(p->current.value ? p->current.value : "");
            parser_advance(p);
        }
        else if (parser_check(p, TOK_CONNECT))
        {
            member_name = strdup("connect");
            parser_advance(p);
        }
        else if (parser_check(p, TOK_DISCONNECT))
        {
            member_name = strdup("disconnect");
            parser_advance(p);
        }
        else if (parser_check(p, TOK_WRITE))
        {
            member_name = strdup("write");
            parser_advance(p);
        }
        else if (parser_check(p, TOK_READ))
        {
            member_name = strdup("read");
            parser_advance(p);
        }
        else if (parser_check(p, TOK_SEND))
        {
            member_name = strdup("send");
            parser_advance(p);
        }
        else if (parser_check(p, TOK_RECEIVE))
        {
            member_name = strdup("receive");
            parser_advance(p);
        }
        else
        {
            parser_error(p, "Expected member name");
            return object;
        }

        ASTNode *member_node = ast_create_identifier(member_name, line, col);
        free(member_name);

        ASTNode *access = ast_create_member(object, member_node, object->line, object->column);

        // Check for function call
        if (parser_check(p, TOK_LPAREN))
        {
            parser_consume(p, TOK_LPAREN, "Expected '('");
            ASTNode *args = parser_parse_arguments(p);
            parser_consume(p, TOK_RPAREN, "Expected ')' after arguments");

            ASTNode *call = ast_create_call(access, args, access->line, access->column);
            object = call;
        } else
        {
            object = access;
        }
    }

    return object;
}

static ASTNode* parser_parse_expression_statement(Parser *p)
{
    ASTNode *expr = parser_parse_expression(p);
    parser_consume(p, TOK_SEMICOLON, "Expected ';' after expression");

    ASTNode *stmt = ast_create(AST_EXPR, expr->line, expr->column);
    stmt->left = expr;
    return stmt;
}

static ASTNode* parser_parse_statement(Parser *p)
{
    if (parser_check(p, TOK_AT))
        return parser_parse_import(p);
    if (parser_check(p, TOK_NEW))
        return parser_parse_declaration(p);
    if (parser_check(p, TOK_IF))
        return parser_parse_if(p);
    if (parser_check(p, TOK_EOF))
        return NULL;

    return parser_parse_expression_statement(p);
}

static ASTNode* parser_parse_comparison(Parser *p)
{
    ASTNode *left = parser_parse_primary(p);

    while (parser_check(p, TOK_EQUAL) || parser_check(p, TOK_NOT_EQUAL) || parser_check(p, TOK_LT) || parser_check(p, TOK_GT) || parser_check(p, TOK_LE) || parser_check(p, TOK_GE))
    {
        const char *op;
        int line = p->current.line;
        int col = p->current.column;

        if (parser_match(p, TOK_EQUAL)) op = "==";
        else if (parser_match(p, TOK_NOT_EQUAL)) op = "!=";
        else if (parser_match(p, TOK_LT)) op = "<";
        else if (parser_match(p, TOK_GT)) op = ">";
        else if (parser_match(p, TOK_LE)) op = "<=";
        else if (parser_match(p, TOK_GE)) op = ">=";
        else break;

        ASTNode *right = parser_parse_primary(p);
        left = ast_create_binary(left, op, right, line, col);
    }

    return left;
}

static ASTNode* parser_parse_primary(Parser *p)
{
    int line = p->current.line;
    int col = p->current.column;

    if (parser_check(p, TOK_NUMBER))
    {
        double value = strtod(p->current.value ? p->current.value : "0", NULL);
        parser_advance(p);
        return ast_create_number(value, line, col);
    }

    if (parser_check(p, TOK_STRING))
    {
        char *value = p->current.value ? strdup(p->current.value) : strdup("");
        parser_advance(p);
        ASTNode *str_node = ast_create_string(value, line, col);
        free(value);
        return str_node;
    }

    if (parser_check(p, TOK_IDENTIFIER))
    {
        char *ident_value = p->current.value ? strdup(p->current.value) : strdup("");
        parser_advance(p);
        ASTNode *ident = ast_create_identifier(ident_value, line, col);
        free(ident_value);

        if (parser_check(p, TOK_LPAREN))
        {
            parser_consume(p, TOK_LPAREN, "Expected '('");
            ASTNode *args = parser_parse_arguments(p);
            parser_consume(p, TOK_RPAREN, "Expected ')' after arguments");
            ASTNode *call = ast_create_call(ident, args, line, col);
            return call;
        }
        return parser_parse_call_or_member(p, ident);
    }

    if (parser_match(p, TOK_LPAREN))
    {
        ASTNode *expr = parser_parse_expression(p);
        parser_consume(p, TOK_RPAREN, "Expected ')' after expression");
        return expr;
    }

    parser_error(p, "Unexpected token");
    return ast_create(AST_IDENTIFIER, line, col);
}

ASTNode* parser_parse(Parser *p)
{
    return parser_parse_program(p);
}

void parser_free(Parser *p)
{
    if (!p) return;

    if (p->current.value) free(p->current.value);
    if (p->peek.value) free(p->peek.value);
    free(p);
}
