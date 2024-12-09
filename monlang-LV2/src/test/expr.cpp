#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("lvalue from atom", "[test-2111][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Expression: Lvalue: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("special symbol from atom", "[test-2131][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `$1`
    )EOF");

    auto expect = "-> Expression: SpecialSymbol: `$1`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("grouped expression => ungroup", "[test-2112][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Expression: Lvalue: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("literal from atom", "[test-2113][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `91`
    )EOF");

    auto expect = "-> Expression: Literal: `91`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("block from curly brackets group", "[test-2114][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: BlockExpression
       |  -> Statement: ExpressionStatement
       |    -> Expression: Literal: `91`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("lambda from pg-cbg association", "[test-2115][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Association
       |    -> Word: ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `x`
       |    -> Word: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: Atom: `x`
    )EOF");

    /* TODO: will make '-> parameter #1 #2 #...' when multiple parameters */
    auto expect = tommy_str(R"EOF(
       |-> Expression: Lambda
       |  -> parameter #1: `x`
       |  -> body
       |    -> Statement: ExpressionStatement
       |      -> Expression: Lvalue: `x`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("function call from postfix parentheses group", "[test-2116][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: Atom: `func`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `arg`
    )EOF");

    /* TODO: Lvalue will be able to contain indent, later on */
    /* TODO: will be able to add Lvalue #1, #2, #.. if multiple args */
    auto expect = tommy_str(R"EOF(
       |-> Expression: FunctionCall
       |  -> function
       |    -> Expression: Lvalue: `func`
       |  -> arguments
       |    -> Expression: Lvalue: `arg`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("operation from term of atoms", "[test-2117][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: Atom: `1`
       |  -> Word #2: Atom: `+`
       |  -> Word #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Operation
       |  -> Expression: Literal: `1`
       |  -> operator: `+`
       |  -> Expression: Literal: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}
