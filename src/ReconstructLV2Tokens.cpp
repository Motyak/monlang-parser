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

#include <algorithm>
#include <numeric>

#define token tokens[tokenId]

void ReconstructLV2Tokens::operator()(MayFail<MayFail_<LV2::Program>>& prog) {
    /* reset state */
    tokens = {};
    curPos = 0;
    // lastCorrectToken = -1;

    auto tokenId = newToken();
    prog.val._tokenId = tokenId;
    token.is_malformed = prog.has_error();
    token.name = "Program";

    if (token.is_malformed) {
        token.err_fmt = prog.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupLastCorrectToken = lastCorrectToken;
    for (auto stmt: prog.val.statements) {
        auto stmtTokenId = tokens.size();
        operator()(stmt);
        if (!tokens[stmtTokenId].is_malformed) {
            lastCorrectToken = stmtTokenId;
        }
    }
    // special case, no _tokenLen
    token.end = asTokenPosition(curPos);

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
        auto tokenId = newToken();
        token.is_malformed = lvalue.has_error();
        token.name = "Lvalue";

        if (token.is_malformed) {
            token.err_fmt = lvalue.error().fmt; // TODO: we will need to fill token.err_desc as well
        }

        /* no group nesting because impossible in an Lvalue */

        token.start = asTokenPosition(curPos);
        token.end = asTokenPosition(curPos);

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
    auto tokenId = newToken();
    token.is_malformed = curStmt.has_error();
    token.name = "Statement";

    if (token.is_malformed) {
        token.err_fmt = curStmt.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += stub->_tokenLeadingNewlines;
    curPos += stub->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV2Tokens::operator()(MayFail_<Assignment>* assign) {
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken();
    assign->_tokenId = tokenId;
    token.is_malformed = curStmt_.has_error();
    token.name = "Assignment";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += assign->_tokenLeadingNewlines;
    curPos += assign->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(assign->variable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += Assignment::SEPARATOR.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(assign->value);
    curPos = backupCurPos;
    curPos += assign->_tokenLen;
    token.end = asTokenPosition(curPos);

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

    auto tokenId = newToken();
    acc->_tokenId = tokenId;
    token.is_malformed = curStmt_.has_error();
    token.name = "Accumulation";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += acc->_tokenLeadingNewlines;
    curPos += acc->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(acc->variable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(&acc->operator_);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(acc->value);
    curPos = backupCurPos;
    curPos += acc->_tokenLen;
    token.end = asTokenPosition(curPos);

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

    auto tokenId = newToken();
    letStmt->_tokenId = tokenId;
    token.is_malformed = curStmt_.has_error();
    token.name = "LetStatement";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += letStmt->_tokenLeadingNewlines;
    curPos += letStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += LetStatement::KEYWORD.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(&letStmt->label);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(letStmt->value);
    curPos = backupCurPos;
    curPos += letStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

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

    auto tokenId = newToken();
    varStmt->_tokenId = tokenId;
    token.is_malformed = curStmt_.has_error();
    token.name = "VarStatement";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += varStmt->_tokenLeadingNewlines;
    curPos += varStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += VarStatement::KEYWORD.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(&varStmt->variable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(varStmt->value);
    curPos = backupCurPos;
    curPos += varStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

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

    auto tokenId = newToken();
    returnStmt->_tokenId = tokenId;
    token.is_malformed = curStmt_.has_error();
    token.name = "ReturnStatement";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += returnStmt->_tokenLeadingNewlines;
    curPos += returnStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += ReturnStatement::KEYWORD.size();
    if (returnStmt->value) {
        curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        operator()(*returnStmt->value);
    }
    curPos = backupCurPos;
    curPos += returnStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

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

    auto tokenId = newToken();
    breakStmt->_tokenId = tokenId;
    token.is_malformed = false;
    token.name = "BreakStatement";

    curPos += breakStmt->_tokenLeadingNewlines;
    curPos += breakStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    // lastCorrectToken = -1;
    curPos += breakStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

    curPos += breakStmt->_tokenTrailingNewlines;
}

void ReconstructLV2Tokens::operator()(ContinueStatement* continueStmt) {
    /* NOTE: ContinueStatement cannot be malformed */
    ASSERT (!curStmt.has_error());

    auto tokenId = newToken();
    continueStmt->_tokenId = tokenId;
    token.is_malformed = false;
    token.name = "ContinueStatement";

    curPos += continueStmt->_tokenLeadingNewlines;
    curPos += continueStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    // lastCorrectToken = -1;
    curPos += continueStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

    curPos += continueStmt->_tokenTrailingNewlines;
}

void ReconstructLV2Tokens::operator()(DieStatement* dieStmt) {
    /* NOTE: DieStatement cannot be malformed */
    ASSERT (!curStmt.has_error());

    auto tokenId = newToken();
    dieStmt->_tokenId = tokenId;
    token.is_malformed = false;
    token.name = "DieStatement";

    curPos += dieStmt->_tokenLeadingNewlines;
    curPos += dieStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    // lastCorrectToken = -1;
    curPos += dieStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

    curPos += dieStmt->_tokenTrailingNewlines;
}

void ReconstructLV2Tokens::operator()(MayFail_<ForeachStatement>* foreachStmt) {
    // NOTE: will require lastCorrectToken
    auto curStmt_ = curStmt; // local copy

    auto tokenId = newToken();
    foreachStmt->_tokenId = tokenId;
    token.is_malformed = curStmt_.has_error();
    token.name = "ForeachStatement";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += foreachStmt->_tokenLeadingNewlines;
    curPos += foreachStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += ForeachStatement::KEYWORD.size();
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(foreachStmt->iterable);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(mayfail_cast_by_ref<Expression_>(foreachStmt->block));
    curPos = backupCurPos;
    curPos += foreachStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

    curPos += foreachStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
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

    auto tokenId = newToken();
    whileStmt->_tokenId = tokenId;
    token.is_malformed = curStmt_.has_error();
    token.name = "WhileStatement";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += whileStmt->_tokenLeadingNewlines;
    curPos += whileStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    if (whileStmt->until_loop) {
        curPos += WhileStatement::UNTIL_KEYWORD.size();
    }
    else {
        curPos += WhileStatement::WHILE_KEYWORD.size();
    }
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    curPos += sequenceLen(SquareBracketsTerm::INITIATOR_SEQUENCE);
    operator()(whileStmt->condition);
    curPos += sequenceLen(SquareBracketsTerm::TERMINATOR_SEQUENCE);
    curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
    operator()(mayfail_cast_by_ref<Expression_>(whileStmt->block));
    curPos = backupCurPos;
    curPos += whileStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

    curPos += whileStmt->_tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
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

    auto tokenId = newToken();
    token.is_malformed = curStmt_.has_error();
    token.name = "DoWhileStatement";

    if (token.is_malformed) {
        token.err_fmt = curStmt_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;

    /* C_DoStatement */
    {
        // NOTE: will require lastCorrectToken
        auto& doStmt = doWhileStmt->doStmt.val;

        auto tokenId = newToken();
        doStmt._tokenId = tokenId;
        token.is_malformed = doWhileStmt->doStmt.has_error();
        token.name = "C_DoStatement";

        if (token.is_malformed) {
            token.err_fmt = doWhileStmt->doStmt.error().fmt; // TODO: we will need to fill token.err_desc as well
        }

        curPos += doStmt._tokenLeadingNewlines;
        curPos += doStmt._tokenIndentSpaces;

        token.start = asTokenPosition(curPos);
        auto backupCurPos = curPos;
        auto backupLastCorrectToken = lastCorrectToken;
        // lastCorrectToken = -1;
        curPos += C_DoStatement::KEYWORD.size();
        curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        operator()(mayfail_cast_by_ref<Expression_>(doStmt.block));
        curPos = backupCurPos;
        curPos += doStmt._tokenLen;
        token.end = asTokenPosition(curPos);

        curPos += doStmt._tokenTrailingNewlines;

        if (token.is_malformed) {
            if (lastCorrectToken == size_t(-1)) {
                token.err_start = token.start;
            }
            else {
                token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
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
        whileStmt._tokenId = tokenId;
        token.is_malformed = doWhileStmt->whileStmt.has_error();
        token.name = "C_WhileStatement";

        if (token.is_malformed) {
            token.err_fmt = doWhileStmt->whileStmt.error().fmt; // TODO: we will need to fill token.err_desc as well
        }

        curPos += whileStmt._tokenLeadingNewlines;
        curPos += whileStmt._tokenIndentSpaces;

        token.start = asTokenPosition(curPos);
        auto backupCurPos = curPos;
        auto backupLastCorrectToken = lastCorrectToken;
        // lastCorrectToken = -1;
        if (whileStmt.until_loop) {
            curPos += C_WhileStatement::UNTIL_KEYWORD.size();
        }
        else {
            curPos += C_WhileStatement::WHILE_KEYWORD.size();
        }
        curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        curPos += sequenceLen(SquareBracketsTerm::INITIATOR_SEQUENCE);
        operator()(whileStmt.condition);
        curPos += sequenceLen(SquareBracketsTerm::TERMINATOR_SEQUENCE);
        curPos = backupCurPos;
        curPos += whileStmt._tokenLen;
        token.end = asTokenPosition(curPos);

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

    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
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
    auto tokenId = newToken();
    exprStmt->_tokenId = tokenId;
    token.is_malformed = curStmt.has_error();
    token.name = "ExpressionStatement";

    if (token.is_malformed) {
        token.err_fmt = curStmt.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += exprStmt->_tokenLeadingNewlines;
    curPos += exprStmt->_tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    if (exprStmt->expression) {
        auto backupCurPos = curPos;
        operator()(*exprStmt->expression);
        curPos = backupCurPos;
    }
    curPos += exprStmt->_tokenLen;
    token.end = asTokenPosition(curPos);

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
    auto tokenId = newToken();
    token.is_malformed = curExpr.has_error();
    token.name = "Expression";

    if (token.is_malformed) {
        token.err_fmt = curExpr.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    curPos += group_nesting(curExpr.val);

    token.start = asTokenPosition(curPos);
    token.end = asTokenPosition(curPos);

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
    auto tokenId = newToken();
    operation->_tokenId = tokenId;
    token.is_malformed = curExpr.has_error();
    token.name = "Operation";

    if (token.is_malformed) {
        token.err_fmt = curExpr.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*operation);
    operator()(operation->leftOperand);
    curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
    operator()(&operation->operator_);
    curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
    operator()(operation->rightOperand);
    curPos = backupCurPos;
    curPos += operation->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<FunctionCall>* functionCall) {
    auto tokenId = newToken();
    functionCall->_tokenId = tokenId;
    token.is_malformed = curExpr.has_error();
    token.name = "FunctionCall";

    if (token.is_malformed) {
        token.err_fmt = curExpr.error().fmt; // TODO: we will need to fill token.err_desc as well
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
                token.err_fmt = arg.error().fmt; // TODO: we will need to fill token.err_desc as well
            }

            token.start = asTokenPosition(curPos);
            auto backupCurPos = curPos;
            auto backupLastCorrectToken = lastCorrectToken;
            // lastCorrectToken = -1;
            curPos += arg.val.passByRef;
            operator()(arg.val.expr);
            curPos = backupCurPos;
            curPos += arg.val._tokenLen;
            token.end = asTokenPosition(curPos);

            if (token.is_malformed) {
                if (lastCorrectToken == size_t(-1)) {
                    token.err_start = token.start;
                }
                else {
                    token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
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
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<FieldAccess>* fieldAccess) {
    auto curExpr_ = curExpr;
    auto tokenId = newToken();
    fieldAccess->_tokenId = tokenId;
    token.is_malformed = curExpr_.has_error();
    token.name = "FieldAccess";

    if (token.is_malformed) {
        token.err_fmt = curExpr_.error().fmt; // TODO: we will need to fill token.err_desc as well
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
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
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
    auto tokenId = newToken();
    subscript->_tokenId = tokenId;
    token.is_malformed = curExpr_.has_error();
    token.name = "Subscript";

    if (token.is_malformed) {
        token.err_fmt = curExpr_.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*subscript);
    operator()(subscript->array);
    curPos += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE);
    std::visit(overload{
        [this](MayFail_<Subscript>::Index& index) {
            auto tokenId = newToken();
            index._tokenId = tokenId;
            token.is_malformed = false; // index can't be malformed
            token.name = "SubscriptIndex";
            token.start = asTokenPosition(curPos);
            auto backupCurPos = curPos;

            curPos += 1; // #
            std::visit([this](auto* i){operator()(i);}, index.nth);

            curPos = backupCurPos;
            curPos += index._tokenLen;
            token.end = asTokenPosition(curPos);
        },
        [this](MayFail_<Subscript>::Range& range) {
            auto tokenId = newToken();
            range._tokenId = tokenId;
            token.is_malformed = false; // range can't be malformed
            token.name = "SubscriptRange";
            token.start = asTokenPosition(curPos);
            auto backupCurPos = curPos;

            curPos += 1; // #
            std::visit([this](auto* i){operator()(i);}, range.from);
            curPos += 2; // ..
            if (range.exclusive) curPos += 1; // <
            std::visit([this](auto* i){operator()(i);}, range.to);

            curPos = backupCurPos;
            curPos += range._tokenLen;
            token.end = asTokenPosition(curPos);
        },
        [this](MayFail_<Subscript>::Key key) {
            operator()(key.expr);
        },
        [](MayFail_<Subscript>::_StubArgument_) {
            ;
        }
    }, subscript->argument);
    curPos = backupCurPos;
    curPos += subscript->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<Lambda>* lambda) {
    auto tokenId = newToken();
    lambda->_tokenId = tokenId;
    token.is_malformed = curExpr.has_error();
    token.name = "Lambda";

    if (token.is_malformed) {
        token.err_fmt = curExpr.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += group_nesting(*lambda);
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto& param: lambda->parameters) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        operator()(&param);
        ENDLOOP
    }
    if (lambda->variadicParameters) {
        if (!lambda->parameters.empty()) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        operator()(&*lambda->variadicParameters);
    }
    curPos += sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE);
    curPos += sequenceLen(Association::SEPARATOR_SEQUENCE);
    operator()(mayfail_cast_by_ref<Expression_>(lambda->body));
    curPos = backupCurPos;
    curPos += lambda->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<BlockExpression>* blockExpr) {
    auto tokenId = newToken();
    blockExpr->_tokenId = tokenId;
    token.is_malformed = curExpr.has_error();
    token.name = "BlockExpression";

    if (token.is_malformed) {
        token.err_fmt = curExpr.error().fmt; // TODO: we will need to fill token.err_desc as well
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
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(MayFail_<MapLiteral>* mapLiteral) {
    auto curExpr_ = curExpr; // local copy
    auto tokenId = newToken();
    mapLiteral->_tokenId = tokenId;
    token.is_malformed = curExpr.has_error();
    token.name = "MapLiteral";

    if (token.is_malformed) {
        token.err_fmt = curExpr.error().fmt; // TODO: we will need to fill token.err_desc as well
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
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
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
    auto tokenId = newToken();
    listLiteral->_tokenId = tokenId;
    token.is_malformed = curExpr.has_error();
    token.name = "ListLiteral";

    if (token.is_malformed) {
        token.err_fmt = curExpr.error().fmt; // TODO: we will need to fill token.err_desc as well
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
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV2Tokens::operator()(Numeral* numeral) {
    /* NOTE: Numeral cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken();
    numeral->_tokenId = tokenId;
    token.is_malformed = false;
    token.name = "Numeral";

    token.start = asTokenPosition(curPos);
    curPos += numeral->_tokenLen;
    token.end = asTokenPosition(curPos);
}

void ReconstructLV2Tokens::operator()(StrLiteral* strLiteral) {
    /* NOTE: StrLiteral cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken();
    strLiteral->_tokenId = tokenId;
    token.is_malformed = false;
    token.name = "StrLiteral";

    token.start = asTokenPosition(curPos);
    curPos += strLiteral->_tokenLen;
    token.end = asTokenPosition(curPos);
}

void ReconstructLV2Tokens::operator()(SpecialSymbol* specialSymbol) {
    /* NOTE: SpecialSymbol cannot be malformed */
    ASSERT (!curExpr.has_error());

    auto tokenId = newToken();
    specialSymbol->_tokenId = tokenId;
    token.is_malformed = false;
    token.name = "SpecialSymbol";

    token.start = asTokenPosition(curPos);
    curPos += specialSymbol->_tokenLen;
    token.end = asTokenPosition(curPos);
}

void ReconstructLV2Tokens::operator()(Symbol* symbol) {
    /* NOTE: Symbol cannot be malformed */
    // ASSERT (!curExpr.has_error()); // doesn't necessarily mean parent is an Expression

    auto tokenId = newToken();
    symbol->_tokenId = tokenId;
    token.is_malformed = false;
    token.name = "Symbol";

    token.start = asTokenPosition(curPos);
    curPos += symbol->_tokenLen;
    token.end = asTokenPosition(curPos);
}

///////////////////////////////////////////////////////////////

ReconstructLV2Tokens::ReconstructLV2Tokens(Tokens& tokens, const std::vector<size_t>& newlinesPos, const std::vector<std::pair<size_t, unsigned>>& unicharBytesPos)
        : tokens(tokens), newlinesPos(newlinesPos), unicharBytesPos(unicharBytesPos){}

TokenId ReconstructLV2Tokens::newToken() {
    tokens._vec.push_back(Token{});
    return tokens.size() - 1;
}

TokenPosition ReconstructLV2Tokens::asTokenPosition(size_t index) {
    static const auto cmp_lt_key = [](const std::pair<size_t, unsigned>& a, size_t b){
        return a.first < b;
    };
    static const auto op_add_val = [](size_t a, const std::pair<size_t, unsigned>& b){
        return a + b.second;
    };

    auto lower_bound = std::lower_bound(newlinesPos.begin(), newlinesPos.end(), index);
    size_t line = lower_bound - newlinesPos.begin() + 1;
    size_t lineStartIndex = lower_bound == newlinesPos.begin()? 0 : *(lower_bound - 1);

    auto begin = std::lower_bound(unicharBytesPos.begin(), unicharBytesPos.end(), lineStartIndex, cmp_lt_key);
    auto end = std::lower_bound(begin, unicharBytesPos.end(), index, cmp_lt_key);
    unsigned unicharBytes = std::accumulate(begin, end, 0, op_add_val);
    size_t column = (lower_bound == newlinesPos.begin()? index + 1 : index - *(lower_bound - 1)) - unicharBytes;

    return TokenPosition{index, line, column};
}
