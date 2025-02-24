#ifndef LVALUE_H
#define LVALUE_H

#include <monlang-LV2/ast/Lvalue.h>

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/expr/Symbol.h>
// #include <monlang-LV2/expr/Subscript.h>
// #include <monlang-LV2/expr/FieldAccess.h>

#include <monlang-LV1/ast/Word.h>

struct Lvalue_ {
    using Variant = std::variant<
        Symbol*
        // MayFail_<Subscript>*,
        // MayFail_<FieldAccess>*
    >;
    Variant variant;

    bool _stub = false;

    /* any cast below will fail if _lvalue field isn't true */
    Lvalue_(Symbol*);
    // Lvalue_(MayFail_<Subscript>*);
    // Lvalue_(MayFail_<FieldAccess>*);
    Lvalue_(Expression_); // we need this

    operator Expression_() const; // we need this

    explicit Lvalue_(_dummy_stub); // sets _stub to true
};

Lvalue unwrap_lvalue(Lvalue_);
Lvalue_ wrap_lvalue(Lvalue);

#endif // LVALUE_H
