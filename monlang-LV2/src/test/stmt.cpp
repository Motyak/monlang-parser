#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/context_init.h>

///////////////////////////////////////////////////////////

TEST_CASE ("expression statement", "[test-3111][stmt]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `somevar`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: ExpressionStatement
       |  -> Expression: Lvalue: `somevar`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto context = context_init_t{};
    auto output = consumeStatement(input_prog, context);
    auto output_str = montree::astToString(output);

    REQUIRE (input_prog.sentences.empty());
    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("assignment", "[test-3112][stmt]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: Atom: `0`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
       |  -> Expression: Literal: `0`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto context = context_init_t{};
    auto output = consumeStatement(input_prog, context);
    auto output_str = montree::astToString(output);

    REQUIRE (input_prog.sentences.empty());
    REQUIRE (!context.malformed_stmt); // no err
    REQUIRE (output_str == expect);
}
