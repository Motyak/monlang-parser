#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("comment as ExpressionStatement", "[test-9111][comments]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `--`
       |  -> ProgramWord #2: Atom: `somevar`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: ExpressionStatement (empty)
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

TEST_CASE ("comment within multiline ListLiteral", "[test-9112][comments]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: MultilineSquareBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord: Atom: `a`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `--`
       |      -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: ListLiteral
       |  -> Expression #1: Symbol: `a`
       |  -> Expression #2 (empty argument)
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("comment within MapLiteral", "[test-9113][comments]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: MultilineSquareBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `a`
       |      -> ProgramWord #2: Atom: `=>`
       |      -> ProgramWord #3: Atom: `1`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `--`
       |      -> ProgramWord #2: Atom: `b`
       |      -> ProgramWord #3: Atom: `=>`
       |      -> ProgramWord #4: Atom: `2`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: MapLiteral
       |  -> argument #1
       |    -> key
       |      -> Expression: Symbol: `a`
       |    -> value
       |      -> Expression: Numeral: `1`
       |  -> argument #2 (empty)
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("expression side comment", "[test-9114][comments]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: Atom: `123`
       |  -> Word #2: Atom: `--`
       |  -> Word #3: Atom: `some`
       |  -> Word #4: Atom: `comment`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `123`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("can't generate empty expression", "[test-9115][comments]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `--`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Symbol: `--`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("comment within EnumDefinition", "[test-9116][comments]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `Color`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `--`
       |      -> ProgramWord #2: Atom: `RED`
       |      -> ProgramWord #3: Atom: `=`
       |      -> ProgramWord #4: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `GREEN`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: EnumDefinition
       |  -> name: `Color`
       |  -> enum value #1 (empty)
       |  -> enum value #2
       |    -> name: `GREEN`
       |    -> enumerate
       |      -> Expression: Numeral: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
