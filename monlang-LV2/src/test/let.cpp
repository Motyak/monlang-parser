#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/context_init.h>

///////////////////////////////////////////////////////////

TEST_CASE ("labelize literal", "[test-4211][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `x`
       |  -> Expression: Literal: `91`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize literal", "[test-4251][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `x`
       |  -> Expression: Literal: `91`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize special value", "[test-4231][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `x`
       |  -> Expression: SpecialSymbol: `$1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize lvalue", "[test-4212][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `y`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `x`
       |  -> Expression: Lvalue: `y`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize lambda", "[test-4213][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `func`
       |  -> ProgramWord #3: Association
       |    -> Word: ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `x`
       |    -> Word: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `func`
       |  -> Expression: Lambda
       |    -> parameter: `x`
       |    -> body
       |      -> Statement: ExpressionStatement
       |        -> Expression: Lvalue: `x`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize operation", "[test-4214][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `sum`
       |  -> ProgramWord #3: Atom: `1`
       |  -> ProgramWord #4: Atom: `+`
       |  -> ProgramWord #5: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `sum`
       |  -> Expression: Operation
       |    -> leftOperand
       |      -> Expression: Literal: `1`
       |    -> operator: `+`
       |    -> rightOperand
       |      -> Expression: Literal: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize block expression", "[test-4215][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `x`
       |  -> Expression: BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: Literal: `91`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

TEST_CASE ("labelize function call", "[test-4216][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: PostfixParenthesesGroup
       |    -> Word: Atom: `func`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `arg`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> identifier: `x`
       |  -> Expression: FunctionCall
       |    -> function
       |      -> Expression: Lvalue: `func`
       |    -> arguments
       |      -> Expression: Lvalue: `arg`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;

    auto output = consumeStatement(input_prog, &cx);
    REQUIRE (!*cx.malformed_stmt); // no err
    REQUIRE (!*cx.fallthrough); // ..
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
