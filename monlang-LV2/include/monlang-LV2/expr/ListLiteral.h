#ifndef LIST_LITERAL_H
#define LIST_LITERAL_H

#include <monlang-LV2/ast/expr/ListLiteral.h>
#include <monlang-LV2/Expression.h>

#include <utils/MayFail.h>
#include <utils/stub-ctor.h>

template <>
struct MayFail_<ListLiteral> {
    std::vector<MayFail<Expression_>> arguments;

    // bool _stub = false;
    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    MayFail_() = default;
    // explicit MayFail_(_dummy_stub); // sets _stub to true
    explicit MayFail_(const std::vector<MayFail<Expression_>>&);

    explicit MayFail_(ListLiteral);
    explicit operator ListLiteral() const;
};

bool peekListLiteral(const Word&);

MayFail<MayFail_<ListLiteral>> buildListLiteral(const Word&);

#endif // LIST_LITERAL_H
