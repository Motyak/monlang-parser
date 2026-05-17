#ifndef LIST_LITERAL_H
#define LIST_LITERAL_H

#include <monlang-LV2/ast/expr/ListLiteral.h>
#include <monlang-LV2/Expression.h>

#include <utils/MayFail.h>

template <>
struct MayFail_<ListLiteral> {
    struct Argument {
        std::optional<MayFail<Expression_>> expr;

        size_t _tokenLen = 0;
        size_t _tokenId = 123456789;
        Argument() = default;
        Argument(const std::optional<MayFail<Expression_>>&);
    };
    std::vector<Argument> arguments;

    std::optional<std::any> _msbg = std::nullopt; // MultilineSquareBracketsGroup
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const std::vector<Argument>&);

    explicit MayFail_(ListLiteral);
    explicit operator ListLiteral() const;
};

// peekListLiteral would be..
// .. !peekMapLiteral(word) && std::holds_alternative<SquareBracketsGroup*>(word)

MayFail<MayFail_<ListLiteral>> buildListLiteral(const Word&);

#endif // LIST_LITERAL_H
