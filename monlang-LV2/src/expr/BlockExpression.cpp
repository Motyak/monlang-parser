#include <monlang-LV2/expr/BlockExpression.h>

/* impl only */

/* require knowing all words for token_len() */
#include <monlang-LV2/token_len.h>

#include <monlang-LV1/ast/CurlyBracketsGroup.h>

#include <utils/assert-utils.h>
#include <utils/loop-utils.h>

#define until(x) while(!(x))

bool peekBlockExpression(const Word& word) {
    // don't we need to differenciate different types of "block" ? will see
    return std::holds_alternative<CurlyBracketsGroup*>(word);
}

MayFail<MayFail_<BlockExpression>> buildBlockExpression(const Word& word) {
    ASSERT (std::holds_alternative<CurlyBracketsGroup*>(word));
    auto cbg = *std::get<CurlyBracketsGroup*>(word);

    auto statements = std::vector<MayFail<Statement_>>();
    LOOP until (cbg.sentences.empty()) {
        auto statement = consumeStatement((Subprogram&)cbg);
        // handle oneline block expr, to adapt token_len accordingly
        if (__first_it && cbg.term && std::holds_alternative<MayFail_<ExpressionStatement>*>(statement.val)) {
            auto exprStmt = std::get<MayFail_<ExpressionStatement>*>(statement.val);
            if (exprStmt->expression) {
                set_token_len(exprStmt->expression->val, token_len(exprStmt->expression->val) + 1);
            }
        }
        statements.push_back(statement);
        if (statement.has_error()) {
            auto malformed = Malformed(MayFail_<BlockExpression>{statements}, ERR(641));
            malformed.val._dollars = cbg._dollars;
            if (cbg.term) {
                malformed.val._oneline = true;
            }
            return malformed;
        }
        ENDLOOP
    }

    auto blockExpr = MayFail_<BlockExpression>{statements};
    blockExpr._tokenLen = cbg._tokenLen;
    blockExpr._dollars = cbg._dollars;
    if (cbg.term || statements.empty()) {
        blockExpr._oneline = true;
    }
    return blockExpr;
}

BlockExpression::BlockExpression(const std::vector<Statement>& statements) : statements(statements){}

MayFail_<BlockExpression>::MayFail_(_dummy_stub) : _stub(true){}

MayFail_<BlockExpression>::MayFail_(const std::vector<MayFail<Statement_>>& statements) : statements(statements){}

MayFail_<BlockExpression>::MayFail_(BlockExpression blockExpr) {
    auto statements = std::vector<MayFail<Statement_>>();
    for (auto e: blockExpr.statements) {
        statements.push_back(wrap_stmt(e));
    }
    this->statements = statements;
    this->_tokenLen = blockExpr._tokenLen;
    this->_tokenId = blockExpr._tokenId;
    this->_dollars = blockExpr._dollars;
}

MayFail_<BlockExpression>::operator BlockExpression() const {
    auto statements = std::vector<Statement>();
    for (auto e: this->statements) {
        statements.push_back(unwrap_stmt(e.value()));
    }
    auto blockExpr = BlockExpression{statements};
    blockExpr._tokenLen = this->_tokenLen;
    blockExpr._tokenId = this->_tokenId;
    blockExpr._dollars = this->_dollars;
    return blockExpr;
}
