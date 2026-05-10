#ifndef AST_ENUM_DEFINITION_H
#define AST_ENUM_DEFINITION_H

#include <monlang-LV2/ast/expr/Symbol.h>
#include <monlang-LV2/ast/Expression.h>

#include <vector>
#include <string>

struct EnumDefinition {
    static const std::string KEYWORD;

    Symbol enum_;
    struct EnumValue {
        Symbol enumerator; // enum value "name"
        Expression enumerate; // enum value "value"

        size_t _tokenLeadingNewlines = 0;
        size_t _tokenIndentSpaces = 0;
        size_t _tokenLen = 0;
        size_t _tokenTrailingNewlines = 0;
        size_t _tokenId = 123456789;

        EnumValue() = default;
        EnumValue(const Symbol&, const Expression&);
    };
    std::vector<EnumValue> enumValues;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0; // TODO: no need
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    EnumDefinition() = default;
    EnumDefinition(const Symbol&, const std::vector<EnumValue>&);
};

#endif // AST_ENUM_DEFINITION_H
