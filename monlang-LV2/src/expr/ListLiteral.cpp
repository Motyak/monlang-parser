#include <monlang-LV2/expr/ListLiteral.h>

/* impl only */

#include <monlang-LV1/ast/SquareBracketsGroup.h>

#include <utils/assert-utils.h>

MayFail<MayFail_<ListLiteral>> buildListLiteral(const Word& word) {
    ASSERT (std::holds_alternative<SquareBracketsGroup*>(word));
    auto sbg = *std::get<SquareBracketsGroup*>(word);

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
    this->_tokenLen = listLiteral._tokenLen;
    this->_tokenId = listLiteral._tokenId;
}

MayFail_<ListLiteral>::operator ListLiteral() const {
    auto arguments = std::vector<Expression>();
    for (auto arg: this->arguments) {
        arguments.push_back(unwrap_expr(arg.value()));
    }
    auto listLiteral = ListLiteral{arguments};
    listLiteral._tokenLen = this->_tokenLen;
    listLiteral._tokenId = this->_tokenId;
    return listLiteral;
}
