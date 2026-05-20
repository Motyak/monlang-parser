#ifndef MAP_LITERAL_H
#define MAP_LITERAL_H

#include <monlang-LV2/ast/expr/MapLiteral.h>
#include <monlang-LV2/Expression.h>

#include <utils/MayFail.h>

template <>
struct MayFail_<MapLiteral> {
    struct Argument {
        struct Pair {
            MayFail<Expression_> key;
            MayFail<Expression_> value;
        };
        std::optional<Pair> pair;

        size_t _tokenLen = 0;
        size_t _tokenId = 123456789;
        Argument() = default;
        Argument(const MayFail<Expression_>&, const MayFail<Expression_>&);
    };
    std::vector<MayFail<Argument>> arguments;

    std::optional<std::any> _msbg = std::nullopt; // MultilineSquareBracketsGroup
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const std::vector<MayFail<Argument>>&);

    explicit MayFail_(MapLiteral);
    explicit operator MapLiteral() const;
};

bool peekMapLiteral(const Word&);

MayFail<MayFail_<MapLiteral>> buildMapLiteral(const Word&);

#endif // MAP_LITERAL_H
