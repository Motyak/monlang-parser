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
#include <monlang-LV2/expr/FieldAccess.h>
#include <monlang-LV2/expr/Subscript.h>
#include <monlang-LV2/expr/MapLiteral.h>
#include <monlang-LV2/expr/ListLiteral.h>
#include <monlang-LV2/expr/SpecialSymbol.h>
#include <monlang-LV2/expr/Numeral.h>
#include <monlang-LV2/expr/StrLiteral.h>

#include <utils/nb-utils.h>
#include <utils/str-utils.h>
#include <utils/defer-util.h>
#include <utils/variant-utils.h>
#include <utils/assert-utils.h>
#include <utils/loop-utils.h>

#include <cstdarg>

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

// should only be used to check for _StubStatement_{}
static bool is_stub(const Statement_& stmt) {
    return std::holds_alternative<_StubStatement_*>(stmt);
}

// should only be used to check for StubExpression_()
static bool is_stub(const Expression_& expr) {
    return std::holds_alternative<_StubExpression_*>(expr);
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
    } else if (!prog_.statements.empty()) {
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

    if (numbering.empty()) {
        output("Statement");
    } else {
        if (int n = numbering.top(); n == NO_NUMBERING) {
            output("Statement");
        } else {
            output("Statement #", INT2CSTR(n));
        }
        numbering.pop();
    }

    if (is_stub(statement_)) {
        outputLine();
        currIndent++;
        outputLine("~> ", SERIALIZE_ERR(statement));
        currIndent--;
        return;
    }

    output(": ");
    std::visit(*this, statement_);
}

void PrintLV2::operator()(const MayFail<Expression_>& expression) {
    this->currExpression = expression; // needed by expr handlers

    output(expression.has_error()? "~> " : "-> ");

    if (numbering.empty()) {
        output("Expression");
    } else {
        if (int n = numbering.top(); n == NO_NUMBERING) {
            output("Expression");
        } else {
            output("Expression #", INT2CSTR(n));
        }
        numbering.pop();
    }

    if (is_stub(expression.val)) {
        outputLine();
        currIndent++;
        outputLine("~> ", SERIALIZE_ERR(expression));
        currIndent--;
        return;
    }

    output(": ");
    std::visit(*this, expression.val);
}

void PrintLV2::operator()(const MayFail<Lvalue_>& lvalue) {
    this->currExpression = mayfail_cast<Expression_>(lvalue); // needed by expr handlers
    auto lvalue_ = lvalue.val;

    output(lvalue.has_error()? "~> " : "-> ");

    if (lvalue_._stub) {
        outputLine("Lvalue");
        currIndent++;
        outputLine("~> ", SERIALIZE_ERR(lvalue));
        currIndent--;
        return;
    }

    output("Lvalue: ");
    std::visit(*this, lvalue_.variant);
}

