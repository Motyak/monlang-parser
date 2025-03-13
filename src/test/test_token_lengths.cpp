
#include <utils/tommystring.h>
#include <catch2/catch_amalgamated.hpp>
#include <monlang-LV1/Term.h>
#include <monlang-LV1/Program.h>
#include <monlang-LV1/CurlyBracketsGroup.h>
#include <monlang-LV2/Expression.h>
#include <monlang-LV2/Statement.h>

#include <monlang-LV2/ast/expr/BlockExpression.h>
#include <monlang-LV2/ast/expr/FunctionCall.h>
#include <monlang-LV2/ast/expr/Lambda.h>
#include <monlang-LV2/ast/expr/Numeral.h>
#include <monlang-LV2/ast/Lvalue.h>
#include <monlang-LV2/ast/expr/Operation.h>
#include <monlang-LV2/ast/expr/SpecialSymbol.h>

#include <monlang-LV2/ast/stmt/Assignment.h>
#include <monlang-LV2/ast/stmt/Accumulation.h>
#include <monlang-LV2/ast/stmt/LetStatement.h>
#include <monlang-LV2/ast/stmt/VarStatement.h>
#include <monlang-LV2/ast/stmt/Guard.h>
#include <monlang-LV2/ast/stmt/ReturnStatement.h>
#include <monlang-LV2/ast/stmt/BreakStatement.h>
#include <monlang-LV2/ast/stmt/ContinueStatement.h>
#include <monlang-LV2/ast/stmt/DieStatement.h>
#include <monlang-LV2/ast/stmt/ForeachStatement.h>
#include <monlang-LV2/ast/stmt/WhileStatement.h>
#include <monlang-LV2/ast/stmt/ExpressionStatement.h>

///////////////////////////////////////////////////////////
// expr
///////////////////////////////////////////////////////////

TEST_CASE ("numeral", "[test-9711][expr]") {
    auto input = tommy_str(R"EOF(
        1
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 1);

    auto numeral = *std::get<Numeral*>(expr);
    REQUIRE (token_len(numeral) == 1);
}

///////////////////////////////////////////////////////////

TEST_CASE ("numeral with additional parentheses", "[test-9712][expr]") {
    auto input = tommy_str(R"EOF(
        ((1))
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 5);

    auto numeral = *std::get<Numeral*>(expr);
    REQUIRE (token_len(numeral) == 5);
}

///////////////////////////////////////////////////////////

TEST_CASE ("special symbol", "[test-9713][expr]") {
    auto input = tommy_str(R"EOF(
        $fds
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 4);

    auto specialSymbol = *std::get<SpecialSymbol*>(expr);
    REQUIRE (token_len(specialSymbol) == 4);
}

///////////////////////////////////////////////////////////

TEST_CASE ("symbol", "[test-9714][expr]") {
    auto input = tommy_str(R"EOF(
        somevar
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 7);

    auto symbol = *std::get<Symbol*>(expr);
    REQUIRE (token_len(symbol) == 7);
}

///////////////////////////////////////////////////////////

TEST_CASE ("lambda", "[test-9715][expr]") {
    auto input = tommy_str(R"EOF(
        (x):{x}
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 7);

    auto lambda = *std::get<Lambda*>(expr);
    REQUIRE (token_len(lambda) == 7);
}

///////////////////////////////////////////////////////////

TEST_CASE ("function call", "[test-9716][expr]") {
    auto input = tommy_str(R"EOF(
        doit()
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 6);

    auto functionCall = *std::get<FunctionCall*>(expr);
    REQUIRE (token_len(functionCall) == 6);
}

///////////////////////////////////////////////////////////

TEST_CASE ("single line block expr", "[test-9717][expr]") {
    auto input = tommy_str(R"EOF(
        {1}
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 3);

    auto blockExpr = *std::get<BlockExpression*>(expr);
    REQUIRE (token_len(blockExpr) == 3);
}

///////////////////////////////////////////////////////////

TEST_CASE ("multiline block expr", "[test-9718][expr]") {
    auto input = tommy_str(R"EOF(
        {
            1
        }
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 9);

    auto blockExpr = *std::get<BlockExpression*>(expr);
    REQUIRE (token_len(blockExpr) == 9);
}

///////////////////////////////////////////////////////////
// expr - Operation, precedence
///////////////////////////////////////////////////////////

TEST_CASE ("single operation", "[test-9719][expr]") {
    auto input = tommy_str(R"EOF(
        1 + 2
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 5);

    auto operation = *std::get<Operation*>(expr);
    REQUIRE (token_len(operation.leftOperand) == 1);
    REQUIRE (token_len(operation.rightOperand) == 1);
}

///////////////////////////////////////////////////////////

TEST_CASE ("chained operations, left association", "[test-9720][expr]") {
    auto input = tommy_str(R"EOF(
        1 + 2 + 3
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 9);

    auto operation = *std::get<Operation*>(expr);
    REQUIRE (token_len(operation.leftOperand) == 5); // 1 + 2
    REQUIRE (token_len(operation.rightOperand) == 1); // 3
}

