#include <montree/PrintLV2.h>

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

#include <utils/nb-utils.h>
#include <utils/str-utils.h>
#include <utils/defer-util.h>

#include <cstdarg>

#define unless(x) if(!(x))
#define until(x) while(!(x))

#define SERIALIZE_ERR(malformedMayfail) malformedMayfail.error().fmt.c_str()

namespace {
    constexpr char SPACE = 32;
}

#define outputLine(...) \
    output(__VA_ARGS__ __VA_OPT__(,) "\n", nullptr); \
    startOfNewLine = true

#define output(a, ...) output(a, __VA_ARGS__ __VA_OPT__(,) nullptr)
void (PrintLV2::output)(const char* strs...) {
    if (startOfNewLine) {
        out << std::string(currIndent * TAB_SIZE, SPACE);
    }

    va_list args;
    va_start(args, strs);
    const char* currArg = strs;
    until (currArg == nullptr) {
        out << currArg;
        currArg = va_arg(args, const char*);
    }
    va_end(args);

    startOfNewLine = false;
}

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

///////////////////////////////////////////////////////////////

void PrintLV2::operator()(const MayFail<MayFail_<LV2::Program>>& prog) {
    auto prog_ = prog.val;
    outputLine(prog.has_error()? "~> Program" : "-> Program");

    if (prog_.statements.size() > 0) {
        currIndent++;
    }

    if (prog_.statements.size() > 1) {
        for (int n : range(prog_.statements.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    for (auto statement: prog_.statements) {
        operator()(statement);
    }

    if (prog_.statements.size() > 0) {
        currIndent--;
    }
}

void PrintLV2::operator()(const MayFail<Statement_>& statement) {
    this->currStatement = statement; // needed by stmt handlers
    auto statement_ = statement.val;
    output(statement.has_error()? "~> " : "-> ");

    unless (!is_stub(statement_)) {
        outputLine("Statement");
        currIndent++;
        outputLine("~> ", SERIALIZE_ERR(statement));
        currIndent--;
        return;
    }

    if (numbering.empty()) {
        output("Statement: ");
    } else {
        if (int n = numbering.top(); n == NO_NUMBERING) {
            output("Statement: ");
        } else {
            output("Statement #", INT2CSTR(n), ": ");
        }
        numbering.pop();
    }

    std::visit(*this, statement_);
}

void PrintLV2::operator()(const MayFail<Expression_>& expression) {
    this->currExpression = expression; // needed by expr handlers
    auto expression_ = expression.val;

    output(expression.has_error()? "~> " : "-> ");

    unless (!is_stub(expression_)) {
        outputLine("Expression");
        currIndent++;
        outputLine("~> ", SERIALIZE_ERR(expression));
        currIndent--;
        return;
    }

    output("Expression: ");
    std::visit(*this, expression_);
}

void PrintLV2::operator()(MayFail_<Assignment>* assignment) {
    outputLine("Assignment");
    currIndent++;


    // we assume that empty identifier means stub
    unless (assignment->variable.identifier != "") {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    output("-> ");
    operator()(&assignment->variable);


    unless (!is_stub(assignment->value.val)) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    operator()(assignment->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<Accumulation>* accumulation) {
    outputLine("Accumulation");
    currIndent++;


    // we assume that empty identifier means stub
    unless (accumulation->variable.identifier != "") {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    output("-> ");
    operator()(&accumulation->variable);


    outputLine("-> operator: `", accumulation->operator_.c_str(), "`"); /*
        always wellformed, otherwise wouldn't be peeked in the first place
    */


    unless (!is_stub(accumulation->value.val)) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    operator()(accumulation->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<LetStatement>* letStatement) {
    outputLine("LetStatement");
    currIndent++;


    // we assume that empty identifier means stub
    unless (letStatement->identifier != "") {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    outputLine("-> identifier: `", letStatement->identifier.c_str(), "`");


    if (is_stub(letStatement->value.val)
            && !letStatement->value.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    operator()(letStatement->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<VarStatement>* varStatement) {
    outputLine("VarStatement");
    currIndent++;


    // we assume that empty identifier means stub
    unless (varStatement->identifier != "") {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    outputLine("-> identifier: `", varStatement->identifier.c_str(), "`");


    if (is_stub(varStatement->value.val)
            && !varStatement->value.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    operator()(varStatement->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<ReturnStatement>* returnStatement) {
    outputLine("ReturnStatement");
    if (returnStatement->value) {
        currIndent++;
        unless (!is_stub(returnStatement->value->val)) {
            outputLine("~> ", SERIALIZE_ERR(currStatement));
            currIndent--;
            return;
        }
        operator()(*returnStatement->value);
        currIndent--;
    }
}

void PrintLV2::operator()(BreakStatement*) {
    outputLine("BreakStatement");
}

void PrintLV2::operator()(ContinueStatement*) {
    outputLine("ContinueStatement");
}

void PrintLV2::operator()(DieStatement*) {
    outputLine("DieStatement");
}

void PrintLV2::operator()(MayFail_<ForeachStatement>* foreachStatement) {
    outputLine("ForeachStatement");
    currIndent++;


    unless (!is_stub(foreachStatement->iterable.val)) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }

    output(foreachStatement->iterable.has_error()? "~> " : "-> ");
    outputLine("iterable");
    currIndent++;
    operator()(foreachStatement->iterable);
    currIndent--;


    unless (!foreachStatement->block.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }

    auto any_malformed_stmt = false;
    for (auto statement: foreachStatement->block.val.statements) {
        if (statement.has_error()) {
            any_malformed_stmt = true;
        }
        break;
    }

    output(any_malformed_stmt? "~> " : "-> ");
    outputLine("block");
    currIndent++;
    for (auto statement: foreachStatement->block.val.statements) {
        operator()(statement);
    }
    currIndent--;


    currIndent--;
}

void PrintLV2::operator()(MayFail_<WhileStatement>* whileStatement) {
    auto currStatement_ = currStatement; // backup because gets overwritten when visiting the block expression
    outputLine("WhileStatement");
    currIndent++;


    unless (!is_stub(whileStatement->condition.val)) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }
    output(whileStatement->condition.has_error()? "~> " : "-> ");
    outputLine("condition");
    currIndent++;
    operator()(whileStatement->condition);
    currIndent--;


    unless (!whileStatement->block.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }
    output(whileStatement->block.has_error()? "~> " : "-> ");
    outputLine("block");
    currIndent++;
    for (auto statement: whileStatement->block.val.statements) {
        operator()(statement);
    }
    currIndent--;


    if (currStatement_.has_error() && !whileStatement->block.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
    }

    currIndent--;
}

void PrintLV2::operator()(MayFail_<DoWhileStatement>* doWhileStatement) {
    auto currStatement_ = currStatement; // backup because gets overwritten when visiting the block expression
    outputLine("DoWhileStatement");
    currIndent++;

    unless (!doWhileStatement->block.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }
    output(doWhileStatement->block.has_error()? "~> " : "-> ");
    outputLine("block");
    currIndent++;
    for (auto statement: doWhileStatement->block.val.statements) {
        operator()(statement);
    }
    currIndent--;

    if (doWhileStatement->block.has_error()) {
        return;
    }

    unless (!is_stub(doWhileStatement->condition.val)) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }
    output(doWhileStatement->condition.has_error()? "~> " : "-> ");
    outputLine("condition");
    currIndent++;
    operator()(doWhileStatement->condition);
    currIndent--;


    if (currStatement_.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
    }

    currIndent--;
}

void PrintLV2::operator()(MayFail_<ExpressionStatement>* expressionStatement) {
    outputLine("ExpressionStatement");
    currIndent++;
    operator()(expressionStatement->expression);
    currIndent--;
}

void PrintLV2::operator()(MayFail_<Operation>* operation) {
    auto currExpression_ = currExpression; // backup because we operation contains expressions as well
    outputLine("Operation");
    currIndent++;


    unless (!is_stub(operation->leftOperand.val)) {
        outputLine("~> ", SERIALIZE_ERR(currExpression_));
        currIndent--;
        return;
    }
    operator()(operation->leftOperand);


    outputLine("-> operator: `", operation->operator_.c_str(), "`");


    unless (!is_stub(operation->rightOperand.val)) {
        outputLine("~> ", SERIALIZE_ERR(currExpression_));
        currIndent--;
        return;
    }
    operator()(operation->rightOperand);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<FunctionCall>* functionCall) {
    outputLine("FunctionCall");
    currIndent++;


    unless (!is_stub(functionCall->function.val)) {
        outputLine("~> ", SERIALIZE_ERR(currExpression));
        currIndent--;
        return;
    }
    outputLine("-> function");
    currIndent++;
    operator()(functionCall->function);
    currIndent--;

    unless (!functionCall->arguments.empty()) {
        outputLine("-> arguments (none)");
        currIndent--;
        return;
    }

    auto any_malformed_arg = false;
    for (auto arg: functionCall->arguments) {
        if (arg.has_error()) {
            any_malformed_arg = true;
            break;
        }
    }

    output(any_malformed_arg? "~> " : "-> ");
    outputLine("arguments");
    currIndent++;
    for (auto arg: functionCall->arguments) {
        operator()(arg);
    }
    currIndent--;


    currIndent--;
}

void PrintLV2::operator()(MayFail_<Lambda>* lambda) {
    outputLine("Lambda");
    currIndent++;

    if (lambda->parameters.empty()) {
        outputLine("-> (no parameters)");
    } else {
        int i = 1;
        for (auto parameter: lambda->parameters) {
            outputLine("-> parameter #", INT2CSTR(i++), ": `", parameter.c_str(), "`");
        }
    }

    unless (!lambda->body.statements.empty()) {
        outputLine("-> body (empty)");
        currIndent--;
        return;
    }

    auto any_malformed_stmt = false;
    for (auto stmt: lambda->body.statements) {
        if (stmt.has_error()) {
            any_malformed_stmt = true;
            break;
        }
    }

    output(any_malformed_stmt? "~> " : "-> ");
    outputLine("body");
    currIndent++;
    for (auto statement: lambda->body.statements) {
        operator()(statement);
    }
    currIndent--;

    currIndent--;
}

void PrintLV2::operator()(MayFail_<BlockExpression>* block) {
    outputLine("BlockExpression");
    currIndent++;
    for (auto statement: block->statements) {
        operator()(statement);
    }
    currIndent--;
}

void PrintLV2::operator()(SpecialSymbol* specialSymbol) {
    outputLine("SpecialSymbol: `", specialSymbol->str.c_str(), "`");
}

void PrintLV2::operator()(Literal* literal) {
    outputLine("Literal: `", literal->str.c_str(), "`");
}

void PrintLV2::operator()(Lvalue* lvalue) {
    outputLine("Lvalue: `", lvalue->identifier.c_str(), "`");
}

void PrintLV2::operator()(auto) {
    outputLine("<ENTITY NOT IMPLEMENTED YET>");
}

///////////////////////////////////////////////////////////

PrintLV2::PrintLV2(std::ostream& os, int TAB_SIZE) : TAB_SIZE(TAB_SIZE), out(os){}
