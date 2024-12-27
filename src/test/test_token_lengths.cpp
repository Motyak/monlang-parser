
#include <utils/tommystring.h>
#include <catch2/catch_amalgamated.hpp>
#include <monlang-LV1/Term.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV2/ast/expr/BlockExpression.h>
#include <monlang-LV2/ast/expr/FunctionCall.h>
#include <monlang-LV2/ast/expr/Lambda.h>
#include <monlang-LV2/ast/expr/Literal.h>
#include <monlang-LV2/ast/expr/Lvalue.h>
#include <monlang-LV2/ast/expr/Operation.h>
#include <monlang-LV2/ast/expr/SpecialSymbol.h>

///////////////////////////////////////////////////////////

// TEST_CASE ("lvalue token length", "[test-9711][expr]") {
//     auto input = tommy_str(R"EOF(
//         somevar
//     )EOF");

//     auto iss = std::istringstream(input);
//     auto term = (Term)consumeTerm(iss);
//     auto expr = unwrap_expr(buildExpression(term).value());

//     REQUIRE (true);
//     REQUIRE (token_len(expr) == 1);
// }

///////////////////////////////////////////////////////////

TEST_CASE ("additional parentheses", "[test-9730][expr]") {
    auto input = tommy_str(R"EOF(
        ((1))
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 5);

    auto literal = *std::get<Literal*>(expr);
    REQUIRE (token_len(literal) == 5);
}

///////////////////////////////////////////////////////////

TEST_CASE ("single operation", "[test-9731][expr]") {
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

TEST_CASE ("chained operations, left association", "[test-9761][expr]") {
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

TEST_CASE ("chained operations, right association", "[test-9732][expr]") {
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

TEST_CASE ("explicit parentheses", "[test-9733][expr]") {
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

TEST_CASE ("chained nested operation", "[test-9734][expr]") {
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

// TEST_CASE ("hardcore mode", "[test-9735][expr]") {
//     auto input = tommy_str(R"EOF(
//         1 ^ 2 ^ ((3 + 4)) ^ 5
//     )EOF");

//     auto iss = std::istringstream(input);
//     auto term = (Term)consumeTerm(iss);
//     auto expr = unwrap_expr(buildExpression(term).value());
//     REQUIRE (token_len(expr) == 21);

//     auto operation = *std::get<Operation*>(expr);
//     auto operation_ = *std::get<Operation*>(operation.rightOperand);
//     auto operation__ = *std::get<Operation*>(operation_.rightOperand);

//     REQUIRE (token_len(operation__.leftOperand) == 9); // ((3 + 4))
//     REQUIRE (token_len(operation__.rightOperand) == 1); // 5

//     REQUIRE (token_len(operation_.leftOperand) == 1); // 2
//     REQUIRE (token_len(operation_.rightOperand) == 13); // ((3 + 4)) ^ 5

//     REQUIRE (token_len(operation.leftOperand) == 1); // 1
//     REQUIRE (token_len(operation.rightOperand) == 17); // 2 ^ ((3 + 4)) ^ 5

// }

///////////////////////////////////////////////////////////

TEST_CASE ("4 chained operations", "[test-9735][expr]") {
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
// further investigate: chained nested operation
///////////////////////////////////////////////////////////

TEST_CASE ("chained nested operation, right association then left association", "[test-9771][expr]") {
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

TEST_CASE ("chained nested operation, left association then right association", "[test-9772][expr]") {
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

TEST_CASE ("chained nested operation, right association then left association, parentheses first", "[test-9773][expr]") {
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

TEST_CASE ("chained nested operation, left association then right association, parentheses first", "[test-9774][expr]") {
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
