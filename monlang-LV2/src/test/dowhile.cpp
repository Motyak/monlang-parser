#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/stmt/WhileStatement.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>

///////////////////////////////////////////////////////////

TEST_CASE ("symbol condition", "[test-3411][dowhile]") {
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  -> C_WhileStatement
       |    -> Expression: Symbol: `end`
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  -> C_WhileStatement
       |    -> Expression: Symbol: `end`
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  -> C_WhileStatement
       |    -> Expression: BlockExpression
       |      -> Statement: ExpressionStatement
       |        -> Expression: Symbol: `end`
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  -> C_WhileStatement
       |    -> Expression: FunctionCall
       |      -> function
       |        -> Expression: Symbol: `end?`
       |      -> arguments
       |        -> Expression: Symbol: `stream`
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  -> C_WhileStatement
       |    -> Expression: Operation
       |      -> Expression: Symbol: `i`
       |      -> operator: `>`
       |      -> Expression: Numeral: `0`
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

TEST_CASE ("Malformed DoWhileStatement, missing C_WhileStatement", "[test-3431][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: PostfixParenthesesGroup
       |          -> Word: Atom: `doit`
       |          -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
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

TEST_CASE ("Malformed C_DoStatement, contains less than 2 words (no block)", "[test-3416][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord: Atom: `do`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  ~> C_DoStatement
       |    ~> ERR-351
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (/*DoWhileStatement*/output.error().fmt == "ERR-341");
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_DoStatement, contains a non-Word as block", "[test-3417][dowhile][err]") {
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
       |  ~> C_DoStatement
       |    ~> ERR-352
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_DoStatement, contains a non-BlockExpression as block", "[test-3431][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `true`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  ~> C_DoStatement
       |    ~> ERR-353
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_DoStatement, contains a Malformed BlockExpression as block", "[test-3418][dowhile][err]") {
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
       |  ~> C_DoStatement
       |    ~> Statement: ExpressionStatement
       |      ~> Expression
       |        ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (/*C_DoStatement*/ "ERR-354" == std::get<MayFail_<DoWhileStatement>*>(output.val)->doStmt.error().fmt);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_DoStatement, contains a dollars BlockExpression as block", "[test-3433][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup (empty)
    )EOF");
    auto input_ast = montree::buildLV1Ast(input);
    // in montree LV1 we don't differentiate dollars cbg from standard cbg, so...
    std::get<CurlyBracketsGroup*>(std::get<Program>(input_ast).sentences.at(0).programWords.at(1))->_dollars = true;
    auto input_prog = std::get<Program>(input_ast);

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  ~> C_DoStatement
       |    ~> ERR-356
    )EOF");

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_DoStatement, contains a oneline BlockExpression as block", "[test-3432][dowhile][err]") {
    auto input = tommy_str(R"EOF(
       |-> Program
       |  -> ProgramSentence
       |    -> ProgramWord #1: Atom: `do`
       |    -> ProgramWord #2: CurlyBracketsGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: DoWhileStatement
       |  ~> C_DoStatement
       |    ~> ERR-355
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_WhileStatement, contains less than 2 words (no condition)", "[test-3420][dowhile][err]") {
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  ~> C_WhileStatement
       |    ~> ERR-361
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (/*DoWhileStatement*/output.error().fmt == "ERR-343");
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_WhileStatement, contains a non-SquareBracketsTerm as condition", "[test-3421][dowhile][err]") {
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  ~> C_WhileStatement
       |    ~> ERR-362
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed C_WhileStatement, contains a Malformed Expression as part of condition", "[test-3422][dowhile][err]") {
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
       |  -> C_DoStatement
       |    -> Statement: ExpressionStatement
       |      -> Expression: FunctionCall
       |        -> function
       |          -> Expression: Symbol: `doit`
       |        -> arguments (none)
       |  ~> C_WhileStatement
       |    ~> Expression
       |      ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_prog = std::get<Program>(input_ast);

    auto output = consumeStatement(input_prog);
    REQUIRE (/*C_DoStatement*/ "ERR-363" == std::get<MayFail_<DoWhileStatement>*>(output.val)->whileStmt.error().fmt);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
