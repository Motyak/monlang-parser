#ifndef MAP_LITERAL_H
#define MAP_LITERAL_H

#include <monlang-LV2/ast/expr/MapLiteral.h>
#include <monlang-LV2/Expression.h>

#include <utils/MayFail.h>
#include <utils/stub-ctor.h>

template <>
struct MayFail_<MapLiteral> {
    using Argument = std::pair<MayFail<Expression_>, MayFail<Expression_>>;
    std::vector<Argument> arguments;

    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const std::vector<Argument>&);

    explicit MayFail_(MapLiteral);
    explicit operator MapLiteral() const;
};

bool peekMapLiteral(const Word&);

MayFail<MayFail_<MapLiteral>> buildMapLiteral(const Word&);

#endif // MAP_LITERAL_H
