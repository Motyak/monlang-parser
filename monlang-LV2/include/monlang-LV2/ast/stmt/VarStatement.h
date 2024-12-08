#ifndef AST_VAR_STATEMENT_H
#define AST_VAR_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>

using identifier_t = std::string;

struct VarStatement {
    identifier_t identifier;
    Expression value;
};

#endif // AST_VAR_STATEMENT_H
