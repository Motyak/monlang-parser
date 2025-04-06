#ifndef SUBSCRIPT_H
#define SUBSCRIPT_H

#include <monlang-LV2/ast/expr/Subscript.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Word.h>

template <>
struct MayFail_<Subscript> {
    struct _StubArgument_ {};
    using Index = Subscript::Index;
    using Range = Subscript::Range;
    struct Key {
        MayFail<Expression_> expr;
    };
    using Argument = std::variant<_StubArgument_, Index, Range, Key>;

    MayFail<Expression_> array;
    Argument argument;

    size_t _tokenLen = 0;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const MayFail<Expression_>&, const Argument&);

    explicit MayFail_(Subscript);
    explicit operator Subscript() const;
};

bool peekSubscript(const Word&);

MayFail<MayFail_<Subscript>> buildSubscript(const Word&);

#endif // SUBSCRIPT_H
