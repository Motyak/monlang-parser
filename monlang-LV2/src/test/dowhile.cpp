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

TEST_CASE ("Malformed DoStatement, contains less than 2 words (no block)", "[test-3416][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord: Atom: `do`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  ~> ERR-341
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed DoStatement, contains a non-Word as block", "[test-3417][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: SquareBracketsTerm
       |      -> Term
       |        -> Word: Atom: `true`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  ~> ERR-342
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed DoStatement, contains a Malformed BlockExpression as block", "[test-3418][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord #1: Atom: `a`
       |        -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  ~> block
       |    ~> Statement: ExpressionStatement
       |      ~> Expression
       |        ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (/*DoWhileStatement*/ "ERR-343" == output.error().fmt);

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed DoStatement, contains more than 2 words", "[test-3419][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: PostfixParenthesesGroup
       |          -> Word: Atom: `doit`
       |          -> ParenthesesGroup (empty)
       |    -> ProgramWord #3: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  ~> ERR-344
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed DoWhileStatement, contains less than 2 words (no condition)", "[test-3420][dowhile][err]") {
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
       |    -> ProgramWord: Atom: `until`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  ~> ERR-345
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed DoWhileStatement, contains a non-SquareBracketsTerm as condition", "[test-3421][dowhile][err]") {
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
       |    -> ProgramWord #2: Atom: `end`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  ~> ERR-346
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed DoWhileStatement, contains a Malformed Expression as part of condition", "[test-3422][dowhile][err]") {
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
       |        -> Word #1: Atom: `a`
       |        -> Word #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  ~> ERR-347
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed DoWhileStatement, contains more than 2 words", "[test-3423][dowhile][err]") {
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
       |    -> ProgramWord #3: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  -> block
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Lvalue: `doit`
       |        -> arguments (none)
       |  -> condition
       |    -> Expression: Lvalue: `end`
       |  ~> ERR-348
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
