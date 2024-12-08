#ifndef AST_RETURN_STATEMENT_H
#define AST_RETURN_STATEMENT_H

#include <monlang-LV2/ast/Expression.h>

#include <optional>

struct ReturnStatement {
    std::optional<Expression> value;
};

#endif // AST_RETURN_STATEMENT_H
