#ifndef AST_STRUCT_DEFINITION_H
#define AST_STRUCT_DEFINITION_H

#include <monlang-LV2/ast/expr/Symbol.h>

#include <vector>
#include <string>
#include <optional>

struct StructDefinition {
    static const std::string KEYWORD;

    Symbol struct_;
    struct Field {
        struct Pair {
            Symbol type;
            Symbol name;
        };
        std::optional<Pair> pair;

        size_t _tokenLeadingNewlines = 0;
        size_t _tokenIndentSpaces = 0;
        size_t _tokenLen = 0;
        size_t _tokenTrailingNewlines = 0;
        size_t _tokenId = 123456789;

        Field() = default;
        Field(const Symbol&, const Symbol&);
    };
    std::vector<Field> fields;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0;
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    StructDefinition() = default;
    StructDefinition(const Symbol&, const std::vector<Field>&);
};

#endif // AST_STRUCT_DEFINITION_H
