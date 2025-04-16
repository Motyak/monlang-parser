#include <utils/tommystring.h>
#include <montree/montree-LV2.h>
#include <catch2/catch_amalgamated.hpp>

#include <monlang-LV1/Program.h>

////////////////////////////////////////////////////////////////

TEST_CASE ("fds") {
    auto input = tommy_str(R"EOF(
       |((-0.4)) + -0.3(4)(5)
       |
    )EOF");

    auto expect = "whatever";

    auto input_iss = std::istringstream(input);
    auto output_LV1 = (LV1::Program)consumeProgram(input_iss);
    auto output_LV2 = consumeProgram(output_LV1);
    auto output_str = montree::astToString(output_LV2);
    REQUIRE (output_str == expect);

    REQUIRE (true);
}
