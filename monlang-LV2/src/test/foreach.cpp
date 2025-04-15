#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV1/ast/CurlyBracketsGroup.h>

///////////////////////////////////////////////////////////

// TODO: add as first test case the list literal once implemented '[1, 2, 3]'

// TODO: change this to use a grouped literal once we define the list literal '[1, 2, 3]'
TEST_CASE ("iterable grouped expr (symbol here)", "[test-3211][foreach]") {
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
       |  -> iterable
       |    -> Expression: Symbol: `things`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `print`
       |        -> argument #1
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

TEST_CASE ("iterable special value", "[test-3231][foreach]") {
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
       |  -> iterable
       |    -> Expression: SpecialSymbol: `$1`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `print`
       |        -> argument #1
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

TEST_CASE ("iterable function call", "[test-3212][foreach]") {
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
       |  -> iterable
       |    -> Expression: FunctionCall
       |      -> function
       |        -> Expression: Symbol: `List`
       |      -> arguments (none)
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `print`
       |        -> argument #1
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

TEST_CASE ("iterable operation", "[test-3213][foreach]") {
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
       |  -> iterable
       |    -> Expression: Operation
       |      -> Expression: Symbol: `list_a`
       |      -> operator: `+`
       |      -> Expression: Symbol: `list_b`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `print`
       |        -> argument #1
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

TEST_CASE ("iterable block expression", "[test-3214][foreach]") {
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
       |  -> iterable
       |    -> Expression: BlockExpression
       |      -> Statement: ExpressionStatement
       |        -> Expression: Symbol: `list_a`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `print`
       |        -> argument #1
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

TEST_CASE ("ERR contains less than 3 words", "[test-3215][foreach][err]") {
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

TEST_CASE ("ERR contains a non-Word as part of iterable", "[test-3216][foreach][err]") {
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

TEST_CASE ("ERR contains a Malformed Expression as iterable", "[test-3217][foreach][err]") {
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
       |  ~> iterable
       |    ~> Expression
       |      ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (/*ForeachStatement*/output.error().fmt == "ERR-323");
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains a non-Word as block", "[test-3218][foreach][err]") {
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
       |  -> iterable
       |    -> Expression: Symbol: `list`
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

TEST_CASE ("ERR contains a Malformed BlockExpression as block", "[test-3219][foreach][err]") {
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
       |  -> iterable
       |    -> Expression: Symbol: `list`
       |  ~> block
       |    ~> Statement: ExpressionStatement
       |      ~> Expression
       |        ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (output.error().fmt == "ERR-325");
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains a non-BlockExpression as block", "[test-3232][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `list`
       |  -> ProgramWord #3: ParenthesesGroup
       |    -> Term
       |      -> Word : Atom: `fds`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  -> iterable
       |    -> Expression: Symbol: `list`
       |  ~> ERR-326
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

TEST_CASE ("ERR contains a dollars BlockExpression as block", "[test-3233][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `list`
       |  -> ProgramWord #3: CurlyBracketsGroup (empty)
    )EOF");
    auto input_ast = montree::buildLV1Ast(input);
    // in montree LV1 we don't differentiate dollars cbg from standard cbg, so...
    std::get<CurlyBracketsGroup*>(std::get<ProgramSentence>(input_ast).programWords.at(2))->_dollars = true;
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  -> iterable
       |    -> Expression: Symbol: `list`
       |  ~> ERR-328
    )EOF");

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains a oneline BlockExpression as block", "[test-3234][foreach][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `foreach`
       |  -> ProgramWord #2: Atom: `list`
       |  -> ProgramWord #3: CurlyBracketsGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: ForeachStatement
       |  -> iterable
       |    -> Expression: Symbol: `list`
       |  ~> ERR-327
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
