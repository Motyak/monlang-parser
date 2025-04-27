#ifndef AST_ASSIGNMENT_H
#define AST_ASSIGNMENT_H

#include <monlang-LV2/ast/Lvalue.h>
#include <monlang-LV2/ast/Expression.h>

#include <string>
#include <cstddef>

struct Assignment {
    static const std::string SEPARATOR;

    Lvalue variable;
    Expression value;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    Assignment() = default;
    Assignment(const Lvalue&, const Expression&);
};

#endif // AST_ASSIGNMENT_H
