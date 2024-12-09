#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("lvalue condition", "[test-4411][while]") {
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
       |  -> Expression: Lvalue: `end`
       |  -> BlockExpression
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

TEST_CASE ("grouped expr condition", "[test-4412][while]") {
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
       |  -> Expression: Lvalue: `end`
       |  -> BlockExpression
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

TEST_CASE ("block condition", "[test-4413][while]") {
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
       |  -> Expression: BlockExpression
       |    -> Statement: ExpressionStatement
       |      -> Expression: Lvalue: `end`
       |  -> BlockExpression
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

TEST_CASE ("function call condition", "[test-4414][while]") {
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
       |  -> Expression: FunctionCall
       |    -> function
       |      -> Expression: Lvalue: `end?`
       |    -> arguments
       |      -> Expression: Lvalue: `stream`
       |  -> BlockExpression
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

TEST_CASE ("operation condition", "[test-4415][while]") {
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
       |  -> Expression: Operation
       |    -> Expression: Lvalue: `i`
       |    -> operator: `>`
       |    -> Expression: Literal: `0`
       |  -> BlockExpression
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
