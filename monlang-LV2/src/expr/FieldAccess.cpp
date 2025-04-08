#include <monlang-LV2/expr/FieldAccess.h>

/* impl only */
#include <monlang-LV1/ast/Path.h>

/* require knowing all words for token_len() */
#include <monlang-LV2/ast/token_len.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>
#include <utils/mem-utils.h>

#define unless(x) if(!(x))

// sum object (path left part) token len and Path separator seq
// .., and add it to error offset
#define SET_FIELD_ERR_OFFSET(error) \
    auto err_offset = size_t(0); \
    err_offset += token_len(unwrap_expr(object.value())); \
    err_offset += sequenceLen(Path::SEPARATOR_SEQUENCE); \
    error._info["err_offset"] = err_offset

bool peekFieldAccess(const Word& word) {
    return std::holds_alternative<Path*>(word);
}

MayFail<MayFail_<FieldAccess>> buildFieldAccess(const Word& word) {
    ASSERT (std::holds_alternative<Path*>(word));
    auto path = *std::get<Path*>(word);
    auto object = buildExpression((Term)variant_cast(path.leftPart));
    if (object.has_error()) {
        return Malformed(MayFail_<FieldAccess>{object, Symbol{}}, ERR(711));
    }

    auto atom_ptr = move_to_heap(path.rightPart);
    auto expr = buildExpression((Term)atom_ptr);
    delete atom_ptr;
    unless (std::holds_alternative<Symbol*>(expr.val)) {
        auto error = ERR(712);
        SET_FIELD_ERR_OFFSET(error);
        return Malformed(MayFail_<FieldAccess>{object, Symbol{}}, error);
    }
    auto field = *std::get<Symbol*>(expr.val);

    auto fieldAccess = MayFail_<FieldAccess>{object, field};
    fieldAccess._tokenLen = path._tokenLen;
    return fieldAccess;
}

FieldAccess::FieldAccess(const Expression& object, const Symbol& field)
        : object(object), field(field){}

MayFail_<FieldAccess>::MayFail_(const MayFail<Expression_>& object, const Symbol& field)
        : object(object), field(field){}

MayFail_<FieldAccess>::MayFail_(FieldAccess fieldAccess) {
    auto object = wrap_expr(fieldAccess.object);
    auto field = fieldAccess.field;

    this->object = object;
    this->field = field;
    this->_lvalue = fieldAccess._lvalue;
    this->_tokenLen = fieldAccess._tokenLen;
}

MayFail_<FieldAccess>::operator FieldAccess() const {
    auto object = unwrap_expr(this->object.value());
    auto field = this->field;
    auto fieldAccess = FieldAccess{object, field};
    fieldAccess._lvalue = this->_lvalue;
    fieldAccess._tokenLen = this->_tokenLen;
    return fieldAccess;
}
