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

TEST_CASE ("comment within ListLiteral", "[test-9112][comments]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup
       |    -> Term #1
       |      -> Word: Atom: `a`
       |    -> Term #2
       |      -> Word #1: Atom: `--`
       |      -> Word #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: ListLiteral
       |  -> Expression: Symbol: `a`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

// TEST_CASE ("comment within MapLiteral", "[test-9113][comments]") {
//     auto input = tommy_str(R"EOF(
//        |-> Term
//        |  -> Word: SquareBracketsGroup
//        |    -> Term #1
//        |      -> Word: Association
//        |        -> Word: Atom: `a`
//        |        -> Word: Atom: `1`
//        |    -> Term #2
//        |      -> Word #1: Atom: `--`
//        |      -> Word #2: Association
//        |        -> Word: Atom: `b`
//        |        -> Word: Atom: `2
//     )EOF");

//     auto expect = tommy_str(R"EOF(
//        |-> Expression: MapLiteral
//        |  -> argument #1
//        |    -> key
//        |      -> Expression: Symbol: `a`
//        |    -> value
//        |      -> Expression: Numeral: `1`
//     )EOF");

//     auto input_ast = montree::buildLV1Ast(input);
//     auto input_term = std::get<Term>(input_ast);
//     auto output = buildExpression(input_term);
//     auto output_str = montree::astToString(output);

//     REQUIRE (output_str == expect);
// }


