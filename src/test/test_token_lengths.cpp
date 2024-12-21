
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

TEST_CASE ("chained operations (implicit parentheses)", "[test-9732][expr]") {
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

TEST_CASE ("nested operation (explicit parentheses)", "[test-9733][expr]") {
    auto input = tommy_str(R"EOF(
        1 + (2 * 3)
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());
    REQUIRE (token_len(expr) == 11);

    auto operation = *std::get<Operation*>(expr);
    REQUIRE (token_len(operation.leftOperand) == 1);
    REQUIRE (token_len(operation.rightOperand) == 7);
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
