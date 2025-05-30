#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/expr/FunctionCall.h>

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

TEST_CASE ("lambda with variadic parameters", "[test-1642][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Association
       |    -> Word: ParenthesesGroup
       |      -> Term #1
       |        -> Word: Atom: `x`
       |      -> Term #2
       |        -> Word: Atom: `args...`
       |    -> Word: CurlyBracketsGroup
       |      -> ProgramSentence
       |        -> ProgramWord: Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Lambda
       |  -> parameter #1: `x`
       |  -> variadic parameters: `args...`
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

    auto expect = tommy_str(R"EOF(
       |-> Expression: FunctionCall
       |  -> function
       |    -> Expression: Symbol: `func`
       |  -> argument #1
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
       |  -> Expression #1: Symbol: `a`
       |  -> Expression #2: Symbol: `b`
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

///////////////////////////////////////////////////////////

TEST_CASE ("field access", "[test-1656][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Path
       |    -> Word: Atom: `a`
       |    -> Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: FieldAccess
       |  -> Expression: Symbol: `a`
       |  -> Symbol: `b`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("subscript with index", "[test-1657][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> ProgramWord: PostfixSquareBracketsGroup
       |    -> Word: Atom: `a`
       |    -> SquareBracketsGroup
       |      -> Term
       |        -> Atom: `#1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Subscript
       |  -> Expression: Symbol: `a`
       |  -> index
       |    -> Expression: Numeral: `1`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("subscript with range", "[test-1658][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> ProgramWord: PostfixSquareBracketsGroup
       |    -> Word: Atom: `a`
       |    -> SquareBracketsGroup
       |      -> Term
       |        -> Atom: `#1..10`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Subscript
       |  -> Expression: Symbol: `a`
       |  -> range
       |    -> Expression: Numeral: `1`
       |    -> Expression: Numeral: `10`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("subscript with exclusive range", "[test-1659][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> ProgramWord: PostfixSquareBracketsGroup
       |    -> Word: Atom: `a`
       |    -> SquareBracketsGroup
       |      -> Term
       |        -> Atom: `#1..<10`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Subscript
       |  -> Expression: Symbol: `a`
       |  -> (exclusive) range
       |    -> Expression: Numeral: `1`
       |    -> Expression: Numeral: `10`
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("subscript with key", "[test-1661][expr]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> ProgramWord: PostfixSquareBracketsGroup
       |    -> Word: Atom: `a`
       |    -> SquareBracketsGroup
       |      -> Term
       |        -> Quotation: `key`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Expression: Subscript
       |  -> Expression: Symbol: `a`
       |  -> key
       |    -> Expression: StrLiteral: `key`
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

TEST_CASE ("Malformed FunctionCallArgument, contains a Malformed Expression", "[test-1625][expr][err]") {
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
       |  ~> argument #1
       |    ~> Expression
       |      ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*FunctionCall*/ "ERR-622" == output.error().fmt);
    REQUIRE (/*FunctionCallArgument*/ "ERR-731" == std::get<MayFail_<FunctionCall>*>(output.val)->arguments.at(0).error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed FunctionCallArgument, contains a non-lvalue passed by reference", "[test-1670][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: PostfixParenthesesGroup
       |    -> Word: Atom: `func`
       |    -> ParenthesesGroup
       |      -> Term
       |        -> PostfixParenthesesGroup
       |          -> Word: Atom: `&a`
       |          -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: FunctionCall
       |  -> function
       |    -> Expression: Symbol: `func`
       |  ~> argument #1 (passed by reference)
       |    -> Expression: FunctionCall
       |      -> function
       |        -> Expression: Symbol: `a`
       |      -> arguments (none)
       |    ~> ERR-732
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
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

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed FieldAccess, contains a malformed object", "[test-1675][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Path
       |    -> Word: ParenthesesGroup (empty)
       |    -> Atom: `x`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: FieldAccess
       |  ~> Expression
       |    ~> ERR-169
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*FieldAccess*/ "ERR-711" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed FieldAccess, contains a non-Symbol as field", "[test-1676][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> Word: Path
       |    -> Word: Atom: `a`
       |    -> Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: FieldAccess
       |  -> Expression: Symbol: `a`
       |  ~> ERR-712
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed Subscript, contains a malformed Expression as array", "[test-1677][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> ProgramWord: PostfixSquareBracketsGroup
       |    -> Word: ParenthesesGroup (empty)
       |    -> SquareBracketsGroup
       |      -> Term
       |        -> Quotation: `key`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: Subscript
       |  ~> Expression
       |    ~> ERR-169
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*Subscript*/ "ERR-721" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("Malformed Subscript, contains a malformed Expression as key", "[test-1677][expr][err]") {
    auto input = tommy_str(R"EOF(
       |-> Term
       |  -> ProgramWord: PostfixSquareBracketsGroup
       |    -> Word: Atom: `arr`
       |    -> SquareBracketsGroup
       |      -> Term
       |        -> ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Expression: Subscript
       |  -> Expression: Symbol: `arr`
       |  ~> key
       |    ~> Expression
       |      ~> ERR-169
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_term = std::get<Term>(input_ast);
    auto output = buildExpression(input_term);
    REQUIRE (/*Subscript*/ "ERR-722" == output.error().fmt);
    auto output_str = montree::astToString(output);

    REQUIRE (output_str == expect);
}
