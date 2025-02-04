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

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/Association.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>

#include <utils/assert-utils.h>
#include <utils/loop-utils.h>
#include <utils/variant-utils.h>

#define token tokens._vec.at(tokenId)

// should only be used to check for Statement_()
static bool is_stub(const Statement_& stmt) {
    return std::visit(
        [](auto* ptr){return ptr == nullptr;},
        stmt
    );
}

// should only be used to check for StubExpression_()
static bool is_stub(const Expression_& expr) {
    return std::visit(overload{
        [](_StubExpression_*){return true;},
        [](auto*){return false;},
    }, expr);
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

        curPos += group_nesting(expr.val);

        token.start = asTokenPosition(curPos);
        token.end = asTokenPosition(curPos - !!curPos);

        if (token.is_malformed) {
            token.err_start = token.start;
            if (expr.err->_info.contains("err_offset")) {
                auto err_offset = std::any_cast<size_t>(expr.err->_info.at("err_offset"));
                token.err_start = asTokenPosition(token.err_start + err_offset);
            }
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

void ReconstructLV2Tokens::operator()(MayFail_<Assignment>* assign) {
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "Assignment";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    curPos += assign->_tokenLeadingNewlines;
    curPos += assign->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(/*const*/&assign->variable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += Assignment::SEPARATOR._tokenLen;
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(assign->value);
    curPos = backupCurPos;
    curPos += assign->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += assign->_tokenTrailingNewlines;

    if (token.is_malformed) {
        token.err_start = token.start;
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<Accumulation>* acc) {
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "Accumulation";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    curPos += acc->_tokenLeadingNewlines;
    curPos += acc->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(/*const*/&acc->variable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += acc->SEPARATOR()._tokenLen;
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(acc->value);
    curPos = backupCurPos;
    curPos += acc->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += acc->_tokenTrailingNewlines;

    if (token.is_malformed) {
        token.err_start = token.start;
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<LetStatement>* letStmt) {
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "LetStatement";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    curPos += letStmt->_tokenLeadingNewlines;
    curPos += letStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += LetStatement::KEYWORD._tokenLen;
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += letStmt->identifier.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(letStmt->value);
    curPos = backupCurPos;
    curPos += letStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += letStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        token.err_start = token.start;
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<VarStatement>* varStmt) {
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "VarStatement";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    curPos += varStmt->_tokenLeadingNewlines;
    curPos += varStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += LetStatement::KEYWORD._tokenLen;
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += varStmt->identifier.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(varStmt->value);
    curPos = backupCurPos;
    curPos += varStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += varStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        token.err_start = token.start;
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<ReturnStatement>* returnStmt) {
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "ReturnStatement";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    curPos += returnStmt->_tokenLeadingNewlines;
    curPos += returnStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += ReturnStatement::KEYWORD._tokenLen;
    if (returnStmt->value) {
        curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        operator()(*returnStmt->value);
    }
    curPos = backupCurPos;
    curPos += returnStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += returnStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        token.err_start = token.start;
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(BreakStatement* breakStmt) {
    /* NOTE: BreakStatement cannot be malformed */
    ASSERT (!curStmt.has_error());

    auto tokenId = newToken(curStmt);
    token.is_malformed = false;
    token.name = "BreakStatement";

    curPos += breakStmt->_tokenLeadingNewlines;
    curPos += breakStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    // lastCorrectToken = -1;
    curPos += breakStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += breakStmt->_tokenTrailingNewlines;
}

void ReconstructLV2Tokens::operator()(ContinueStatement* continueStmt) {
    /* NOTE: ContinueStatement cannot be malformed */
    ASSERT (!curStmt.has_error());

    auto tokenId = newToken(curStmt);
    token.is_malformed = false;
    token.name = "ContinueStatement";

    curPos += continueStmt->_tokenLeadingNewlines;
    curPos += continueStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    // lastCorrectToken = -1;
    curPos += continueStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += continueStmt->_tokenTrailingNewlines;
}

void ReconstructLV2Tokens::operator()(DieStatement* dieStmt) {
    /* NOTE: DieStatement cannot be malformed */
    ASSERT (!curStmt.has_error());

    auto tokenId = newToken(curStmt);
    token.is_malformed = false;
    token.name = "DieStatement";

    curPos += dieStmt->_tokenLeadingNewlines;
    curPos += dieStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    // lastCorrectToken = -1;
    curPos += dieStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += dieStmt->_tokenTrailingNewlines;
}

void ReconstructLV2Tokens::operator()(MayFail_<ForeachStatement>* foreachStmt) {
    // NOTE: will require lastCorrectToken
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "ForeachStatement";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    curPos += foreachStmt->_tokenLeadingNewlines;
    curPos += foreachStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += ForeachStatement::KEYWORD._tokenLen;
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(foreachStmt->iterable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(mayfail_convert<Expression_>(foreachStmt->block));
    curPos = backupCurPos;
    curPos += foreachStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += foreachStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<WhileStatement>* whileStmt) {
    // NOTE: will require lastCorrectToken
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "WhileStatement";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    curPos += whileStmt->_tokenLeadingNewlines;
    curPos += whileStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    if (whileStmt->until_loop) {
        curPos += WhileStatement::UNTIL_KEYWORD._tokenLen;
    }
    else {
        curPos += WhileStatement::WHILE_KEYWORD._tokenLen;
    }
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += sequenceLen(SquareBracketsTerm::INITIATOR_SEQUENCE);
    operator()(whileStmt->condition);
    curPos += sequenceLen(SquareBracketsTerm::TERMINATOR_SEQUENCE);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(mayfail_convert<Expression_>(whileStmt->block));
    curPos = backupCurPos;
    curPos += whileStmt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += whileStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<DoWhileStatement>*) {
    // NOTE: will require lastCorrectToken
    TODO();
}

void ReconstructLV2Tokens::operator()(MayFail_<ExpressionStatement>* exprStmt) {
    auto tokenId = newToken(curStmt);
    token.is_malformed = curStmt.has_error();
    token.name = "ExpressionStatement";

    if (token.is_malformed) {
        token.err_desc = curStmt.error().fmt; // TODO: map this to the actual error description
    }

    curPos += exprStmt->_tokenLeadingNewlines;
    curPos += exprStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    operator()(exprStmt->expression);
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += exprStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

///////////////////////////////////////////////////////////////
// EXPRESSIONS
///////////////////////////////////////////////////////////////

void ReconstructLV2Tokens::operator()(MayFail_<Operation>* operation) {
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "Operation";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    curPos += group_nesting(*operation);

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(operation->leftOperand);
    curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
    curPos += operation->operator_.size();
    curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
    operator()(operation->rightOperand);
    curPos = backupCurPos;
    curPos += operation->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<FunctionCall>* functionCall) {
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "FunctionCall";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    curPos += group_nesting(*functionCall);

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(functionCall->function);
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto expr: functionCall->arguments) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        operator()(expr);
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += functionCall->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<Lambda>* lambda) {
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "Lambda";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    curPos += group_nesting(*lambda);

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto param: lambda->parameters) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        curPos += param.size();
        ENDLOOP
    }
    curPos += sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE);
    curPos += sequenceLen(Association::SEPARATOR_SEQUENCE);
    operator()(&lambda->body);
    curPos = backupCurPos;
    curPos += lambda->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<BlockExpression>* blockExpr) {
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "BlockExpression";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    curPos += group_nesting(*blockExpr);

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(CurlyBracketsGroup::INITIATOR_SEQUENCE);
    if (!blockExpr->_oneline) {
        curPos += 1; // newline
    }
    for (auto stmt: blockExpr->statements) {
        operator()(stmt);
    }
    curPos = backupCurPos;
    curPos += blockExpr->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(Literal* literal) {
    /* NOTE: Literal cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken(curExpr);
    token.is_malformed = false;
    token.name = "Literal";

    curPos += group_nesting(*literal);

    token.start = asTokenPosition(curPos);
    curPos += literal->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);
}

void ReconstructLV2Tokens::operator()(SpecialSymbol* specialSymbol) {
    /* NOTE: SpecialSymbol cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken(curExpr);
    token.is_malformed = false;
    token.name = "SpecialSymbol";

    curPos += group_nesting(*specialSymbol);

    token.start = asTokenPosition(curPos);
    curPos += specialSymbol->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);
}

void ReconstructLV2Tokens::operator()(Lvalue* lvalue) {
    /* NOTE: Lvalue cannot be malformed */
    // ASSERT (!curExpr.has_error()); // doesn't necessarily mean parent is an Expression

    auto tokenId = newToken(curExpr);
    token.is_malformed = false;
    token.name = "Lvalue";

    curPos += group_nesting(*lvalue);

    token.start = asTokenPosition(curPos);
    curPos += lvalue->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);
}

void ReconstructLV2Tokens::operator()(_StubExpression_*) {
    SHOULD_NOT_HAPPEN();
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
