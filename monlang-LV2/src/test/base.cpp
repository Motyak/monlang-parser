#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("empty program", "[test-1111][base]") {
    auto input = "-> Program";

    auto expect = "-> Program";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);
    auto output = consumeProgram(input_prog);
    auto output_str = montree::astToString(output);

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
    auto output = consumeProgram(input_prog);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

//==============================================================
// ERR
//==============================================================

TEST_CASE ("ERR contains a Malformed Statement", "[test-1113][base][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `a`
       |    -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Program
       |  ~> Statement: ExpressionStatement
       |    ~> Expression
       |      ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);
    auto output = consumeProgram(input_prog);
    REQUIRE (/*Program*/ "ERR-111" == output.error().fmt);
    REQUIRE (/*ExpressionStatement*/ "ERR-591" == output.val.statements.at(0).error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}
