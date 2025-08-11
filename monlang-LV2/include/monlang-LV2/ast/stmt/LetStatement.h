#ifndef AST_LET_STATEMENT_H
#define AST_LET_STATEMENT_H

#include <monlang-LV2/ast/expr/Symbol.h>
#include <monlang-LV2/ast/Lvalue.h>

#include <string>
#include <cstddef>

struct LetStatement {
    static const std::string KEYWORD;

    Symbol alias;
    Lvalue variable;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    LetStatement() = default;
    LetStatement(const Symbol&, const Lvalue&);
};

#endif // AST_LET_STATEMENT_H
