#ifndef LIST_LITERAL_H
#define LIST_LITERAL_H

#include <monlang-LV2/ast/expr/ListLiteral.h>
#include <monlang-LV2/Expression.h>

#include <utils/MayFail.h>
#include <utils/stub-ctor.h>

template <>
struct MayFail_<ListLiteral> {
    std::vector<MayFail<Expression_>> arguments;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const std::vector<MayFail<Expression_>>&);

    explicit MayFail_(ListLiteral);
    explicit operator ListLiteral() const;
};

// peekListLiteral would be..
// .. !peekMapLiteral(word) && std::holds_alternative<SquareBracketsGroup*>(word)

MayFail<MayFail_<ListLiteral>> buildListLiteral(const Word&);

#endif // LIST_LITERAL_H