void PrintLV2::operator()(MayFail_<Assignment>* assignment) {
    outputLine("Assignment");
    currIndent++;


    if (assignment->variable.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    operator()(assignment->variable);


    if (is_stub(assignment->value.val)
            && !assignment->value.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    numbering.push(NO_NUMBERING);
    operator()(assignment->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<Accumulation>* accumulation) {
    outputLine("Accumulation");
    currIndent++;


    if (accumulation->variable.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    operator()(accumulation->variable);


    outputLine("-> operator: `", accumulation->operator_.name.c_str(), "`"); /*
        always wellformed, otherwise wouldn't be peeked in the first place
    */


    if (is_stub(accumulation->value.val)
            && !accumulation->value.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    numbering.push(NO_NUMBERING);
    operator()(accumulation->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<LetStatement>* letStatement) {
    outputLine("LetStatement");
    currIndent++;


    // we assume that empty name means stub
    if (letStatement->label.name == "") {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    output("-> ");
    operator()(&letStatement->label);


    if (is_stub(letStatement->value.val)
            && !letStatement->value.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    numbering.push(NO_NUMBERING);
    operator()(letStatement->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<VarStatement>* varStatement) {
    outputLine("VarStatement");
    currIndent++;


    // we assume that empty name means stub
    if (varStatement->variable.name == "") {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    output("-> ");
    operator()(&varStatement->variable);


    if (is_stub(varStatement->value.val)
            && !varStatement->value.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement));
        currIndent--;
        return;
    }
    numbering.push(NO_NUMBERING);
    operator()(varStatement->value);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<ReturnStatement>* returnStatement) {
    outputLine("ReturnStatement");
    if (returnStatement->value) {
        currIndent++;
        if (is_stub(returnStatement->value->val)) {
            outputLine("~> ", SERIALIZE_ERR(currStatement));
            currIndent--;
            return;
        }
        numbering.push(NO_NUMBERING);
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
    auto currStatement_ = currStatement; // local copy

    outputLine("ForeachStatement");
    currIndent++;


    if (is_stub(foreachStatement->iterable.val)
            && !foreachStatement->iterable.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }

    output(foreachStatement->iterable.has_error()? "~> " : "-> ");
    outputLine("iterable");
    currIndent++;
    numbering.push(NO_NUMBERING);
    operator()(foreachStatement->iterable);
    currIndent--;

    if (foreachStatement->iterable.has_error()) {
        return;
    }

    if (foreachStatement->block.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
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

    if (foreachStatement->block.val.statements.size() > 1) {
        for (int n : range(foreachStatement->block.val.statements.size(), 0)) {
            numbering.push(n);
        }
    } else {
        // NOTE: foreach block cannot be empty
        numbering.push(NO_NUMBERING);
    }

    outputLine(any_malformed_stmt? "~> block" : "-> block");
    currIndent++;
    for (auto statement: foreachStatement->block.val.statements) {
        operator()(statement);
    }
    currIndent--;

    if (currStatement_.has_error() && !any_malformed_stmt) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
    }

    currIndent--;
}

void PrintLV2::operator()(MayFail_<WhileStatement>* whileStatement) {
    auto currStatement_ = currStatement; // backup because gets overwritten when visiting the block expression
    outputLine("WhileStatement");
    currIndent++;


    if (is_stub(whileStatement->condition.val)
            && !whileStatement->condition.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }
    output(whileStatement->condition.has_error()? "~> " : "-> ");
    outputLine("condition");
    currIndent++;
    numbering.push(NO_NUMBERING);
    operator()(whileStatement->condition);
    currIndent--;

    if (whileStatement->condition.has_error()) {
        return;
    }

    if (whileStatement->block.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }

    if (whileStatement->block.val.statements.size() > 1) {
        for (int n : range(whileStatement->block.val.statements.size(), 0)) {
            numbering.push(n);
        }
    } else {
        // NOTE: while block cannot be empty
        numbering.push(NO_NUMBERING);
    }

    outputLine(whileStatement->block.has_error()? "~> block" : "-> block");
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


    output(doWhileStatement->doStmt.has_error()? "~> " : "-> ");
    outputLine("C_DoStatement");
    currIndent++;
    {
        auto& doStmt = doWhileStatement->doStmt.val;
        if (doStmt.block.val._stub) {
            outputLine("~> ", SERIALIZE_ERR(doWhileStatement->doStmt));
            currIndent--;
            return;
        }

        if (doStmt.block.val.statements.size() > 1) {
            for (int n : range(doStmt.block.val.statements.size(), 0)) {
                numbering.push(n);
            }
        } else {
            // NOTE: do..while block cannot be empty
            numbering.push(NO_NUMBERING);
        }

        for (auto statement: doStmt.block.val.statements) {
            operator()(statement);
        }

        if (doStmt.block.has_error()) {
            return;
        }

        if (doWhileStatement->doStmt.has_error()) {
            outputLine("~> ", SERIALIZE_ERR(doWhileStatement->doStmt));
        }
    }
    currIndent--;


    if (doWhileStatement->doStmt.has_error()) {
        currIndent--;
        return;
    }

    if (doWhileStatement->whileStmt.val._stub) {
        outputLine("~> ", SERIALIZE_ERR(currStatement_));
        currIndent--;
        return;
    }


    output(doWhileStatement->whileStmt.has_error()? "~> " : "-> ");
    outputLine("C_WhileStatement");
    currIndent++;
    {
        auto& whileStmt = doWhileStatement->whileStmt.val;
        if (is_stub(whileStmt.condition.val)
                && !whileStmt.condition.has_error()) {
            outputLine("~> ", SERIALIZE_ERR(doWhileStatement->whileStmt));
            currIndent--;
            return;
        }
        numbering.push(NO_NUMBERING);
        operator()(whileStmt.condition);
    }
    currIndent--;


    currIndent--;
}

void PrintLV2::operator()(MayFail_<ExpressionStatement>* expressionStatement) {
    output("ExpressionStatement");
    if (!expressionStatement->expression) {
        outputLine(" (empty)");
        return;
    }
    outputLine();
    currIndent++;
    numbering.push(NO_NUMBERING);
    operator()(*expressionStatement->expression);
    currIndent--;
}

void PrintLV2::operator()(MayFail_<Operation>* operation) {
    auto currExpression_ = currExpression; // backup in case left operand is an operation as well (would overwrite)
    outputLine("Operation");
    currIndent++;


    if (is_stub(operation->leftOperand.val)
            && !operation->leftOperand.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currExpression_));
        currIndent--;
        return;
    }
    numbering.push(NO_NUMBERING);
    operator()(operation->leftOperand);

    if (operation->leftOperand.has_error()) {
        return;
    }


    outputLine("-> operator: `", operation->operator_.name.c_str(), "`");


    if (is_stub(operation->rightOperand.val)
            && !operation->rightOperand.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currExpression_));
        currIndent--;
        return;
    }
    numbering.push(NO_NUMBERING);
    operator()(operation->rightOperand);


    currIndent--;
}

void PrintLV2::operator()(MayFail_<FunctionCall>* functionCall) {
    outputLine("FunctionCall");
    currIndent++;


    if (is_stub(functionCall->function.val)
            && !functionCall->function.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currExpression));
        currIndent--;
        return;
    }
    output(functionCall->function.has_error()? "~> " : "-> ");
    outputLine("function");
    currIndent++;
    numbering.push(NO_NUMBERING);
    operator()(functionCall->function);
    currIndent--;

    if (functionCall->function.has_error()) {
        return;
    }

    if (functionCall->arguments.empty()) {
        outputLine("-> arguments (none)");
        currIndent--;
        return;
    }

    int i = 0;
    for (auto arg: functionCall->arguments) {
        output(arg.has_error()? "~> argument #" : "-> argument #");
        output(INT2CSTR(++i));
        if (arg.val.passByRef) {
            output(" (passed by reference)");
        }
        outputLine();
        currIndent++;
        numbering.push(NO_NUMBERING);
        operator()(arg.val.expr);
        if (arg.has_error() && !arg.val.expr.has_error()) {
            outputLine("~> ", SERIALIZE_ERR(arg));
        }
        currIndent--;
    }
    currIndent--;
}

void PrintLV2::operator()(MayFail_<Lambda>* lambda) {
    outputLine("Lambda");
    currIndent++;

    if (lambda->parameters.empty() && !lambda->variadicParameters) {
        outputLine("-> (no parameters)");
    } else {
        int i = 1;
        for (auto parameter: lambda->parameters) {
            outputLine("-> parameter #", INT2CSTR(i++), ": `", parameter.name.c_str(), "`");
        }
    }

    if (lambda->variadicParameters) {
        outputLine("-> variadic parameters: `", lambda->variadicParameters->name.c_str(), "`");
    }

    if (!lambda->body.has_error() && lambda->body.val.statements.empty()) {
        outputLine("-> body (empty)");
        currIndent--;
        return;
    }

    output(lambda->body.has_error()? "~> " : "-> ");
    outputLine("body");
    currIndent++;
    if (lambda->body.val.statements.size() > 1) {
        for (int n : range(lambda->body.val.statements.size(), 0)) {
            numbering.push(n);
        }
    } else if (!lambda->body.val.statements.empty()) {
        numbering.push(NO_NUMBERING);
    }
    for (auto statement: lambda->body.val.statements) {
        operator()(statement);
    }
    currIndent--;

    currIndent--;
}

void PrintLV2::operator()(MayFail_<BlockExpression>* block) {
    outputLine("BlockExpression");
    currIndent++;

    if (block->statements.size() > 1) {
        for (int n : range(block->statements.size(), 0)) {
            numbering.push(n);
        }
    } else if (!block->statements.empty()) {
        numbering.push(NO_NUMBERING);
    }

    for (auto statement: block->statements) {
        operator()(statement);
    }
    currIndent--;
}

void PrintLV2::operator()(MayFail_<FieldAccess>* fieldAccess) {
    auto currExpression_ = currExpression;
    outputLine("FieldAccess");
    currIndent++;

    numbering.push(NO_NUMBERING);
    operator()(fieldAccess->object);
    if (fieldAccess->object.has_error()) {
        currIndent--;
        return;
    }

    // empty symbol indicates stub value
    if (fieldAccess->field.name == ""
            && currExpression_.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(currExpression_));
        currIndent--;
        return;
    }

    output("-> ");
    operator()(&fieldAccess->field);

    currIndent--;
}

void PrintLV2::operator()(MayFail_<Subscript>* subscript) {
    auto currExpression_ = currExpression;
    outputLine("Subscript");
    currIndent++;

    numbering.push(NO_NUMBERING);
    operator()(subscript->array);
    if (subscript->array.has_error()) {
        currIndent--;
        return;
    }

    std::visit(overload{
        [](MayFail_<Subscript>::_StubArgument_){
            ; // nothing to print
        },

        [this](MayFail_<Subscript>::Index index){
            outputLine("-> index");
            currIndent++;
            numbering.push(NO_NUMBERING);
            operator()((Expression_)variant_cast(index.nth));
            currIndent--;
        },

        [this](MayFail_<Subscript>::Range range){
            outputLine(range.exclusive? "-> (exclusive) range" : "-> range");
            currIndent++;
            numbering.push(NO_NUMBERING);
            operator()((Expression_)variant_cast(range.from));
            numbering.push(NO_NUMBERING);
            operator()((Expression_)variant_cast(range.to));
            currIndent--;
        },

        [this](MayFail_<Subscript>::Key key){
            outputLine(key.expr.has_error()? "~> key" : "-> key");
            currIndent++;
            numbering.push(NO_NUMBERING);
            operator()(key.expr);
            currIndent--;
        },
    }, subscript->argument);

    currIndent--;
}

void PrintLV2::operator()(MayFail_<MapLiteral>* mapLiteral) {
    auto currExpression_ = currExpression; // save map literal MayFail

    output("MapLiteral");
    if (mapLiteral->arguments.size() == 0 && !currExpression_.has_error()) {
        outputLine(" (empty)");
        return;
    }
    outputLine();

    currIndent++;

    size_t nth_argument = 0;
    LOOP for (auto [key, val]: mapLiteral->arguments) {
        nth_argument = __nth_it;
        output(
            (key.has_error() || val.has_error())? "~> " : "-> "
        );
        outputLine("argument #", INT2CSTR(nth_argument));
        currIndent++;

        output(key.has_error()? "~> " : "-> ");
        outputLine("key");
        currIndent++;
        numbering.push(NO_NUMBERING);
        operator()(key);
        currIndent--;

        if (!key.has_error()) {
            output(val.has_error()? "~> " : "-> ");
            outputLine("value");
            currIndent++;
            numbering.push(NO_NUMBERING);
            operator()(val);
            currIndent--;
        }

        currIndent--;

        ENDLOOP
    }

    // ugly but works
    if (currExpression_.has_error() && currExpression_.error().code == 691) {
        ASSERT (nth_argument >= 1);
        outputLine("~> argument #", INT2CSTR(++nth_argument));
        currIndent++;
        outputLine("~> ", currExpression_.error().fmt.c_str());
        currIndent--;
    }

    currIndent--;
}

void PrintLV2::operator()(MayFail_<ListLiteral>* listLiteral) {
    output("ListLiteral");
    if (listLiteral->arguments.size() == 0 && !currExpression.has_error()) {
        outputLine(" (empty)");
        return;
    }
    outputLine();

    currIndent++;

    if (listLiteral->arguments.size() > 1) {
        for (int n : range(listLiteral->arguments.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    for (auto arg: listLiteral->arguments) {
        operator()(arg);
    }

    currIndent--;
}

void PrintLV2::operator()(SpecialSymbol* specialSymbol) {
    outputLine("SpecialSymbol: `", specialSymbol->name.c_str(), "`");
}

void PrintLV2::operator()(Numeral* numeral) {
    outputLine("Numeral: `", numeral->fmt.c_str(), "`");
}

void PrintLV2::operator()(StrLiteral* strLiteral) {
    auto strLiteralLines = split(strLiteral->str, "\n");
    ASSERT (strLiteralLines.size() > 0);
    output("StrLiteral: `", strLiteralLines[0].c_str(), "`");
    if (strLiteralLines.size() > 1) {
        output(" (", INT2CSTR(strLiteralLines.size() - 1), " more ");
        output(strLiteralLines.size() > 2? "lines" : "line", ")");
        outputLine();
        currIndent++;
        for (auto it = strLiteralLines.begin() + 1; it != strLiteralLines.end(); ++it) {
            outputLine("-> `", it->c_str(), "`");
        }
        currIndent--;
    }
    else {
        outputLine();
    }
}

void PrintLV2::operator()(Symbol* symbol) {
    outputLine("Symbol: `", symbol->name.c_str(), "`");
}

void PrintLV2::operator()(_StubStatement_*) {
    SHOULD_NOT_HAPPEN(); // already handled in operator()(MayFail<Statement_>)
}

void PrintLV2::operator()(_StubExpression_*) {
    SHOULD_NOT_HAPPEN(); // already handled in operator()(MayFail<Expression_>)
}

///////////////////////////////////////////////////////////

PrintLV2::PrintLV2(std::ostream& os, int TAB_SIZE) : TAB_SIZE(TAB_SIZE), out(os){}
