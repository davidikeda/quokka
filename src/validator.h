//
// Created by Ikeda, David on 2/18/26.
//

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "ast.h"

typedef struct
{
    int error_count;
    int warning_count;
    char errors[1024][256]; // error message storing
} ValidationResult;

ValidationResult* validator_validate(ASTNode *ast);
void validator_print_errors(ValidationResult *result);
void validator_free(ValidationResult *result);
#endif //VALIDATOR_H
