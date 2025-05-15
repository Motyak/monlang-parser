#ifndef AST_FIELD_ACCESS_H
#define AST_FIELD_ACCESS_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/Symbol.h>

#include <vector>

struct FieldAccess {
    Expression object;
    Symbol field;

    bool _lvalue = false;
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    FieldAccess() = default;
    FieldAccess(const Expression&, const Symbol&);
};

#endif // AST_FIELD_ACCESS_H
