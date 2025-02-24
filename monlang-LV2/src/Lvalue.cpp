#include <monlang-LV2/Lvalue.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

Lvalue unwrap_lvalue(Lvalue_ lvalue) {
    return std::visit(overload{
        [](Symbol* symbol) -> Lvalue {return symbol;},
        [](auto* mf_) -> Lvalue {return unwrap(mf_);},
    }, lvalue.variant);
}

Lvalue_ wrap_lvalue(Lvalue lvalue) {
    return std::visit(overload{
        [](Symbol* symbol) -> Lvalue_ {return symbol;},
        [](auto* other) -> Lvalue_ {return wrap(other);},
    }, lvalue.variant);
}

Lvalue::Lvalue(Symbol* symbol) {
    ASSERT (symbol->_lvalue);
    this->variant = symbol;
}

Lvalue::Lvalue(Expression expr) {
    std::visit(overload{
        [this](Symbol* symbol){*this = Lvalue(symbol);},
        // [this](Subscript* subscript){*this = Lvalue(subscript);},
        // [this](FieldAccess* fieldAccess){*this = Lvalue(fieldAccess);},
        [this](auto*){SHOULD_NOT_HAPPEN();},
    }, expr);
}

Lvalue::operator Expression() const {
    return (Expression)variant_cast(this->variant);
}

Lvalue::Lvalue(_dummy_stub) : _stub(true){}

//////////////////////

Lvalue_::Lvalue_(Symbol* symbol) {
    ASSERT (symbol->_lvalue);
    this->variant = symbol;
}

Lvalue_::Lvalue_(Expression_ expr) {
    std::visit(overload{
        [this](Symbol* symbol){*this = Lvalue_(symbol);},
        // [this](MayFail_<Subscript>* subscript){*this = Lvalue_(subscript);},
        // [this](MayFail_<FieldAccess>* fieldAccess){*this = Lvalue_(fieldAccess);},
        [this](auto*){SHOULD_NOT_HAPPEN();},
    }, expr);
}

Lvalue_::operator Expression_() const {
    return (Expression_)variant_cast(this->variant);
}

Lvalue_::Lvalue_(_dummy_stub) : _stub(true){}
