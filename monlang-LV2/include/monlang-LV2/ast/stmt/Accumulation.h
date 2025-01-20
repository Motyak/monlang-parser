#ifndef AST_ACCUMULATION_H
#define AST_ACCUMULATION_H

#include <monlang-LV2/ast/expr/Lvalue.h>
#include <monlang-LV2/ast/Expression.h>

using identifier_t = std::string;

struct Accumulation {
    static const std::string SEPARATOR_SUFFIX;

    Lvalue variable;
    identifier_t operator_;
    Expression value;

    Atom SEPARATOR(); // operator_ + SUFFIX

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    Accumulation() = default;
    Accumulation(const Lvalue&, const identifier_t&, const Expression&);
};

#endif // AST_ACCUMULATION_H
