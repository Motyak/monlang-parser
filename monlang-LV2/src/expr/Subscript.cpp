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

MayFail<MayFail_<Subscript>> buildSubscript(const Word& word) {
    ASSERT (std::holds_alternative<PostfixSquareBracketsGroup*>(word));
    auto psbg = *std::get<PostfixSquareBracketsGroup*>(word);
    auto array = buildExpression((Term)variant_cast(psbg.leftPart));
    if (array.has_error()) {
        return Malformed(MayFail_<Subscript>{array, /*_StubArgument_*/{}}, ERR(721));
    }

    ASSERT (psbg.rightPart.terms.size() == 1);

    /* breakable block */ for (int i = 1; i <= 1;)
    {
        unless (psbg.rightPart.terms[0].words.size() == 1) break;
        unless (std::holds_alternative<Atom*>(psbg.rightPart.terms[0].words[0])) break;
        auto str = std::get<Atom*>(psbg.rightPart.terms[0].words[0])->value;
        ASSERT (str.size() > 0);
        unless (str[0] == '#') break;
        str = split_in_two(str, "#").at(1);
        if (split_in_two(str, "..<").size() == 2) {
            auto atom_from = Atom{split_in_two(str, "..<").at(0)};
            auto atom_to = Atom{split_in_two(str, "..<").at(1)};
            auto from = unwrap_expr(buildExpression((Term)move_to_heap(atom_from)).value());
            auto to = unwrap_expr(buildExpression((Term)move_to_heap(atom_to)).value());
            auto range = MayFail_<Subscript>::Range{from, to, /*exclusive*/true};
            auto subscript = MayFail_<Subscript>{array, range};
            subscript._tokenLen = psbg._tokenLen;
            return subscript;
        }
        if (split_in_two(str, "..").size() == 2) {
            auto atom_from = Atom{split_in_two(str, "..").at(0)};
            auto atom_to = Atom{split_in_two(str, "..").at(1)};
            auto from = unwrap_expr(buildExpression((Term)move_to_heap(atom_from)).value());
            auto to = unwrap_expr(buildExpression((Term)move_to_heap(atom_to)).value());
            auto range = MayFail_<Subscript>::Range{from, to};
            auto subscript = MayFail_<Subscript>{array, range};
            subscript._tokenLen = psbg._tokenLen;
            return subscript;
        }
        auto atom_index = Atom{str};
        auto expr = unwrap_expr(buildExpression((Term)move_to_heap(atom_index)).value());
        auto index = MayFail_<Subscript>::Index{expr};
        auto subscript = MayFail_<Subscript>{array, index};
        subscript._tokenLen = psbg._tokenLen;
        return subscript;
    }

    auto expr = buildExpression(psbg.rightPart.terms[0]);
    auto key = MayFail_<Subscript>::Key{expr};
    if (expr.has_error()) {
        return Malformed(MayFail_<Subscript>{array, key}, ERR(722));
    }

    auto subscript = MayFail_<Subscript>{array, key};
    subscript._tokenLen = psbg._tokenLen;
    return subscript;
}

Subscript::Subscript(const Expression& array, const Argument& argument)
        : array(array), argument(argument){}

MayFail_<Subscript>::MayFail_(const MayFail<Expression_>& array, const Argument& argument)
        : array(array), argument(argument){}

MayFail_<Subscript>::MayFail_(Subscript subscript) {
    auto array = wrap_expr(subscript.array);
    auto argument = std::visit(overload{
        [](Subscript::Index index) -> Argument {return index;},
        [](Subscript::Range range) -> Argument {return range;},
        [](Subscript::Key key) -> Argument {return Key{wrap_expr(key.expr)};},
    }, subscript.argument);

    this->array = array;
    this->argument = argument;
    this->_tokenLen = subscript._tokenLen;
}

MayFail_<Subscript>::operator Subscript() const {
    auto array = unwrap_expr(this->array.value());
    Subscript::Argument argument = std::visit(overload{
        [](_StubArgument_) -> Subscript::Argument {SHOULD_NOT_HAPPEN();},
        [](Index index) -> Subscript::Argument {return index;},
        [](Range range) -> Subscript::Argument {return range;},
        [](Key key) -> Subscript::Argument {return Subscript::Key{unwrap_expr(key.expr.value())};},
    }, this->argument);
    auto subscript = Subscript{array, argument};
    subscript._tokenLen = this->_tokenLen;
    return subscript;
}
