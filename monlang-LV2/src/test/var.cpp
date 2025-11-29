#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("labelize numeral", "[test-2411][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> Symbol: `x`
       |  -> Expression: Numeral: `91`
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

TEST_CASE ("labelize grouped expr", "[test-2451][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> Symbol: `x`
       |  -> Expression: Numeral: `91`
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

TEST_CASE ("labelize special value", "[test-2431][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> Symbol: `x`
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

TEST_CASE ("labelize symbol", "[test-2412][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `y`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> Symbol: `x`
       |  -> Expression: Symbol: `y`
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

TEST_CASE ("labelize lambda", "[test-2413][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
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
       |-> Statement: VarStatement
       |  -> Symbol: `func`
       |  -> Expression: Lambda
       |    -> parameter #1: `x`
       |    -> body
       |      -> Statement: ExpressionStatement
       |        -> Expression: Symbol: `x`
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

TEST_CASE ("labelize operation", "[test-2414][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `sum`
       |  -> ProgramWord #3: Atom: `1`
       |  -> ProgramWord #4: Atom: `+`
       |  -> ProgramWord #5: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> Symbol: `sum`
       |  -> Expression: Operation
       |    -> Expression: Numeral: `1`
       |    -> operator: `+`
       |    -> Expression: Numeral: `1`
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

TEST_CASE ("labelize block expression", "[test-2415][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> Symbol: `x`
       |  -> Expression: BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: Numeral: `91`
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

TEST_CASE ("labelize function call", "[test-2416][var]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: PostfixParenthesesGroup
       |    -> Word: Atom: `func`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `arg`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: VarStatement
       |  -> Symbol: `x`
       |  -> Expression: FunctionCall
       |    -> function
       |      -> Expression: Symbol: `func`
       |    -> argument #1
       |      -> Expression: Symbol: `arg`
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

TEST_CASE ("ERR contains less than 2 words (no name)", "[test-2417][var][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `var`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: VarStatement
       |  ~> ERR-241
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

TEST_CASE ("ERR contains a non-Word as name", "[test-2431][var][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `var`
       |  -> ProgramWord: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `fds`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: VarStatement
       |  ~> ERR-242
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

TEST_CASE ("ERR contains a non-Symbol as name", "[test-2418][var][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: VarStatement
       |  ~> ERR-243
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

TEST_CASE ("ERR contains a non-Atom Symbol as name", "[test-2422][var][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `:=`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: VarStatement
       |  -> Symbol: `:=`
       |  ~> ERR-244
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

TEST_CASE ("ERR contains less than 3 words (no value)", "[test-2419][var][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: VarStatement
       |  -> Symbol: `x`
       |  ~> ERR-245
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

TEST_CASE ("ERR contains a non-Word as part of the value", "[test-2420][var][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `true`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: VarStatement
       |  -> Symbol: `x`
       |  ~> ERR-246
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

TEST_CASE ("ERR contains a Malformed Expression as value", "[test-2421][var][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `var`
       |  -> ProgramWord #2: Atom: `x`
       |  -> ProgramWord #3: Atom: `y`
       |  -> ProgramWord #4: Atom: `z`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: VarStatement
       |  -> Symbol: `x`
       |  ~> Expression
       |    ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (output.error().fmt == "ERR-247");
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
