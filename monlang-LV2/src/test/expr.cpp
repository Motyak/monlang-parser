#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/context_init.h>

///////////////////////////////////////////////////////////

TEST_CASE ("lvalue from atom", "[test-2111][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Expression: Lvalue: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = buildExpression(input_term, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
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
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = buildExpression(input_term, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
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
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = buildExpression(input_term, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
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
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = buildExpression(input_term, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
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
       |  -> parameter: `x`
       |  -> body
       |    -> Statement: ExpressionStatement
       |      -> Expression: Lvalue: `x`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = buildExpression(input_term, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
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
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = buildExpression(input_term, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
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
       |  -> leftOperand
       |    -> Expression: Literal: `1`
       |  -> operator: `+`
       |  -> rightOperand
       |    -> Expression: Literal: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto cx_init = context_init_t{};
    auto cx = (context_t)cx_init;
    auto output = buildExpression(input_term, &cx);
    auto output_str = montree::astToString(output);

    REQUIRE (!*cx.fallthrough); // no err
    REQUIRE (output_str == expect);
}
