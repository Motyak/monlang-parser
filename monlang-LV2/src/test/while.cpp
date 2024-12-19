#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("lvalue condition", "[test-3311][while]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `end`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `doit`
       |        -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: WhileStatement
       |  -> condition
       |    -> Expression: Lvalue: `end`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
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

TEST_CASE ("grouped expr condition", "[test-3312][while]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> ParenthesesGroup
       |        -> Term
       |          -> Word: Atom: `end`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `doit`
       |        -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: WhileStatement
       |  -> condition
       |    -> Expression: Lvalue: `end`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
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

TEST_CASE ("block condition", "[test-3313][while]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: CurlyBracketsGroup
       |        -> ProgramSentence
       |          -> ProgramWord: Atom: `end`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `doit`
       |        -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: WhileStatement
       |  -> condition
       |    -> Expression: BlockExpression
       |      -> Statement: ExpressionStatement
       |        -> Expression: Lvalue: `end`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
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

TEST_CASE ("function call condition", "[test-3314][while]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: PostfixParenthesesGroup
       |        -> Word: Atom: `end?`
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `stream`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `doit`
       |        -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: WhileStatement
       |  -> condition
       |    -> Expression: FunctionCall
       |      -> function
       |        -> Expression: Lvalue: `end?`
       |      -> arguments
       |        -> Expression: Lvalue: `stream`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
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

TEST_CASE ("operation condition", "[test-3315][while]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `while`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word #1: Atom: `i`
       |      -> Word #2: Atom: `>`
       |      -> Word #3: Atom: `0`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `doit`
       |        -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: WhileStatement
       |  -> condition
       |    -> Expression: Operation
       |      -> Expression: Lvalue: `i`
       |      -> operator: `>`
       |      -> Expression: Literal: `0`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
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

TEST_CASE ("ERR contains less than 2 words (no condition)", "[test-3316][while][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `while`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: WhileStatement
       |  ~> ERR-331
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

TEST_CASE ("ERR contains a non-SquareBracketsTerm as condition", "[test-3316][while][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: Atom: `end`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: WhileStatement
       |  ~> ERR-332
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

TEST_CASE ("ERR contains a Malformed Expression as part of condition", "[test-3317][while][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word #1: Atom: `a`
       |      -> Word #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: WhileStatement
       |  ~> ERR-333
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

TEST_CASE ("ERR contains less than 3 words (no block)", "[test-3318][while][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `end`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: WhileStatement
       |  -> condition
       |    -> Expression: Lvalue: `end`
       |  ~> ERR-334
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

TEST_CASE ("ERR contains a non-Word as block", "[test-3319][while][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `end`
       |  -> ProgramWord #3: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `true`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: WhileStatement
       |  -> condition
       |    -> Expression: Lvalue: `end`
       |  ~> ERR-335
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

TEST_CASE ("ERR contains a Malformed BlockExpression as block", "[test-3320][while][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `end`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord #1: Atom: `a`
       |      -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: WhileStatement
       |  -> condition
       |    -> Expression: Lvalue: `end`
       |  ~> block
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

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains more than 3 words", "[test-3321][while][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `until`
       |  -> ProgramWord #2: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `end`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: PostfixParenthesesGroup
       |        -> Word: Atom: `doit`
       |        -> ParenthesesGroup (empty)
       |  -> ProgramWord #4: Atom: `;`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: WhileStatement
       |  -> condition
       |    -> Expression: Lvalue: `end`
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  ~> ERR-337
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}