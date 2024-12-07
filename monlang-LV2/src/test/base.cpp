#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/context_init.h>

///////////////////////////////////////////////////////////

TEST_CASE ("empty program", "[test-1111][base]") {
    auto input = "-> Program";

    auto expect = "-> Program";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = consumeProgram(input_prog, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("one statement program", "[test-1112][base]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord: Atom: `fds`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Program
       |  -> Statement: ExpressionStatement
       |    -> Expression: Lvalue: `fds`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = consumeProgram(input_prog, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
    REQUIRE (output_str == expect);
}
