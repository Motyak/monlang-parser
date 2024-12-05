#ifndef AST_LET_STATEMENT_H
#define AST_LET_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>

using identifier_t = std::string;

struct LetStatement {
    identifier_t identifier;
    Expression value;
};

#endif // AST_LET_STATEMENT_H
