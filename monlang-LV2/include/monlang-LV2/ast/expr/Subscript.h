#ifndef AST_SUBSCRIPT_H
#define AST_SUBSCRIPT_H

#include <monlang-LV2/ast/Expression.h>

#include <variant>
#include <cstddef>

struct Subscript {
    using IndexExpression = std::variant<
        Numeral*,
        SpecialSymbol*,
        Symbol*
    >;
    struct Index {
        IndexExpression nth;
    };
    struct Range {
        IndexExpression from;
        IndexExpression to;
        bool exclusive = false;
    };
    struct Key {
        Expression expr;
    };
    using Argument = std::variant<Index, Range, Key>;

    enum Suffix {
        NONE = 0,
        EXCLAMATION_MARK = 33,
        QUESTION_MARK = 63,
    };

    Expression array;
    Argument argument;
    Suffix suffix = Suffix::NONE;

    bool _lvalue = false;
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    Subscript() = default;
    Subscript(const Expression&, const Argument&, Suffix);
};

#endif // AST_SUBSCRIPT_H
