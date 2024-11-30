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
    REQUIRE (!context.malformed_stmt); // no err
    REQUIRE (!context.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
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
    REQUIRE (!context.malformed_stmt); // no err
    REQUIRE (!context.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("accumulation", "[test-3113][stmt]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `count`
       |  -> ProgramWord #2: Atom: `+=`
       |  -> ProgramWord #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Accumulation
       |  -> Lvalue: `count`
       |  -> operator: `+`
       |  -> Expression: Literal: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto context = context_init_t{};

    auto output = consumeStatement(input_prog, context);
    REQUIRE (!context.malformed_stmt); // no err
    REQUIRE (!context.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("let statement", "[test-3114][stmt]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `age`
       |  -> ProgramWord #3: Atom: `27`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `age`
       |  -> Expression: Literal: `27`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto context = context_init_t{};

    auto output = consumeStatement(input_prog, context);
    REQUIRE (!context.malformed_stmt); // no err
    REQUIRE (!context.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("var statement", "[test-3115][stmt]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `count`
       |  -> ProgramWord #3: Atom: `0`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> identifier: `count`
       |  -> Expression: Literal: `0`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto context = context_init_t{};

    auto output = consumeStatement(input_prog, context);
    REQUIRE (!context.malformed_stmt); // no err
    REQUIRE (!context.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
