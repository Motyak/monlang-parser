#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/context_init.h>

///////////////////////////////////////////////////////////

TEST_CASE ("assign literal", "[test-4111][assign]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
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

TEST_CASE ("assign grouped expr", "[test-4151][assign]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
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

TEST_CASE ("assign lvalue to lvalue", "[test-4112][assign]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: Atom: `othervar`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
       |  -> Expression: Lvalue: `othervar`
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

TEST_CASE ("assign lambda", "[test-4113][assign]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: Association
       |    -> Word: ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `x`
       |    -> Word: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
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

TEST_CASE ("assign operation", "[test-4114][assign]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: Atom: `1`
       |  -> ProgramWord #4: Atom: `+`
       |  -> ProgramWord #5: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
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

TEST_CASE ("assign block expression", "[test-4115][assign]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
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

TEST_CASE ("assign function call", "[test-4116][assign]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `somevar`
       |  -> ProgramWord #2: Atom: `:=`
       |  -> ProgramWord #3: PostfixParenthesesGroup
       |    -> Word: Atom: `func`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `arg`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: Assignment
       |  -> Lvalue: `somevar`
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
