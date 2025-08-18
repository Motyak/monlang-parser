#include <monlang-LV2/expr/ListLiteral.h>

/* impl only */

#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/MultilineSquareBracketsGroup.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

#define unless(x) if(!(x))

#define SET_NTH_SENTENCE_ERR_OFFSET(error, nth) \
    auto err_offset = size_t(0); \
    err_offset += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE); \
    err_offset += 1; /*newline*/ \
    size_t i = 0; \
    for (; i < __nth_it - 1; ++i) { \
        err_offset += msbg.sentences.at(i)._tokenLeadingNewlines; \
        err_offset += msbg.sentences.at(i)._tokenIndentSpaces; \
        err_offset += msbg.sentences.at(i)._tokenLen; \
    } \
    err_offset += msbg.sentences.at(i)._tokenLeadingNewlines; \
    err_offset += msbg.sentences.at(i)._tokenIndentSpaces; \
    error._info["err_offset"] = err_offset

static MayFail<MayFail_<ListLiteral>> buildOnelineListLiteral(const SquareBracketsGroup&);
static MayFail<MayFail_<ListLiteral>> buildMultilineListLiteral(const MultilineSquareBracketsGroup&);

MayFail<MayFail_<ListLiteral>> buildListLiteral(const Word& word) {
    return std::visit(overload{
        [](SquareBracketsGroup* sbg) -> MayFail<MayFail_<ListLiteral>> {
            return buildOnelineListLiteral(*sbg);
        },
        [](MultilineSquareBracketsGroup* msbg) -> MayFail<MayFail_<ListLiteral>> {
            return buildMultilineListLiteral(*msbg);
        },
        [](auto*) -> MayFail<MayFail_<ListLiteral>> {
            SHOULD_NOT_HAPPEN();
        },
    }, word);
}

static MayFail<MayFail_<ListLiteral>> buildOnelineListLiteral(const SquareBracketsGroup& sbg) {
    auto arguments = std::vector<MayFail<Expression_>>();

    for (auto term: sbg.terms) {
        auto expr = buildExpression(term);
        arguments.push_back(expr);
        if (expr.has_error()) {
            return Malformed(MayFail_<ListLiteral>{arguments}, ERR(681));
        }
    }

    auto listLiteral = MayFail_<ListLiteral>{arguments};
    listLiteral._tokenLen = sbg._tokenLen;
    return listLiteral;
}

static MayFail<MayFail_<ListLiteral>> buildMultilineListLiteral(const MultilineSquareBracketsGroup& msbg) {
    auto arguments = std::vector<MayFail<Expression_>>();

    size_t __nth_it = 1;
    for (auto sentence: msbg.sentences) {
        for (auto pw: sentence.programWords) {
            unless (holds_word(pw)) {
                auto error = ERR(682);
                SET_NTH_SENTENCE_ERR_OFFSET(error, __nth_it);
                auto malformed = Malformed(MayFail_<ListLiteral>{arguments}, error);
                malformed.val._msbg = msbg;
                return malformed;
            }
        }
        auto term = (Term)sentence;
        term._tokenLen -= 1;
        auto expr = buildExpression(term);
        arguments.push_back(expr);
        if (expr.has_error()) {
            auto error = ERR(683);
            SET_NTH_SENTENCE_ERR_OFFSET(error, __nth_it);
            auto malformed = Malformed(MayFail_<ListLiteral>{arguments}, error);
            malformed.val._msbg = msbg;
            return malformed;
        }
        __nth_it += 1;
    }

    auto listLiteral = MayFail_<ListLiteral>{arguments};
    listLiteral._msbg = msbg;
    listLiteral._tokenLen = msbg._tokenLen;
    return listLiteral;
}

ListLiteral::ListLiteral(const std::vector<Expression>& arguments)
        : arguments(arguments){}

MayFail_<ListLiteral>::MayFail_(const std::vector<MayFail<Expression_>>& arguments)
        : arguments(arguments){}

MayFail_<ListLiteral>::MayFail_(ListLiteral listLiteral) {
    auto arguments = std::vector<MayFail<Expression_>>();
    for (auto arg: listLiteral.arguments) {
        arguments.push_back(wrap_expr(arg));
    }
    this->arguments = arguments;
    this->_msbg = _msbg;
    this->_tokenLen = listLiteral._tokenLen;
    this->_tokenId = listLiteral._tokenId;
}

MayFail_<ListLiteral>::operator ListLiteral() const {
    auto arguments = std::vector<Expression>();
    for (auto arg: this->arguments) {
        arguments.push_back(unwrap_expr(arg.value()));
    }
    auto listLiteral = ListLiteral{arguments};
    listLiteral._msbg = this->_msbg;
    listLiteral._tokenLen = this->_tokenLen;
    listLiteral._tokenId = this->_tokenId;
    return listLiteral;
}
