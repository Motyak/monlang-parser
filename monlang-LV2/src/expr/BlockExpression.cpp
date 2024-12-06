#include <monlang-LV2/expr/BlockExpression.h>

/* impl only */

#include <monlang-LV1/ast/CurlyBracketsGroup.h>

#include <utils/assert-utils.h>

#define until(x) while(!(x))

bool peekBlockExpression(const Word& word) {
    // don't we need to differenciate different types of "block" ? will see
    return std::holds_alternative<CurlyBracketsGroup*>(word);
}

MayFail<MayFail_<BlockExpression>> buildBlockExpression(const Word& word) {
    ASSERT (std::holds_alternative<CurlyBracketsGroup*>(word));
    auto cbg = *std::get<CurlyBracketsGroup*>(word);

    auto statements = std::vector<MayFail<Statement_>>();
    until (cbg.sentences.empty()) {
        auto statement = consumeStatement((Subprogram&)cbg);
        statements.push_back(statement);
        if (statement.has_error()) {
            return Malformed(MayFail_<BlockExpression>{statements}, ERR(641));
        }
    }

    return MayFail_<BlockExpression>{statements};
}

MayFail_<BlockExpression>::MayFail_(std::vector<MayFail<Statement_>> statements) : statements(statements){}

MayFail_<BlockExpression>::MayFail_(BlockExpression blockExpr) {
    auto statements = std::vector<MayFail<Statement_>>();
    for (auto e: blockExpr.statements) {
        statements.push_back(wrap_stmt(e));
    }
    this->statements = statements;
}

MayFail_<BlockExpression>::operator BlockExpression() const {
    auto statements = std::vector<Statement>();
    for (auto e: this->statements) {
        statements.push_back(unwrap_stmt(e.value()));
    }
    return BlockExpression{statements};
}

template <>
BlockExpression unwrap(const MayFail_<BlockExpression>& blockExpr) {
    return (BlockExpression)blockExpr;
}

template <>
MayFail_<BlockExpression> wrap(const BlockExpression& blockExpr) {
    return MayFail_<BlockExpression>(blockExpr);
}
