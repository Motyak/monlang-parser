#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/context_init.h>

///////////////////////////////////////////////////////////

TEST_CASE ("lvalue from atom", "[test-2111][rvalue]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Rvalue: Lvalue: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("grouped rvalue => ungroup", "[test-2112][rvalue]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Rvalue: Lvalue: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("literal from atom", "[test-2113][rvalue]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `91`
    )EOF");

    auto expect = "-> Rvalue: Literal: `91`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("block from curly brackets group", "[test-2114][rvalue]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Rvalue: BlockRvalue
       |  -> Statement: RvalueStatement
       |    -> Rvalue: Literal: `91`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("lambda from pg-cbg association", "[test-2115][rvalue]") {
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
       |-> Rvalue: Lambda
       |  -> parameter: `x`
       |  -> body
       |    -> Statement: RvalueStatement
       |      -> Rvalue: Lvalue: `x`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("lambda from pg-cbg association", "[test-2116][rvalue]") {
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
       |-> Rvalue: FunctionCall
       |  -> function
       |    -> Rvalue: Lvalue: `func`
       |  -> arguments
       |    -> Rvalue: Lvalue: `arg`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto context = context_init_t{};
    auto output = buildRvalue(input_term, context);
    auto output_str = montree::astToString(output);

    REQUIRE (!context.fallthrough); // no err
    REQUIRE (output_str == expect);
}
