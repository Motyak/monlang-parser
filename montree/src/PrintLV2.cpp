#include <montree/PrintLV2.h>

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

    outputLine("-> arguments");
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
