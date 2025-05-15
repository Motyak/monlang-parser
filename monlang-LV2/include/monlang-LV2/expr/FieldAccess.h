#ifndef FIELD_ACCESS_H
#define FIELD_ACCESS_H

#include <monlang-LV2/ast/expr/FieldAccess.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Word.h>

template <>
struct MayFail_<FieldAccess> {
    MayFail<Expression_> object;
    Symbol field;

    bool _lvalue = false;
    size_t _tokenLen = 0;
    size_t _tokenId = 123456789;
    size_t _groupNesting = 0;
    MayFail_() = default;
    explicit MayFail_(const MayFail<Expression_>&, const Symbol&);

    explicit MayFail_(FieldAccess);
    explicit operator FieldAccess() const;
};

bool peekFieldAccess(const Word&);

MayFail<MayFail_<FieldAccess>> buildFieldAccess(const Word&);

#endif // FIELD_ACCESS_H
