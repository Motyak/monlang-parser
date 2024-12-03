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
#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <monlang-LV2/expr/Operation.h>
#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/expr/Lambda.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/expr/Literal.h>
#include <monlang-LV2/expr/Lvalue.h>

#include <cstdarg>

#define until(x) while(!(x))

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

///////////////////////////////////////////////////////////////

void PrintLV2::operator()(const LV2::Program& prog) {
    outputLine("-> Program");
    currIndent++;
    for (auto statement: prog.statements) {
        operator()(statement);
    }
    currIndent--;
}

void PrintLV2::operator()(const Statement& statement) {
    output("-> Statement: ");
    std::visit(*this, statement);
}

void PrintLV2::operator()(Assignment* assignment) {
    outputLine("Assignment");
    currIndent++;

    output("-> "); operator()(&assignment->lhs);
    operator()(assignment->rhs);

    currIndent--;
}

void PrintLV2::operator()(Accumulation* accumulation) {
    outputLine("Accumulation");
    currIndent++;

    output("-> "); operator()(&accumulation->lhs);
    outputLine("-> operator: `", accumulation->operator_.c_str(), "`");
    operator()(accumulation->rhs);

    currIndent--;
}

void PrintLV2::operator()(LetStatement* letStatement) {
    outputLine("LetStatement");
    currIndent++;

    outputLine("-> identifier: `", letStatement->identifier.c_str(), "`");
    operator()(letStatement->value);

    currIndent--;
}

void PrintLV2::operator()(VarStatement* varStatement) {
    outputLine("VarStatement");
    currIndent++;

    outputLine("-> identifier: `", varStatement->identifier.c_str(), "`");
    operator()(varStatement->value);

    currIndent--;
}

void PrintLV2::operator()(ReturnStatement* returnStatement) {
    outputLine("ReturnStatement");
    if (returnStatement->value) {
        currIndent++;
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

void PrintLV2::operator()(ForeachStatement* foreachStatement) {
    outputLine("ForeachStatement");
    currIndent++;

    operator()(foreachStatement->iterable);
    output("-> "); operator()(&foreachStatement->block);

    currIndent--;
}

void PrintLV2::operator()(ExpressionStatement* expressionStatement) {
    outputLine("ExpressionStatement");
    currIndent++;
    operator()(expressionStatement->expression);
    currIndent--;
}

void PrintLV2::operator()(const Expression& expression) {
    output("-> Expression: ");
    std::visit(*this, expression);
}

void PrintLV2::operator()(Operation* operation) {
    outputLine("Operation");
    currIndent++;

    outputLine("-> leftOperand");
    currIndent++;
    operator()(operation->leftOperand);
    currIndent--;

    outputLine("-> operator: `", operation->operator_.c_str(), "`");

    outputLine("-> rightOperand");
    currIndent++;
    operator()(operation->rightOperand);
    currIndent--;

    currIndent--;
}

void PrintLV2::operator()(FunctionCall* functionCall) {
    outputLine("FunctionCall");
    currIndent++;

    outputLine("-> function");
    currIndent++;
    operator()(functionCall->function);
    currIndent--;

    output("-> arguments");
    outputLine(functionCall->arguments.empty()? " (none)" : "");
    currIndent++;
    for (auto arg: functionCall->arguments) {
        operator()(arg);
    }
    currIndent--;

    currIndent--;
}

void PrintLV2::operator()(Lambda* lambda) {
    outputLine("Lambda");
    currIndent++;

    /* parameters */
    for (auto parameter: lambda->parameters) {
        //TODO: #1 #2 #.. when multiple parameters
        outputLine("-> parameter: `", parameter.c_str(), "`");
    }

    /* body + body statements */
    outputLine("-> body");
    currIndent++;
    for (auto statement: lambda->body.statements) {
        operator()(statement);
    }
    currIndent--;

    currIndent--;
}

void PrintLV2::operator()(BlockExpression* block) {
    outputLine("BlockExpression");
    currIndent++;
    for (auto statement: block->statements) {
        operator()(statement);
    }
    currIndent--;
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
