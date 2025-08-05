#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("int numeral", "[test-6500][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `-123'456'789`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `-123'456'789`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("decimal numeral with fixed dec part only", "[test-6501][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Path
       |    -> Word: Atom: `-123'456`
       |    -> Atom: `789`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `-123'456.789`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("decimal numeral with periodic dec part only", "[test-6502][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: Path
       |      -> Word: Atom: `-123'456`
       |      -> Atom: `'`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `789`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `-123'456.'(789)`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("decimal numeral with both fixed and periodic dec parts", "[test-6503][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: Path
       |      -> Word: Atom: `-123'456`
       |      -> Atom: `7`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `89`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `-123'456.7(89)`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("fraction numeral", "[test-6504][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `-123'456/789`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `-123'456/789`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("power numeral", "[test-6505][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `-123'456^789`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `-123'456^789`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("hex numeral", "[test-6506][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `0xF9'0F`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `0xF9'0F`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("octal numeral", "[test-6507][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `0o755'700`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `0o755'700`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("binary numeral", "[test-6508][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `0b1001'1111`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Numeral: `0b1001'1111`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("decimal numeral with periodic part as function in function call", "[test-6509][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: PostfixParenthesesGroup
       |      -> Word: Path
       |        -> Word: Atom: `-123'456`
       |        -> Atom: `7`
       |      -> ParenthesesGroup
       |        -> Term
       |          -> Word: Atom: `89`
       |    -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: FunctionCall
       |  -> function
       |    -> Expression: Numeral: `-123'456.7(89)`
       |  -> arguments (none)
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("decimal numeral withOUT periodic part as function in function call", "[test-6510][num]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: Path
       |      -> Word: Atom: `0`
       |      -> Atom: `3`
       |    -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: FunctionCall
       |  -> function
       |    -> Expression: Numeral: `0.3`
       |  -> arguments (none)
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
