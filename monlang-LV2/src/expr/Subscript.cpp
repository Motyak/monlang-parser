#include <monlang-LV2/expr/Subscript.h>

/* impl only */
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>
#include <utils/str-utils.h>
#include <utils/mem-utils.h>

#define unless(x) if(!(x))

bool peekSubscript(const Word& word) {
    unless (std::holds_alternative<PostfixSquareBracketsGroup*>(word)) {
        return false;
    }
    auto psbg = *std::get<PostfixSquareBracketsGroup*>(word);
    return psbg.rightPart.terms.size() == 1;
}

static Subscript::IndexExpression to_index_expr(const Atom&);

MayFail<MayFail_<Subscript>> buildSubscript(const Word& word) {
    ASSERT (std::holds_alternative<PostfixSquareBracketsGroup*>(word));
    auto psbg = *std::get<PostfixSquareBracketsGroup*>(word);
    auto suffix = MayFail_<Subscript>::Suffix(psbg._suffix);
    auto array = buildExpression((Term)variant_cast(psbg.leftPart));
    if (array.has_error()) {
        return Malformed(MayFail_<Subscript>{array, /*_StubArgument_*/{}, suffix}, ERR(721));
    }

    ASSERT (psbg.rightPart.terms.size() == 1);

    /* breakable block */ for (int i = 1; i <= 1; ++i)
    {
        unless (psbg.rightPart.terms[0].words.size() == 1) break;
        unless (std::holds_alternative<Atom*>(psbg.rightPart.terms[0].words[0])) break;
        auto str = std::get<Atom*>(psbg.rightPart.terms[0].words[0])->value;
        ASSERT (str.size() > 0);
        unless (str[0] == '#') break;
        str = split_in_two(str, "#").at(1);
        unless (str != "") break;
        /* breakable block */ for (int i = 1; i <= 1; ++i)
        {
            if (split_in_two(str, "..<").size() == 2) {
                unless (split_in_two(str, "..<").at(0).size() > 0) break;
                unless (split_in_two(str, "..<").at(1).size() > 0) break;
                auto atom_from = Atom{split_in_two(str, "..<").at(0)};
                atom_from._tokenLen = atom_from.value.size();
                auto atom_to = Atom{split_in_two(str, "..<").at(1)};
                atom_to._tokenLen = atom_to.value.size();
                auto from = to_index_expr(atom_from);
                auto to = to_index_expr(atom_to);
                auto range = MayFail_<Subscript>::Range{from, to, /*exclusive*/true};
                range._tokenLen = std::string("#").size() + atom_from._tokenLen
                        + std::string("..<").size() + atom_to._tokenLen;
                auto subscript = MayFail_<Subscript>{array, range, suffix};
                subscript._tokenLen = psbg._tokenLen;
                return subscript;
            }
            if (split_in_two(str, "..").size() == 2) {
                unless (split_in_two(str, "..").at(0).size() > 0) break;
                unless (split_in_two(str, "..").at(1).size() > 0) break;
                auto atom_from = Atom{split_in_two(str, "..").at(0)};
                atom_from._tokenLen = atom_from.value.size();
                auto atom_to = Atom{split_in_two(str, "..").at(1)};
                atom_to._tokenLen = atom_to.value.size();
                auto from = to_index_expr(atom_from);
                auto to = to_index_expr(atom_to);
                auto range = MayFail_<Subscript>::Range{from, to};
                range._tokenLen = std::string("#").size() + atom_from._tokenLen
                        + std::string("..").size() + atom_to._tokenLen;
                auto subscript = MayFail_<Subscript>{array, range, suffix};
                subscript._tokenLen = psbg._tokenLen;
                return subscript;
            }
        }
        auto atom_index = Atom{str};
        atom_index._tokenLen = str.size();
        auto nth = to_index_expr(atom_index);
        auto index = MayFail_<Subscript>::Index{nth};
        index._tokenLen = std::string("#").size() + atom_index._tokenLen;
        auto subscript = MayFail_<Subscript>{array, index, suffix};
        subscript._tokenLen = psbg._tokenLen;
        return subscript;
    }

    auto expr = buildExpression(psbg.rightPart.terms[0]);
    auto key = MayFail_<Subscript>::Key{expr};
    if (expr.has_error()) {
        return Malformed(MayFail_<Subscript>{array, key, suffix}, ERR(722));
    }

    auto subscript = MayFail_<Subscript>{array, key, suffix};
    subscript._tokenLen = psbg._tokenLen;
    return subscript;
}

static Subscript::IndexExpression to_index_expr(const Atom& atom) {
    auto expr = buildExpression((Term)move_to_heap(atom)).value();
    return std::visit(overload{
        [](Numeral* numeral) -> Subscript::IndexExpression {return numeral;},
        [](SpecialSymbol* ss) -> Subscript::IndexExpression {return ss;},
        [](Symbol* symbol) -> Subscript::IndexExpression {return symbol;},
        [](auto*) -> Subscript::IndexExpression {SHOULD_NOT_HAPPEN();},
    }, expr);
}

///////////////////////////////////////////////////////////

Subscript::Index::Index(const IndexExpression& nth)
        : nth(nth){}

Subscript::Range::Range(const IndexExpression& from, const IndexExpression& to, bool exclusive)
        : from(from), to(to), exclusive(exclusive){}

Subscript::Subscript(const Expression& array, const Argument& argument, Suffix suffix)
        : array(array), argument(argument), suffix(suffix){}

MayFail_<Subscript>::MayFail_(const MayFail<Expression_>& array, const Argument& argument, Suffix suffix)
        : array(array), argument(argument), suffix(suffix){}

MayFail_<Subscript>::MayFail_(Subscript subscript) {
    auto array = wrap_expr(subscript.array);
    auto argument = std::visit(overload{
        [](Subscript::Index index) -> Argument {return index;},
        [](Subscript::Range range) -> Argument {return range;},
        [](Subscript::Key key) -> Argument {return Key{wrap_expr(key.expr)};},
    }, subscript.argument);
    auto suffix = subscript.suffix;

    this->array = array;
    this->argument = argument;
    this->suffix = suffix;
    this->_lvalue = subscript._lvalue;
    this->_tokenLen = subscript._tokenLen;
    this->_tokenId = subscript._tokenId;
}

MayFail_<Subscript>::operator Subscript() const {
    auto array = unwrap_expr(this->array.value());
    auto argument = std::visit(overload{
        [](_StubArgument_) -> Subscript::Argument {SHOULD_NOT_HAPPEN();},
        [](Index index) -> Subscript::Argument {return index;},
        [](Range range) -> Subscript::Argument {return range;},
        [](Key key) -> Subscript::Argument {return Subscript::Key{unwrap_expr(key.expr.value())};},
    }, this->argument);
    auto suffix = this->suffix;

    auto subscript = Subscript{array, argument, suffix};
    subscript._lvalue = this->_lvalue;
    subscript._tokenLen = this->_tokenLen;
    subscript._tokenId = this->_tokenId;
    return subscript;
}
