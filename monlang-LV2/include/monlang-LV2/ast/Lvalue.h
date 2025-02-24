#ifndef AST_LVALUE_H
#define AST_LVALUE_H

#include <monlang-LV2/ast/Expression.h>
#include <monlang-LV2/ast/expr/Symbol.h>
// #include <monlang-LV2/ast/expr/Subscript.h>
// #include <monlang-LV2/ast/expr/FieldAccess.h>

#include <utils/stub-ctor.h>

#include <variant>

struct Lvalue {
    using Variant = std::variant<
        Symbol*
        // Subscript*,
        // FieldAccess*
    >;
    Variant variant;

    bool _stub = false;

    /* any cast below will fail if _lvalue field isn't true */
    Lvalue(Symbol*);
    // Lvalue(Subscript*);
    // Lvalue(FieldAccess*);
    Lvalue(Expression); // we need this

    operator Expression() const; // we need this

    explicit Lvalue(_dummy_stub); // sets _stub to true
};

#endif // AST_LVALUE_H
