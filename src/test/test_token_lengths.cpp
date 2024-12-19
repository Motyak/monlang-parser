
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

TEST_CASE ("lvalue token length", "[test-9711][expr]") {
    auto input = tommy_str(R"EOF(
        somevar
    )EOF");

    auto iss = std::istringstream(input);
    auto term = (Term)consumeTerm(iss);
    auto expr = unwrap_expr(buildExpression(term).value());

    REQUIRE (true);
    REQUIRE (token_len(expr) == 1);
}
