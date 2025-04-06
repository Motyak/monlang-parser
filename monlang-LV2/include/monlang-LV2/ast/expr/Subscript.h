#ifndef AST_SUBSCRIPT_H
#define AST_SUBSCRIPT_H

#include <monlang-LV2/ast/Expression.h>

#include <variant>

struct Subscript {
    struct Index {
        Expression expr;
    };
    struct Range {
        Expression from;
        Expression to;
        bool exclusive = false;
    };
    struct Key {
        Expression expr;
    };
    using Argument = std::variant<Index, Range, Key>;

    Expression array;
    Argument argument;

    size_t _tokenLen = 0;
    Subscript() = default;
    Subscript(const Expression&, const Argument&);
};

#endif // AST_SUBSCRIPT_H
