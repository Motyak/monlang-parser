#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("symbol from atom", "[test-1611][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Expression: Symbol: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("special symbol from atom", "[test-1631][expr]") {
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

TEST_CASE ("grouped expression => ungroup", "[test-1612][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `somevar`
    )EOF");

    auto expect = "-> Expression: Symbol: `somevar`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);

    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("numeral from atom", "[test-1613][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `91`
    )EOF");

    auto expect = "-> Expression: Numeral: `91`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("str literal from quotation", "[test-1651][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Quotation: `hello`
    )EOF");

    auto expect = "-> Expression: StrLiteral: `hello`";

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("block from curly brackets group", "[test-1614][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord: Atom: `91`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: BlockExpression
       |  -> Statement: ExpressionStatement
       |    -> Expression: Numeral: `91`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("lambda from pg-cbg association", "[test-1615][expr]") {
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

    auto expect = tommy_str(R"EOF(
       |-> Expression: Lambda
       |  -> parameter #1: `x`
       |  -> body
       |    -> Statement: ExpressionStatement
       |      -> Expression: Symbol: `x`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("lambda with an OUT parameter", "[test-1641][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Association
       |    -> Word: ParenthesesGroup
       |      -> Term
       |        -> Word #1: Atom: `OUT`
       |        -> Word #2: Atom: `x`
       |    -> Word: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Lambda
       |  -> parameter #1: `x`
       |  -> body
       |    -> Statement: ExpressionStatement
       |      -> Expression: Symbol: `x`
    )EOF");

   auto input_ast = montree::buildLV1Ast(input);
   auto input_term = std::get<Term>(input_ast);
   auto output = buildExpression(input_term);
   auto output_str = montree::astToString(output);

   REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("function call from postfix parentheses group", "[test-1616][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: Atom: `func`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word: Atom: `arg`
    )EOF");

    /* TODO: Symbol will be able to contain indent, later on */
    auto expect = tommy_str(R"EOF(
       |-> Expression: FunctionCall
       |  -> function
       |    -> Expression: Symbol: `func`
       |  -> arguments
       |    -> Expression: Symbol: `arg`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("operation from term of atoms", "[test-1617][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: Atom: `1`
       |  -> Word #2: Atom: `+`
       |  -> Word #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Operation
       |  -> Expression: Numeral: `1`
       |  -> operator: `+`
       |  -> Expression: Numeral: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("empty list literal", "[test-1652][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: ListLiteral (empty)
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("list literal", "[test-1653][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup
       |    -> Term #1
       |      -> Word: Atom: `a`
       |    -> Term #2
       |      -> Word: Atom: `b`
    )EOF");
    auto expect = tommy_str(R"EOF(
       |-> Expression: ListLiteral
       |  -> Expression: Symbol: `a`
       |  -> Expression: Symbol: `b`
    )EOF");
    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("empty map literal", "[test-1654][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Atom: `[:]`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: MapLiteral (empty)
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("map literal", "[test-1655][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup
       |    -> Term #1
       |      -> Word: Association
       |        -> Word: Atom: `a`
       |        -> Word: Atom: `1`
       |    -> Term #2
       |      -> Word: Association
       |        -> Word: Atom: `b`
       |        -> Word: Atom: `2
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: MapLiteral
       |  -> argument #1
       |    -> key
       |      -> Expression: Symbol: `a`
       |    -> value
       |      -> Expression: Numeral: `1`
       |  -> argument #2
       |    -> key
       |      -> Expression: Symbol: `b`
       |    -> value
       |      -> Expression: Numeral: `2`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

//==============================================================
// ERR
//==============================================================

TEST_CASE ("ERR contains an even number of words", "[test-1618][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: Atom: `1`
       |  -> Word #2: Atom: `+`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression
       |  ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains a non-Atom word as operator", "[test-1619][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: Atom: `1`
       |  -> Word #2: ParenthesesGroup (empty)
       |  -> Word #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression
       |  ~> ERR-162
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR contains an unknown operator (unfound in precedence table)", "[test-1620][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: Atom: `1`
       |  -> Word #2: Atom: `#`
       |  -> Word #2: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression
       |  ~> ERR-163
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR Unknown Expression", "[test-1621][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Association
       |    -> Word: Atom: `a`
       |    -> Word: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression
       |  ~> ERR-169
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed Operation, contains a Malformed Expression as LEFT operand", "[test-1622][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: ParenthesesGroup
       |    -> Term
       |      -> Word #1: Atom: `a`
       |      -> Word #2: Atom: `b`
       |  -> Word #2: Atom: `+`
       |  -> Word #2: Atom: `c`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: Operation
       |  ~> Expression
       |    ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (output.error().fmt == "ERR-611");
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed Operation, contains a Malformed Expression as RIGHT operand", "[test-1623][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word #1: Atom: `a`
       |  -> Word #2: Atom: `+`
       |  -> Word #3: ParenthesesGroup
       |    -> Term
       |      -> Word #1: Atom: `b`
       |      -> Word #2: Atom: `c`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: Operation
       |  -> Expression: Symbol: `a`
       |  -> operator: `+`
       |  ~> Expression
       |    ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (output.error().fmt == "ERR-612");
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed FunctionCall, contains a Malformed Expression as FUNCTION", "[test-1624][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: ParenthesesGroup
       |      -> Term
       |        -> Word #1: Atom: `a`
       |        -> Word #2: Atom: `b`
       |    -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: FunctionCall
       |  ~> function
       |    ~> Expression
       |      ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (output.error().fmt == "ERR-621");
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed FunctionCall, contains a Malformed Expression as ARGUMENT", "[test-1625][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: Atom: `func`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> Word #1: Atom: `a`
       |        -> Word #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: FunctionCall
       |  -> function
       |    -> Expression: Symbol: `func`
       |  ~> arguments
       |    ~> Expression
       |      ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*FunctionCall*/ "ERR-622" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed Lambda, contains a Malformed Statement within body", "[test-1626][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Association
       |    -> Word: ParenthesesGroup (empty)
       |    -> Word: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord #1: Atom: `a`
       |        -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: Lambda
       |  -> (no parameters)
       |  ~> body
       |    ~> Statement: ExpressionStatement
       |      ~> Expression
       |        ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*Lambda*/ "ERR-631" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed BlockExpression, contains a Malformed Statement", "[test-1627][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: CurlyBracketsGroup
       |    -> ProgramSentence
       |      -> ProgramWord #1: Atom: `a`
       |      -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: BlockExpression
       |  ~> Statement: ExpressionStatement
       |    ~> Expression
       |      ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*BlockExpression*/ "ERR-641" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed ListLiteral, contains a Malformed Expression", "[test-1671][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup
       |    -> Term
       |      -> Word #1: Atom: `a`
       |      -> Word #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: ListLiteral
       |  ~> Expression
       |    ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*ListLiteral*/ "ERR-681" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed MapLiteral, contains a List argument", "[test-1672][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup
       |    -> Term #1
       |      -> Word: Association
       |        -> Word: Atom: `a`
       |        -> Word: Atom: `1`
       |    -> Term #2:
       |      -> Word: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: MapLiteral
       |  -> argument #1
       |    -> key
       |      -> Expression: Symbol: `a`
       |    -> value
       |      -> Expression: Numeral: `1`
       |  ~> argument #2
       |    ~> ERR-691
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed MapLiteral, contains a malformed Map argument (key part)", "[test-1673][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup
       |    -> Term
       |      -> Word: Association
       |        -> Word: ParenthesesGroup (empty)
       |        -> Word: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: MapLiteral
       |  ~> argument #1
       |    ~> key
       |      ~> Expression
       |        ~> ERR-169
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*MapLiteral*/ "ERR-692" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed MapLiteral, contains a malformed Map argument (value part)", "[test-1674][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: SquareBracketsGroup
       |    -> Term
       |      -> Word: Association
       |        -> Word: Atom: `a`
       |        -> Word: ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: MapLiteral
       |  ~> argument #1
       |    -> key
       |      -> Expression: Symbol: `a`
       |    ~> value
       |      ~> Expression
       |        ~> ERR-169
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*MapLiteral*/ "ERR-693" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}
