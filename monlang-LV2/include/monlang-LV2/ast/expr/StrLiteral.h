#ifndef AST_STR_LITERAL_H
#define AST_STR_LITERAL_H

#include <string>

struct StrLiteral {
    std::string str;

    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    size_t _groupNesting = 0; // special case, because can't be malformed
    StrLiteral() = default;
    StrLiteral(const std::string&);
};

#endif // AST_STR_LITERAL_H
