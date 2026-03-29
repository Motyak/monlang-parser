#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

///////////////////////////////////////////////////////////

TEST_CASE ("define simple type alias", "[test-4111][typedef]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `type`
       |  -> ProgramWord #2: Atom: `UInt`
       |  -> ProgramWord #3: Atom: `Int`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: TypeDefinition
       |  -> type: `UInt`
       |  -> subtype: `Int`
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

TEST_CASE ("define simple type alias", "[test-4112][typedef]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `type`
       |  -> ProgramWord #2: Atom: `MyStr`
       |  -> ProgramWord #3: Atom: `ToStr`
       |  -> ProgramWord #4: Atom: `ToBool`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: TypeDefinition
       |  -> type: `MyStr`
       |  -> subtype #1: `ToStr`
       |  -> subtype #2: `ToBool`
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

TEST_CASE ("ERR contains less than 2 words (no name)", "[test-4141][typedef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `type`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: TypeDefinition
       |  ~> ERR-411
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

TEST_CASE ("ERR contains a non-Symbol as name", "[test-4142][typedef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `type`
       |  -> ProgramWord #2: Atom: `123`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: TypeDefinition
       |  ~> ERR-412
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

TEST_CASE ("ERR contains a non-Atom Symbol as name", "[test-4143][typedef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `type`
       |  -> ProgramWord #2: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `MyType`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: TypeDefinition
       |  ~> ERR-413
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

TEST_CASE ("ERR contains less than 3 words (no subtype)", "[test-4144][typedef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `type`
       |  -> ProgramWord #2: Atom: `MyType`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: TypeDefinition
       |  -> type: `MyType`
       |  ~> ERR-414
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

TEST_CASE ("ERR contains a non-Symbol as part of subtypes", "[test-4145][typedef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `type`
       |  -> ProgramWord #2: Atom: `MyType`
       |  -> ProgramWord #3: Atom: `SomeType`
       |  -> ProgramWord #4: Atom: `123`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: TypeDefinition
       |  -> type: `MyType`
       |  -> subtype #1: `SomeType`
       |  ~> ERR-415
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

TEST_CASE ("ERR contains a non-Atom Symbol as part of subtypes", "[test-4146][typedef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `type`
       |  -> ProgramWord #2: Atom: `MyType`
       |  -> ProgramWord #3: Atom: `SomeType`
       |  -> ProgramWord #4: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `SomeOtherType`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: TypeDefinition
       |  -> type: `MyType`
       |  -> subtype #1: `SomeType`
       |  ~> ERR-416
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
