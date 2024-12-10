#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("lvalue condition", "[test-3411][dowhile]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence #1
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: PostfixParenthesesGroup
       |          -> Word: Atom: `doit`
       |          -> ParenthesesGroup (empty)
       |  -> ProgramSentence #2
       |    -> ProgramWord #1: Atom: `until`
       |    -> ProgramWord #2: SquareBracketsTerm
       |      -> Term
       |        -> Word: Atom: `end`

    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  -> condition
       |    -> Expression: Lvalue: `end`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("grouped expr condition", "[test-3412][dowhile]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence #1
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: PostfixParenthesesGroup
       |          -> Word: Atom: `doit`
       |          -> ParenthesesGroup (empty)
       |  -> ProgramSentence #2
       |    -> ProgramWord #1: Atom: `until`
       |    -> ProgramWord #2: SquareBracketsTerm
       |      -> Term
       |        -> ParenthesesGroup
       |          -> Term
       |            -> Word: Atom: `end`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  -> condition
       |    -> Expression: Lvalue: `end`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("block condition", "[test-3413][dowhile]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence #1
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: PostfixParenthesesGroup
       |          -> Word: Atom: `doit`
       |          -> ParenthesesGroup (empty)
       |  -> ProgramSentence #2
       |    -> ProgramWord #1: Atom: `until`
       |    -> ProgramWord #2: SquareBracketsTerm
       |      -> Term
       |        -> Word: CurlyBracketsGroup
       |          -> ProgramSentence
       |            -> ProgramWord: Atom: `end`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  -> condition
       |    -> Expression: BlockExpression
       |      -> Statement: ExpressionStatement
       |        -> Expression: Lvalue: `end`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("function call condition", "[test-3414][dowhile]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence #1
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: PostfixParenthesesGroup
       |          -> Word: Atom: `doit`
       |          -> ParenthesesGroup (empty)
       |  -> ProgramSentence #2
       |    -> ProgramWord #1: Atom: `until`
       |    -> ProgramWord #2: SquareBracketsTerm
       |      -> Term
       |        -> Word: PostfixParenthesesGroup
       |          -> Word: Atom: `end?`
       |          -> ParenthesesGroup
       |            -> Term
       |              -> Word: Atom: `stream`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  -> condition
       |    -> Expression: FunctionCall
       |      -> function
       |        -> Expression: Lvalue: `end?`
       |      -> arguments
       |        -> Expression: Lvalue: `stream`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("operation condition", "[test-3415][dowhile]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence #1
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: PostfixParenthesesGroup
       |          -> Word: Atom: `doit`
       |          -> ParenthesesGroup (empty)
       |  -> ProgramSentence #2
       |    -> ProgramWord #1: Atom: `until`
       |    -> ProgramWord #2: SquareBracketsTerm
       |      -> Term
       |        -> Word #1: Atom: `i`
       |        -> Word #2: Atom: `>`
       |        -> Word #3: Atom: `0`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  -> condition
       |    -> Expression: Operation
       |      -> Expression: Lvalue: `i`
       |      -> operator: `>`
       |      -> Expression: Literal: `0`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

//==============================================================
// ERR
//==============================================================


