#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV1/CurlyBracketsGroup.h>

///////////////////////////////////////////////////////////

TEST_CASE ("define struct", "[test-4211][structdef]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `Person`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `Str`
       |      -> ProgramWord #2: Atom: `name`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `UInt`
       |      -> ProgramWord #2: Atom: `age`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |-> Statement: StructDefinition
       |  -> name: `Person`
       |  -> field #1
       |    -> type: `Str`
       |    -> name: `name`
       |  -> field #2
       |    -> type: `UInt`
       |    -> name: `age`
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

TEST_CASE ("ERR contains less than 2 words (no name)", "[test-4241][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord: Atom: `struct`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  ~> ERR-421
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

TEST_CASE ("ERR contains a non-Symbol as name", "[test-4242][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `123`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  ~> ERR-422
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

TEST_CASE ("ERR contains a non-Atom Symbol as name", "[test-4243][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: ParenthesesGroup
       |    -> Term
       |      -> Word: Atom: `MyStruct`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  ~> ERR-423
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

TEST_CASE ("ERR contains less than 3 words (no block)", "[test-4244][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  ~> ERR-424
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

TEST_CASE ("ERR contains a non-block", "[test-4245][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
       |  -> ProgramWord #3: ParenthesesGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  ~> ERR-425
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

TEST_CASE ("ERR contains a oneline block", "[test-4246][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
       |  -> ProgramWord #3: CurlyBracketsGroup (empty)
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  ~> ERR-426
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

TEST_CASE ("ERR field contains a non-Symbol as type", "[test-4247][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `Int`
       |      -> ProgramWord #2: Atom: `a`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `123`
       |      -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  -> field #1
       |    -> type: `Int`
       |    -> name: `a`
       |  ~> field #2
       |    ~> ERR-431
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-427");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR field contains a non-Atom Symbol as type", "[test-4248][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `Int`
       |      -> ProgramWord #2: Atom: `a`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: ParenthesesGroup
       |        -> Term
       |          -> Word: Atom: `Int`
       |      -> ProgramWord #2: Atom: `b`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  -> field #1
       |    -> type: `Int`
       |    -> name: `a`
       |  ~> field #2
       |    ~> ERR-432
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-427");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR field contains less than 2 words (no name)", "[test-4249][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `Int`
       |      -> ProgramWord #2: Atom: `a`
       |    -> ProgramSentence #2
       |      -> ProgramWord: Atom: `Int`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  -> field #1
       |    -> type: `Int`
       |    -> name: `a`
       |  ~> field #2
       |    -> type: `Int`
       |    ~> ERR-433
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-427");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR field contains a non-Symbol as name", "[test-4251][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `Int`
       |      -> ProgramWord #2: Atom: `a`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `Int`
       |      -> ProgramWord #2: Atom: `123`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  -> field #1
       |    -> type: `Int`
       |    -> name: `a`
       |  ~> field #2
       |    -> type: `Int`
       |    ~> ERR-434
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-427");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}

///////////////////////////////////////////////////////////

TEST_CASE ("ERR field contains a non-Atom Symbol as name", "[test-4252][structdef][err]") {
    auto input = tommy_str(R"EOF(
       |-> ProgramSentence
       |  -> ProgramWord #1: Atom: `struct`
       |  -> ProgramWord #2: Atom: `MyStruct`
       |  -> ProgramWord #3: CurlyBracketsGroup
       |    -> ProgramSentence #1
       |      -> ProgramWord #1: Atom: `Int`
       |      -> ProgramWord #2: Atom: `a`
       |    -> ProgramSentence #2
       |      -> ProgramWord #1: Atom: `Int`
       |      -> ProgramWord #2: ParenthesesGroup
       |        -> Term
       |          -> Word: Atom: `Int`
    )EOF");

    auto expect = tommy_str(R"EOF(
       |~> Statement: StructDefinition
       |  -> name: `MyStruct`
       |  -> field #1
       |    -> type: `Int`
       |    -> name: `a`
       |  ~> field #2
       |    -> type: `Int`
       |    ~> ERR-435
    )EOF");

    auto input_ast = montree::buildLV1Ast(input);
    auto input_sentence = std::get<ProgramSentence>(input_ast);
    auto input_prog = LV1::Program{{input_sentence}};

    auto output = consumeStatement(input_prog);
    REQUIRE (input_prog.sentences.empty());
    REQUIRE (output.error().fmt == "ERR-427");

    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
