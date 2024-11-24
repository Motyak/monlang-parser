#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/context_init.h>

///////////////////////////////////////////////////////////

TEST_CASE ("lvalue from atom", "[test-2111][rvalue]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Rvalue: Lvalue: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("grouped rvalue => ungroup", "[test-2112][rvalue]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Rvalue: Lvalue: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}
