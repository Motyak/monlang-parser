#include <monlang-LV2/expr/MapLiteral.h>

/* impl only */

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>
#include <utils/loop-utils.h>

#define unless(x) if(!(x))

bool peekMapLiteral(const Word& word) {

    if (std::holds_alternative<Atom*>(word) && std::get<Atom*>(word)->value == "[:]") {
        return true; // special case
    }

    unless (std::holds_alternative<SquareBracketsGroup*>(word)) {
        return false;
    }

    auto sbg = *std::get<SquareBracketsGroup*>(word);
    unless (!sbg.terms.empty()) {
        return false;
    }

    auto term = sbg.terms.at(0);
    ASSERT (term.words.size() > 0);
    unless (term.words.size() < 2) {
        return false;
    }

    auto term_word = term.words.at(0);
    return std::holds_alternative<Association*>(term_word);
}

MayFail<MayFail_<MapLiteral>> buildMapLiteral(const Word& word) {
    // handle empty map literal
    if (std::holds_alternative<Atom*>(word) && std::get<Atom*>(word)->value == "[:]") {
        return MayFail_<MapLiteral>{};
    }

    ASSERT (std::holds_alternative<SquareBracketsGroup*>(word));
    auto sbg = *std::get<SquareBracketsGroup*>(word);
    auto arguments = std::vector<MayFail_<MapLiteral>::Argument>();

    LOOP for (auto term: sbg.terms) {
        ASSERT (term.words.size() == 1);

        auto word = term.words.at(0);
        if (__first_it) {
            ASSERT (std::holds_alternative<Association*>(word));
        }
        else unless (std::holds_alternative<Association*>(word)) {
            return Malformed(MayFail_<MapLiteral>{arguments}, ERR(691));
        }

        auto assoc = *std::get<Association*>(word);

        auto key = buildExpression((Term)variant_cast(assoc.leftPart));
        if (key.has_error()) {
            arguments.push_back({key, StubExpression_()});
            return Malformed(MayFail_<MapLiteral>{arguments}, ERR(692));
        }

        auto val = buildExpression((Term)assoc.rightPart);
        if (val.has_error()) {
            arguments.push_back({key, val});
            return Malformed(MayFail_<MapLiteral>{arguments}, ERR(693));
        }

        arguments.push_back({key, val});

        ENDLOOP
    }

    auto mapLiteral = MayFail_<MapLiteral>{arguments};
    mapLiteral._tokenLen = sbg._tokenLen;
    return mapLiteral;
}

MapLiteral::MapLiteral(const std::vector<Argument>& arguments)
        : arguments(arguments){}

MayFail_<MapLiteral>::MayFail_(const std::vector<Argument>& arguments)
        : arguments(arguments){}

MayFail_<MapLiteral>::MayFail_(MapLiteral mapLiteral) {
    auto arguments = std::vector<Argument>();
    for (auto [key, val]: mapLiteral.arguments) {
        arguments.push_back({wrap_expr(key), wrap_expr(val)});
    }
    this->arguments = arguments;
    this->_tokenLen = mapLiteral._tokenLen;
}

MayFail_<MapLiteral>::operator MapLiteral() const {
    auto arguments = std::vector<MapLiteral::Argument>();
    for (auto [key, val]: this->arguments) {
        arguments.push_back({unwrap_expr(key.value()), unwrap_expr(val.value())});
    }
    auto mapLiteral = MapLiteral{arguments};
    mapLiteral._tokenLen = this->_tokenLen;
    return mapLiteral;
}
