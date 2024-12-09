#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

// TODO: add as first test case the list literal once implemented '[1, 2, 3]'

// TODO: change this to use a grouped literal once we define the list literal '[1, 2, 3]'
TEST_CASE ("iterable grouped expr (lvalue here)", "[test-4311][foreach]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `things`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `print`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: ForeachStatement
       |  -> Expression: Lvalue: `things`
       |  -> BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `print`
       |        -> arguments
       |          -> Expression: SpecialSymbol: `$1`
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

TEST_CASE ("iterable special value", "[test-4331][foreach]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `$1`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `print`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: ForeachStatement
       |  -> Expression: SpecialSymbol: `$1`
       |  -> BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `print`
       |        -> arguments
       |          -> Expression: SpecialSymbol: `$1`
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

TEST_CASE ("iterable function call", "[test-4312][foreach]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: PostfixParenthesesGroup
       |    -> Word: Atom: `List`
       |    -> ParenthesesGroup (empty)
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `print`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: ForeachStatement
       |  -> Expression: FunctionCall
       |    -> function
       |      -> Expression: Lvalue: `List`
       |    -> arguments (none)
       |  -> BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `print`
       |        -> arguments
       |          -> Expression: SpecialSymbol: `$1`
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

TEST_CASE ("iterable operation", "[test-4313][foreach]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `list_a`
       |  -> ProgramWord #3: Atom: `+`
       |  -> ProgramWord #4: Atom: `list_b`
       |  -> ProgramWord #5: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `print`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: ForeachStatement
       |  -> Expression: Operation
       |    -> Expression: Lvalue: `list_a`
       |    -> operator: `+`
       |    -> Expression: Lvalue: `list_b`
       |  -> BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `print`
       |        -> arguments
       |          -> Expression: SpecialSymbol: `$1`
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

TEST_CASE ("iterable block expression", "[test-4314][foreach]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: Atom: `list_a`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `print`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: ForeachStatement
       |  -> Expression: BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: Lvalue: `list_a`
       |  -> BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `print`
       |        -> arguments
       |          -> Expression: SpecialSymbol: `$1`
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

TEST_CASE ("ERR contains less than 3 words", "[test-4315][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `foreach`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  ~> ERR-321
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

TEST_CASE ("ERR contains a non-Word as part of iterable", "[test-4316][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `true`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `print`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  ~> ERR-322
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

TEST_CASE ("ERR contains a Malformed Expression as iterable", "[test-4317][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `a`
       |  -> ProgramWord #3: Atom: `b`
       |  -> ProgramWord #4: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `print`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `$1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  ~> ERR-323
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

TEST_CASE ("ERR contains a non-Word as block", "[test-4318][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `list`
       |  -> ProgramWord #3: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `true`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  -> Expression: Lvalue: `list`
       |  ~> ERR-324
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

TEST_CASE ("ERR contains a Malformed BlockExpression as block", "[test-4319][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `list`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord #1: Atom: `a`
       |      -> ProgramWord #2: Atom: `a`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  -> Expression: Lvalue: `list`
       |  ~> BlockExpression
       |    ~> Statement: ExpressionStatement
       |      ~> Expression
       |        ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
