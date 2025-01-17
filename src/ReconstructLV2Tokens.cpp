#include <monlang-parser/ReconstructLV2Tokens.h>

#include <monlang-LV2/Program.h>
#include <monlang-LV2/Statement.h>
#include <monlang-LV2/Expression.h>

/* in impl only */

#include <monlang-LV2/stmt/Assignment.h>
#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/stmt/LetStatement.h>
#include <monlang-LV2/stmt/VarStatement.h>
#include <monlang-LV2/stmt/ReturnStatement.h>
#include <monlang-LV2/stmt/BreakStatement.h>
#include <monlang-LV2/stmt/ContinueStatement.h>
#include <monlang-LV2/stmt/DieStatement.h>
#include <monlang-LV2/stmt/ForeachStatement.h>
#include <monlang-LV2/stmt/WhileStatement.h>
#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <monlang-LV2/expr/Operation.h>
#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/expr/Lambda.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/expr/SpecialSymbol.h>
#include <monlang-LV2/expr/Literal.h>
#include <monlang-LV2/expr/Lvalue.h>

#include <utils/assert-utils.h>

#define token tokens._vec.at(tokenId)

// should only be used to check for Statement_()
static bool is_stub(const Statement_& stmt) {
    return std::visit(
        [](auto* ptr){return ptr == nullptr;},
        stmt
    );
}

// should only be used to check for Expression_()
static bool is_stub(const Expression_& expr) {
    return std::visit(
        [](auto* ptr){return ptr == nullptr;},
        expr
    );
}

void ReconstructLV2Tokens::operator()(const MayFail<MayFail_<LV2::Program>>& prog) {
    /* reset state */
    tokens = {};
    curPos = 0;
    // lastCorrectToken = -1;

    auto tokenId = newToken(prog);
    token.is_malformed = prog.has_error();
    token.name = "Program";

    if (token.is_malformed) {
        token.err_desc = prog.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupLastCorrectToken = lastCorrectToken;
    for (auto stmt: prog.val.statements) {
        auto stmtTokenId = tokens._vec.size();
        operator()(stmt);
        if (!tokens._vec.at(stmtTokenId).is_malformed) {
            lastCorrectToken = stmtTokenId;
        }
    }
    // special case, no _tokenLen
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(const MayFail<Statement_>& stmt) {
    if (is_stub(stmt.val)) {
        auto tokenId = newToken(stmt);
        token.is_malformed = stmt.has_error();
        token.name = "Statement";

        if (token.is_malformed) {
            token.err_desc = stmt.error().fmt; // TODO: map this to the actual error description
        }

        token.start = asTokenPosition(curPos);
        token.end = asTokenPosition(curPos - !!curPos);

        if (token.is_malformed) {
            token.err_start = token.start;
            tokens.traceback.push_back(token);
        }

        return;
    }

    curStmt = stmt; // needed by stmt handlers
    std::visit(*this, stmt.val);
}

void ReconstructLV2Tokens::operator()(const MayFail<Expression_>& expr) {
    if (is_stub(expr.val)) {
        auto tokenId = newToken(expr);
        token.is_malformed = expr.has_error();
        token.name = "Expression";

        if (token.is_malformed) {
            token.err_desc = expr.error().fmt; // TODO: map this to the actual error description
        }

        token.start = asTokenPosition(curPos);
        token.end = asTokenPosition(curPos - !!curPos);

        if (token.is_malformed) {
            token.err_start = token.start; /* TODO: fix this:
                                               <stdin>:1:1: LV2 error: Malformed Expression
                                                   1 | a + b $ c
                                                     | ^ERR-163
                                           */
            tokens.traceback.push_back(token);
        }

        return;
    }

    curExpr = expr; // needed by expr handlers
    std::visit(*this, expr.val);
}

///////////////////////////////////////////////////////////////
// STATEMENTS
///////////////////////////////////////////////////////////////

void ReconstructLV2Tokens::operator()(MayFail_<Assignment>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<Accumulation>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<LetStatement>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<VarStatement>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<ReturnStatement>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(BreakStatement*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(ContinueStatement*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(DieStatement*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<ForeachStatement>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<WhileStatement>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<DoWhileStatement>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<ExpressionStatement>* exprStmt) {
    operator()(exprStmt->expression);
}

///////////////////////////////////////////////////////////////
// EXPRESSIONS
///////////////////////////////////////////////////////////////

void ReconstructLV2Tokens::operator()(MayFail_<Operation>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<FunctionCall>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<Lambda>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<BlockExpression>*) {
    TODO();
}

void ReconstructLV2Tokens::operator()(Literal* literal) {
    /* NOTE: normally Literal cannot be malformed ? */

    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "Literal";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    curPos += literal->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV2Tokens::operator()(SpecialSymbol* ss) {
    /* NOTE: normally SpecialSymbol cannot be malformed ? */

    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "SpecialSymbol";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    curPos += ss->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV2Tokens::operator()(Lvalue* lvalue) {
    /* NOTE: normally Lvalue cannot be malformed ? */

    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "Lvalue";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    curPos += lvalue->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

///////////////////////////////////////////////////////////////

ReconstructLV2Tokens::ReconstructLV2Tokens(LV2Tokens& tokens, const std::vector<size_t>& newlinesPos)
        : tokens(tokens), newlinesPos(newlinesPos){}

TokenId ReconstructLV2Tokens::newToken(const LV2::Ast_& entity) {
    tokens._vec.push_back(Token{});
    return tokens._map[entity] = tokens._vec.size() - 1;
}

TokenPosition ReconstructLV2Tokens::asTokenPosition(size_t index) {
    if (newlinesPos.empty()) {
        return TokenPosition{index, 1, index + 1};
    }
    auto lower_bound = std::lower_bound(newlinesPos.begin(), newlinesPos.end(), index);
    size_t line = lower_bound - newlinesPos.begin() + 1;
    size_t column = lower_bound == newlinesPos.begin()? index + 1 : index - *(lower_bound - 1);

    return TokenPosition{index, line, column};
}
