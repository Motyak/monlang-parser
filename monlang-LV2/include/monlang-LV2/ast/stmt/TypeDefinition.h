#ifndef AST_TYPE_DEFINITION_H
#define AST_TYPE_DEFINITION_H

#include <monlang-LV2/ast/expr/Symbol.h>

#include <vector>
#include <string>

struct TypeDefinition {
    static const std::string KEYWORD;

    Symbol type;
    std::vector<Symbol> subtypes;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    TypeDefinition() = default;
    TypeDefinition(const Symbol&, const std::vector<Symbol>&);
};

#endif // AST_TYPE_DEFINITION_H
