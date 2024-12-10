#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("labelize literal", "[test-2311][let]") {
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

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize grouped expr", "[test-2351][let]") {
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

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize special value", "[test-2331][let]") {
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

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize lvalue", "[test-2312][let]") {
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

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize lambda", "[test-2313][let]") {
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
       |    -> parameter #1: `x`
       |    -> body
       |      -> Statement: ExpressionStatement
       |        -> Expression: Lvalue: `x`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize operation", "[test-2314][let]") {
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
       |    -> Expression: Literal: `1`
       |    -> operator: `+`
       |    -> Expression: Literal: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize block expression", "[test-2315][let]") {
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

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("labelize function call", "[test-2316][let]") {
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

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

//==============================================================
// ERR
//==============================================================

TEST_CASE ("ERR contains less than 2 words (no variable)", "[test-2317][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `let`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  ~> ERR-231
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains a non-Atom word as variable", "[test-2318][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  ~> ERR-232
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains less than 3 words (no value)", "[test-2319][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  -> identifier: `x`
       |  ~> ERR-233
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains a non-Word as part of the value", "[test-2320][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `true`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  -> identifier: `x`
       |  ~> ERR-234
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains a Malformed Expression as value", "[test-2321][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `y`
       |  -> ProgramWord #4: Atom: `z`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  -> identifier: `x`
       |  ~> Expression
       |    ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