///////////////////////////////////////////////////////////

TEST_CASE ("chained operations, right association", "[test-9721][expr]") {
    auto input = tommy_str(R"EOF(
        1 + 2 * 3
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 9);

    auto operation = *std::get<Operation*>(expr);
    REQUIRE (token_len(operation.leftOperand) == 1); // 1
    REQUIRE (token_len(operation.rightOperand) == 5); // 2 * 3
}

///////////////////////////////////////////////////////////

TEST_CASE ("explicit parentheses", "[test-9722][expr]") {
    auto input = tommy_str(R"EOF(
        (1 + 2) * 3
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 11);

    auto operation = *std::get<Operation*>(expr);
    REQUIRE (token_len(operation.leftOperand) == 7);
    REQUIRE (token_len(operation.rightOperand) == 1);
}

///////////////////////////////////////////////////////////

TEST_CASE ("4 chained operations", "[test-9723][expr]") {
    auto input = tommy_str(R"EOF(
        1 ^ 2 ^ 3 ^ 4
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 13);

    auto operation = *std::get<Operation*>(expr);
    auto operation_ = *std::get<Operation*>(operation.rightOperand);
    auto operation__ = *std::get<Operation*>(operation_.rightOperand);

    REQUIRE (token_len(operation__.leftOperand) == 1); // 3
    REQUIRE (token_len(operation__.rightOperand) == 1); // 4

    REQUIRE (token_len(operation_.leftOperand) == 1); // 2
    REQUIRE (token_len(operation_.rightOperand) == 5); // 3 ^ 4

    REQUIRE (token_len(operation.leftOperand) == 1); // 1 
    REQUIRE (token_len(operation.rightOperand) == 9); // 2 ^ 3 ^ 4
}

///////////////////////////////////////////////////////////

TEST_CASE ("chained nested operation", "[test-9724][expr]") {
    auto input = tommy_str(R"EOF(
        1 + 2 * (3 + 4 * 5)
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 19);

    auto operation = *std::get<Operation*>(expr);
    auto operation_ = *std::get<Operation*>(operation.rightOperand);
    auto operation__ = *std::get<Operation*>(operation_.rightOperand);

    REQUIRE (token_len(operation__.leftOperand) == 1); // 3
    REQUIRE (token_len(operation__.rightOperand) == 5); // 4 * 5

    REQUIRE (token_len(operation_.leftOperand) == 1); // 2
    REQUIRE (token_len(operation_.rightOperand) == 11); // (3 + 4 * 5)

    REQUIRE (token_len(operation.leftOperand) == 1); // 1
    REQUIRE (token_len(operation.rightOperand) == 15); // 2 * (3 + 4 * 5)
}

///////////////////////////////////////////////////////////

TEST_CASE ("chained nested operation, right association then left association", "[test-9725][expr]") {
    auto input = tommy_str(R"EOF(
        1 + 2 * (3 + 4 + 5)
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 19);

    auto operation = *std::get<Operation*>(expr);
    auto operation_ = *std::get<Operation*>(operation.rightOperand);
    auto operation__ = *std::get<Operation*>(operation_.rightOperand);

    REQUIRE (token_len(operation__.leftOperand) == 5); // 3 + 4
    REQUIRE (token_len(operation__.rightOperand) == 1); // 5

    REQUIRE (token_len(operation_.leftOperand) == 1); // 2
    REQUIRE (token_len(operation_.rightOperand) == 11); // (3 + 4 + 5)

    REQUIRE (token_len(operation.leftOperand) == 1); // 1
    REQUIRE (token_len(operation.rightOperand) == 15); // 2 * (3 + 4 + 5)
}

///////////////////////////////////////////////////////////

TEST_CASE ("chained nested operation, left association then right association", "[test-9726][expr]") {
    auto input = tommy_str(R"EOF(
        1 + 2 + (3 + 4 * 5)
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 19);

    auto operation = *std::get<Operation*>(expr);
    auto operation_left = *std::get<Operation*>(operation.leftOperand);
    auto operation_right = *std::get<Operation*>(operation.rightOperand);

    REQUIRE (token_len(operation_left.leftOperand) == 1); // 1
    REQUIRE (token_len(operation_left.rightOperand) == 1); // 2

    REQUIRE (token_len(operation_right.leftOperand) == 1); // 3
    REQUIRE (token_len(operation_right.rightOperand) == 5); // 4 * 5

    REQUIRE (token_len(operation.leftOperand) == 5); // 1 + 2
    REQUIRE (token_len(operation.rightOperand) == 11); // (3 + 4 * 5)
}

///////////////////////////////////////////////////////////

