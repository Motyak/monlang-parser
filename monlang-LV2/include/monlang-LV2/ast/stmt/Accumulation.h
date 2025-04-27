#ifndef AST_ACCUMULATION_H
#define AST_ACCUMULATION_H

#include <monlang-LV2/Lvalue.h>
#include <monlang-LV2/ast/expr/Symbol.h>
#include <monlang-LV2/ast/Expression.h>

#include <string>
#include <cstddef>

struct Accumulation {
    static const std::string SEPARATOR_SUFFIX;

    Lvalue variable;
    Symbol operator_;
    Expression value;

    std::string SEPARATOR(); // operator_ + SUFFIX

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    Accumulation() = default;
    Accumulation(const Lvalue&, const Symbol&, const Expression&);
};

#endif // AST_ACCUMULATION_H
