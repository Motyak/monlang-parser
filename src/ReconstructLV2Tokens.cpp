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
#include <monlang-LV2/expr/FieldAccess.h>
#include <monlang-LV2/expr/Subscript.h>
#include <monlang-LV2/expr/MapLiteral.h>
#include <monlang-LV2/expr/ListLiteral.h>
#include <monlang-LV2/expr/SpecialSymbol.h>
#include <monlang-LV2/expr/Numeral.h>
#include <monlang-LV2/expr/StrLiteral.h>
#include <monlang-LV2/Lvalue.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/Path.h>
#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>

#include <utils/assert-utils.h>
#include <utils/loop-utils.h>
#include <utils/variant-utils.h>

#define token tokens._vec.at(tokenId)

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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(const MayFail<Statement_>& stmt) {
    curStmt = stmt; // needed by stmt handlers
    std::visit(*this, stmt.val);
}

void ReconstructLV2Tokens::operator()(const MayFail<Expression_>& expr) {
    curExpr = expr; // needed by expr handlers
    std::visit(*this, expr.val);
}

void ReconstructLV2Tokens::operator()(const MayFail<Lvalue_>& lvalue) {
    if (lvalue.val._stub) {
        auto tokenId = newToken((Expression_)lvalue.val);
        token.is_malformed = lvalue.has_error();
        token.name = "Lvalue";

        if (token.is_malformed) {
            token.err_desc = lvalue.error().fmt; // TODO: map this to the actual error description
        }

        /* no group nesting because impossible in an Lvalue */

        token.start = asTokenPosition(curPos);
        token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

        if (token.is_malformed) {
            token.err_start = token.start;
            tokens.traceback.push_back(token);
        }

        return;
    }

    curExpr = mayfail_cast<Expression_>(lvalue); // needed by expr handlers
    std::visit(*this, lvalue.val.variant);
}

///////////////////////////////////////////////////////////////
// STATEMENTS
///////////////////////////////////////////////////////////////

