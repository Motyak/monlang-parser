#include <utils/tommystring.h>
#include <montree/montree-LV1.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV2/Program.h>

TEST_CASE ("empty program", "[test-1111][base]") {
    auto input = "-> Program";

    auto expect = "-> Program";

    auto input_ast = montree::buildProgram(input);
    auto output = consumeProgram(input_ast);
    auto output_str = montree::astToString(output);
    REQUIRE (output_str == expect);
}