TEST_CASE ("chained nested operation, right association then left association, parentheses first", "[test-9727][expr]") {
    auto input = tommy_str(R"EOF(
        (1 + 2 * 3) + 4 + 5
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 19);

    auto operation = *std::get<Operation*>(expr);
    auto operation_ = *std::get<Operation*>(operation.leftOperand);
    auto operation__ = *std::get<Operation*>(operation_.leftOperand);

    REQUIRE (token_len(operation__.leftOperand) == 1); // 1
    REQUIRE (token_len(operation__.rightOperand) == 5); // 2 * 3

    REQUIRE (token_len(operation_.leftOperand) == 11); // (1 + 2 * 3)
    REQUIRE (token_len(operation_.rightOperand) == 1); // 4

    REQUIRE (token_len(operation.leftOperand) == 15); // (1 + 2 * 3) + 4
    REQUIRE (token_len(operation.rightOperand) == 1); // 5
}

///////////////////////////////////////////////////////////

TEST_CASE ("chained nested operation, left association then right association, parentheses first", "[test-9728][expr]") {
    auto input = tommy_str(R"EOF(
        (1 + 2 + 3) + 4 * 5
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 19);

    auto operation = *std::get<Operation*>(expr);
    auto operation_left = *std::get<Operation*>(operation.leftOperand);
    auto operation_right = *std::get<Operation*>(operation.rightOperand);

    REQUIRE (token_len(operation_left.leftOperand) == 5); // 1 + 2
    REQUIRE (token_len(operation_left.rightOperand) == 1); // 3

    REQUIRE (token_len(operation_right.leftOperand) == 1); // 4
    REQUIRE (token_len(operation_right.rightOperand) == 1); // 5

    REQUIRE (token_len(operation.leftOperand) == 11); // (1 + 2 + 3)
    REQUIRE (token_len(operation.rightOperand) == 5); // 4 * 5
}

///////////////////////////////////////////////////////////
// stmt
///////////////////////////////////////////////////////////

TEST_CASE ("malformed sentence also contains leading newlines + indent spaces", "[test-9729][stmt]") {
    auto input = tommy_str(R"EOF(
       |{
       |
       |\s\s\s\s wrong indent
       |}
       |
    )EOF");

    auto iss = std::istringstream(input);

    auto malformed_cbg = consumeCurlyBracketsGroupStrictly(iss);
    auto malformed_sentence = malformed_cbg.val.sentences.at(0);
    REQUIRE (token_leading_newlines(malformed_sentence.val) == 1);
    REQUIRE (token_indent_spaces(malformed_sentence.val) == 4);
}

///////////////////////////////////////////////////////////

TEST_CASE ("statement leading/trailing newlines and indent spaces", "[test-9730][stmt]") {
    auto input = tommy_str(R"EOF(
       |{
       |\s\s\s\s
       |
       |    somevar := value
       |
       |\s\s\s\s
       |}
       |
    )EOF");

    auto iss = std::istringstream(input);

    auto prog = (LV1::Program)consumeProgram(iss);
    auto stmt = unwrap_stmt(consumeStatement(prog).value());
    auto exprStmt = *std::get<ExpressionStatement*>(stmt);
    auto blockExpr = *std::get<BlockExpression*>(exprStmt.expression);
    auto assignment = *std::get<Assignment*>(blockExpr.statements.at(0));
    REQUIRE (token_leading_newlines(assignment) == 6);
    REQUIRE (token_indent_spaces(assignment) == 4);
    REQUIRE (token_trailing_newlines(assignment) == 6);
}

///////////////////////////////////////////////////////////

TEST_CASE ("assignment", "[test-9731][stmt]") {
    auto input = tommy_str(R"EOF(
       |somevar := value
       |
    )EOF");

    auto iss = std::istringstream(input);

    auto prog = (LV1::Program)consumeProgram(iss);
    auto stmt = unwrap_stmt(consumeStatement(prog).value());
    // REQUIRE (token_len(stmt) == 17); //TODO: uncomment once all stmt have the new field

    auto assignment = *std::get<Assignment*>(stmt);
    REQUIRE (token_len(assignment) == 17);
    REQUIRE (token_len((Expression)assignment.variable) == 7); // somevar
    REQUIRE (token_len(assignment.value) == 5); // value
}

///////////////////////////////////////////////////////////

TEST_CASE ("accumulation", "[test-9732][stmt]") {
    auto input = tommy_str(R"EOF(
       |somevar += value
       |
    )EOF");

    auto iss = std::istringstream(input);

    auto prog = (LV1::Program)consumeProgram(iss);
    auto stmt = unwrap_stmt(consumeStatement(prog).value());
    // REQUIRE (token_len(stmt) == 17); //TODO: uncomment once all stmt have the new field

    auto accumulation = *std::get<Accumulation*>(stmt);
    REQUIRE (token_len(accumulation) == 17);
    REQUIRE (token_len((Expression)accumulation.variable) == 7); // somevar
    REQUIRE (token_len(accumulation.value) == 5); // value
}