void ReconstructLV2Tokens::operator()(_StubStatement_* stub) {
    auto tokenId = newToken(curStmt);
    token.is_malformed = curStmt.has_error();
    token.name = "Statement";

    if (token.is_malformed) {
        token.err_desc = curStmt.error().fmt; // TODO: map this to the actual error description
    }

    curPos += stub->_tokenLeadingNewlines;
    curPos += stub->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

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
    operator()(assign->variable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += Assignment::SEPARATOR._tokenLen;
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(assign->value);
    curPos = backupCurPos;
    curPos += assign->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    operator()(acc->variable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += acc->SEPARATOR()._tokenLen;
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(acc->value);
    curPos = backupCurPos;
    curPos += acc->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    curPos += letStmt->name.value.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(letStmt->value);
    curPos = backupCurPos;
    curPos += letStmt->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    curPos += varStmt->name.value.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(varStmt->value);
    curPos = backupCurPos;
    curPos += varStmt->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    curPos += foreachStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    curPos += whileStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<DoWhileStatement>* doWhileStmt) {
    // NOTE: will require lastCorrectToken
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken(curStmt_);
    token.is_malformed = curStmt_.has_error();
    token.name = "DoWhileStatement";

    if (token.is_malformed) {
        token.err_desc = curStmt_.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;

    /* C_DoStatement */
    {
        // NOTE: will require lastCorrectToken
        auto& doStmt = doWhileStmt->doStmt.val;

        auto tokenId = newToken();
        token.is_malformed = doWhileStmt->doStmt.has_error();
        token.name = "C_DoStatement";

        if (token.is_malformed) {
            token.err_desc = doWhileStmt->doStmt.error().fmt; // TODO: map this to the actual error description
        }

        curPos += doStmt._tokenLeadingNewlines;
        curPos += doStmt._tokenIndentSpaces;

        token.start = asTokenPosition(curPos);
        auto backupCurPos = curPos;
        auto backupLastCorrectToken = lastCorrectToken;
        // lastCorrectToken = -1;
        curPos += C_DoStatement::KEYWORD._tokenLen;
        curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        operator()(mayfail_convert<Expression_>(doStmt.block));
        curPos = backupCurPos;
        curPos += doStmt._tokenLen;
        token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

        curPos += doStmt._tokenTrailingNewlines;

        if (token.is_malformed) {
            if (lastCorrectToken == size_t(-1)) {
                token.err_start = token.start;
            }
            else {
                token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
                token.err_start = token.err_start < token.start? token.start : token.err_start;
            }
            if (doWhileStmt->doStmt.err->_info.contains("err_offset")) {
                auto err_offset = std::any_cast<size_t>(doWhileStmt->doStmt.err->_info.at("err_offset"));
                token.err_start = asTokenPosition(token.err_start + err_offset);
            }
            tokens.traceback.push_back(token);
        }

        lastCorrectToken = backupLastCorrectToken;
    }
    /* end of C_DoStatement */

    /* C_WhileStatement */
    if (!doWhileStmt->doStmt.has_error() && !doWhileStmt->whileStmt.val._stub) // NOTE: VERY SPECIAL CASE, COMPOUND STATEMENT
    {
        // NOTE: will require lastCorrectToken
        auto& whileStmt = doWhileStmt->whileStmt.val;

        auto tokenId = newToken();
        token.is_malformed = doWhileStmt->whileStmt.has_error();
        token.name = "C_WhileStatement";

        if (token.is_malformed) {
            token.err_desc = doWhileStmt->whileStmt.error().fmt; // TODO: map this to the actual error description
        }

        curPos += whileStmt._tokenLeadingNewlines;
        curPos += whileStmt._tokenIndentSpaces;

        token.start = asTokenPosition(curPos);
        auto backupCurPos = curPos;
        auto backupLastCorrectToken = lastCorrectToken;
        // lastCorrectToken = -1;
        if (whileStmt.until_loop) {
            curPos += C_WhileStatement::UNTIL_KEYWORD._tokenLen;
        }
        else {
            curPos += C_WhileStatement::WHILE_KEYWORD._tokenLen;
        }
        curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        curPos += sequenceLen(SquareBracketsTerm::INITIATOR_SEQUENCE);
        operator()(whileStmt.condition);
        curPos += sequenceLen(SquareBracketsTerm::TERMINATOR_SEQUENCE);
        curPos = backupCurPos;
        curPos += whileStmt._tokenLen;
        token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

        curPos += whileStmt._tokenTrailingNewlines;

        if (token.is_malformed) {
            token.err_start = token.start;
            if (doWhileStmt->whileStmt.err->_info.contains("err_offset")) {
                auto err_offset = std::any_cast<size_t>(doWhileStmt->whileStmt.err->_info.at("err_offset"));
                token.err_start = asTokenPosition(token.err_start + err_offset);
            }
            tokens.traceback.push_back(token);
        }

        lastCorrectToken = backupLastCorrectToken;
    }
    /* end of C_WhileStatement */

    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        if (curStmt_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curStmt_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
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
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    curPos += exprStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

///////////////////////////////////////////////////////////////
// EXPRESSIONS
///////////////////////////////////////////////////////////////

void ReconstructLV2Tokens::operator()(_StubExpression_*) {
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "Expression";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    curPos += group_nesting(curExpr.val);

    token.start = asTokenPosition(curPos);
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        token.err_start = token.start;
        if (curExpr.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curExpr.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV2Tokens::operator()(MayFail_<Operation>* operation) {
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "Operation";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*operation);
    operator()(operation->leftOperand);
    curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
    curPos += operation->operator_.value.size();
    curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
    operator()(operation->rightOperand);
    curPos = backupCurPos;
    curPos += operation->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*functionCall);
    operator()(functionCall->function);
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto arg: functionCall->arguments) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        /* FunctionCallArgument */
        {
            auto tokenId = newToken();
            token.is_malformed = arg.has_error();
            token.name = "FunctionCallArgument";

            if (token.is_malformed) {
                token.err_desc = arg.error().fmt; // TODO: map this to the actual error description
            }

            token.start = asTokenPosition(curPos);
            auto backupCurPos = curPos;
            auto backupLastCorrectToken = lastCorrectToken;
            // lastCorrectToken = -1;
            curPos += arg.val.passByRef;
            operator()(arg.val.expr);
            curPos = backupCurPos;
            curPos += arg.val._tokenLen;
            token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

            if (token.is_malformed) {
                if (lastCorrectToken == size_t(-1)) {
                    token.err_start = token.start;
                }
                else {
                    token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
                    token.err_start = token.err_start < token.start? token.start : token.err_start;
                }
                tokens.traceback.push_back(token);
            }

            lastCorrectToken = backupLastCorrectToken;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += functionCall->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<FieldAccess>* fieldAccess) {
    auto curExpr_ = curExpr;
    auto tokenId = newToken(curExpr_);
    token.is_malformed = curExpr_.has_error();
    token.name = "FieldAccess";

    if (token.is_malformed) {
        token.err_desc = curExpr_.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*fieldAccess);
    operator()(fieldAccess->object);
    curPos += sequenceLen(Path::SEPARATOR_SEQUENCE);
    operator()(&fieldAccess->field);
    curPos = backupCurPos;
    curPos += fieldAccess->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        if (curExpr_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curExpr_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<Subscript>* subscript) {
    auto curExpr_ = curExpr;
    auto tokenId = newToken(curExpr_);
    token.is_malformed = curExpr_.has_error();
    token.name = "Subscript";

    if (token.is_malformed) {
        token.err_desc = curExpr_.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*subscript);
    operator()(subscript->array);
    curPos += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE);
    std::visit(overload{
        [this](MayFail_<Subscript>::Key key) {operator()(key.expr);},
        [](auto){;}
    }, subscript->argument);
    curPos = backupCurPos;
    curPos += subscript->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*lambda);
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto param: lambda->parameters) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        curPos += param.value.size();
        ENDLOOP
    }
    curPos += sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE);
    curPos += sequenceLen(Association::SEPARATOR_SEQUENCE);
    operator()(&lambda->body);
    curPos = backupCurPos;
    curPos += lambda->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

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

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*blockExpr);
    if (blockExpr->_dollars) {
        curPos += 1; // $
    }
    curPos += sequenceLen(CurlyBracketsGroup::INITIATOR_SEQUENCE);
    if (!blockExpr->_oneline) {
        curPos += 1; // newline
    }
    for (auto stmt: blockExpr->statements) {
        operator()(stmt);
    }
    curPos = backupCurPos;
    curPos += blockExpr->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<MapLiteral>* mapLiteral) {
    auto curExpr_ = curExpr; // local copy
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "MapLiteral";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*mapLiteral);
    curPos += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE);
    LOOP for (auto [key, val]: mapLiteral->arguments) {
        if (!__first_it) {
            curPos += sequenceLen(SquareBracketsGroup::CONTINUATOR_SEQUENCE);
        }
        operator()(key);
        curPos += sequenceLen(Association::SEPARATOR_SEQUENCE);
        operator()(val);
        ENDLOOP
    }
    curPos += sequenceLen(SquareBracketsGroup::TERMINATOR_SEQUENCE);
    curPos = backupCurPos;
    curPos += mapLiteral->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        if (curExpr_.err->_info.contains("err_offset")) {
            auto err_offset = std::any_cast<size_t>(curExpr_.err->_info.at("err_offset"));
            token.err_start = asTokenPosition(token.err_start + err_offset);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<ListLiteral>* listLiteral) {
    auto tokenId = newToken(curExpr);
    token.is_malformed = curExpr.has_error();
    token.name = "ListLiteral";

    if (token.is_malformed) {
        token.err_desc = curExpr.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*listLiteral);
    curPos += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE);
    LOOP for (auto arg: listLiteral->arguments) {
        if (!__first_it) {
            curPos += sequenceLen(SquareBracketsGroup::CONTINUATOR_SEQUENCE);
        }
        operator()(arg);
        ENDLOOP
    }
    curPos += sequenceLen(SquareBracketsGroup::TERMINATOR_SEQUENCE);
    curPos = backupCurPos;
    curPos += listLiteral->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(Numeral* numeral) {
    /* NOTE: Numeral cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken(curExpr);
    token.is_malformed = false;
    token.name = "Numeral";

    token.start = asTokenPosition(curPos);
    curPos += numeral->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);
}

void ReconstructLV2Tokens::operator()(StrLiteral* strLiteral) {
    /* NOTE: StrLiteral cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken(curExpr);
    token.is_malformed = false;
    token.name = "StrLiteral";

    token.start = asTokenPosition(curPos);
    curPos += strLiteral->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);
}

void ReconstructLV2Tokens::operator()(SpecialSymbol* specialSymbol) {
    /* NOTE: SpecialSymbol cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken(curExpr);
    token.is_malformed = false;
    token.name = "SpecialSymbol";

    token.start = asTokenPosition(curPos);
    curPos += specialSymbol->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);
}

void ReconstructLV2Tokens::operator()(Symbol* symbol) {
    /* NOTE: Symbol cannot be malformed */
    // ASSERT (!curExpr.has_error()); // doesn't necessarily mean parent is an Expression

    auto tokenId = newToken(curExpr);
    token.is_malformed = false;
    token.name = "Symbol";

    token.start = asTokenPosition(curPos);
    curPos += symbol->_tokenLen;
    token.end = asTokenPosition(token.start == curPos? curPos : curPos - 1);
}

///////////////////////////////////////////////////////////////

ReconstructLV2Tokens::ReconstructLV2Tokens(LV2Tokens& tokens, const std::vector<size_t>& newlinesPos)
        : tokens(tokens), newlinesPos(newlinesPos){}

TokenId ReconstructLV2Tokens::newToken() {
    tokens._vec.push_back(Token{});
    return tokens._vec.size() - 1;
}

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
