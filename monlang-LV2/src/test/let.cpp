#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("alias to symbol", "[test-2311][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `somealias`
       |  -> ProgramWord #3: Atom: `somevar`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> Symbol: `somealias`
       |  -> Lvalue: Symbol: `somevar`
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

TEST_CASE ("alias to subscript", "[test-2312][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `somealias`
       |  -> ProgramWord #3: PostfixSquareBracketsGroup
       |    -> Word: Atom: `somearr`
       |    -> SquareBracketsGroup
       |      -> Term
       |        -> Atom: `#nth`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> Symbol: `somealias`
       |  -> Lvalue: Subscript
       |    -> Expression: Symbol: `somearr`
       |    -> index
       |      -> Expression: Symbol: `nth`
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

TEST_CASE ("alias to field access", "[test-2313][let]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `somealias`
       |  -> ProgramWord #3: Path
       |    -> Word: Atom: `a`
       |    -> Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: LetStatement
       |  -> Symbol: `somealias`
       |  -> Lvalue: FieldAccess
       |    -> Expression: Symbol: `a`
       |    -> Symbol: `b`
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

TEST_CASE ("ERR contains less than 2 words (no alias)", "[test-2317][let][err]") {
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

TEST_CASE ("ERR contains a non-Word as alias", "[test-2331][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `let`
       |  -> ProgramWord: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `fds`
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

TEST_CASE ("ERR contains a non-Symbol as alias", "[test-2318][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
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

TEST_CASE ("ERR contains less than 3 words (no variable)", "[test-2319][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `somealias`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  -> Symbol: `somealias`
       |  ~> ERR-235
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

TEST_CASE ("ERR contains a non-Word as part of the variable", "[test-2321][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `somealias`
       |  -> ProgramWord #3: SquareBracketsTerm
       |    -> Term
       |      -> Word: Atom: `fds`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  -> Symbol: `somealias`
       |  ~> ERR-236
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

TEST_CASE ("ERR contains a non-Lvalue expression as variable", "[test-2322][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `somealias`
       |  -> ProgramWord #3: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `somevar`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  -> Symbol: `somealias`
       |  ~> ERR-237
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

TEST_CASE ("ERR contains a Malformed Lvalue as variable", "[test-2323][let][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `let`
       |  -> ProgramWord #2: Atom: `somealias`
       |  -> ProgramWord #3: PostfixSquareBracketsGroup
       |    -> Word: Atom: `somemap`
       |    -> SquareBracketsGroup:
       |      -> Term
       |        -> Word: ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: LetStatement
       |  -> Symbol: `somealias`
       |  ~> Lvalue: Subscript
       |    -> Expression: Symbol: `somemap`
       |    ~> key
       |      ~> Expression
       |        ~> ERR-169
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (output.error().fmt == "ERR-238");
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
