#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV1/CurlyBracketsGroup.h>
#include <monlang-LV2/stmt/EnumDefinition.h>

///////////////////////////////////////////////////////////

TEST_CASE ("define enum", "[test-4411][enumdef]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `Color`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `RED`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `GREEN`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: EnumDefinition
       |  -> name: `Color`
       |  -> enum value #1
       |    -> name: `RED`
       |    -> enumerate
       |      -> Expression: Numeral: `0`
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

//==============================================================
// ERR
//==============================================================

TEST_CASE ("ERR contains less than 2 words (no name)", "[test-4412][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `enum`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  ~> ERR-441
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

TEST_CASE ("ERR contains a non-Symbol as name", "[test-4413][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `123`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  ~> ERR-442
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

TEST_CASE ("ERR contains a non-Atom Symbol as name", "[test-4414][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `MyEnum`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  ~> ERR-443
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

TEST_CASE ("ERR contains less than 3 words (no block)", "[test-4415][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  ~> ERR-444
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

TEST_CASE ("ERR contains a non-block", "[test-4416][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  ~> ERR-445
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

TEST_CASE ("ERR contains a oneline block", "[test-4417][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: CurlyBracketsGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  ~> ERR-446
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    std::get<CurlyBracketsGroup*>(input_sentence.programWords.at(2))->term = Term{};
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR enum value contains a non-Symbol as name", "[test-4418][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `RED`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `123`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  -> enum value #1
       |    -> name: `RED`
       |    -> enumerate
       |      -> Expression: Numeral: `0`
       |  ~> enum value #2
       |    ~> ERR-451
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-447");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR enum value contains a non-Atom Symbol as type", "[test-4419][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `RED`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: ParenthesesGroup
       |        -> Term
       |          -> Atom: `BLUE`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  -> enum value #1
       |    -> name: `RED`
       |    -> enumerate
       |      -> Expression: Numeral: `0`
       |  ~> enum value #2
       |    ~> ERR-452
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-447");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR enum value contains less than 2 words (no equal sign)", "[test-4421][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `RED`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `BLUE`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  -> enum value #1
       |    -> name: `RED`
       |    -> enumerate
       |      -> Expression: Numeral: `0`
       |  ~> enum value #2
       |    -> name: `BLUE`
       |    ~> ERR-453
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-447");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR enum value missing equal sign", "[test-4422][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `RED`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `BLUE`
       |      -> ProgramWord #2: Atom: `1`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  -> enum value #1
       |    -> name: `RED`
       |    -> enumerate
       |      -> Expression: Numeral: `0`
       |  ~> enum value #2
       |    -> name: `BLUE`
       |    ~> ERR-454
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-447");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR enum value contains less than 3 words (no enumerate)", "[test-4423][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `RED`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `BLUE`
       |      -> ProgramWord #2: Atom: `=`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  -> enum value #1
       |    -> name: `RED`
       |    -> enumerate
       |      -> Expression: Numeral: `0`
       |  ~> enum value #2
       |    -> name: `BLUE`
       |    ~> ERR-455
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-447");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR enum value contains a Malformed Expression as enumerate", "[test-4424][enumdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `enum`
       |  -> ProgramWord #2: Atom: `MyEnum`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `RED`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `0`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `BLUE`
       |      -> ProgramWord #2: Atom: `=`
       |      -> ProgramWord #3: Atom: `1`
       |      -> ProgramWord #4: Atom: `+`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: EnumDefinition
       |  -> name: `MyEnum`
       |  -> enum value #1
       |    -> name: `RED`
       |    -> enumerate
       |      -> Expression: Numeral: `0`
       |  ~> enum value #2
       |    -> name: `BLUE`
       |    ~> enumerate
       |      ~> Expression
       |        ~> ERR-161
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-447");
    REQUIRE (std::get<MayFail_<EnumDefinition>*>(output.val)->enumValues.back().error().fmt == "ERR-456");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
