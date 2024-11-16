#ifndef LV2_PROGRAM_H
#define LV2_PROGRAM_H

#include <monlang-LV2/Statement.h>
#include <monlang-LV2/context.h>

#include <monlang-LV1/Program.h>

#include <vector>

namespace LV2
{

struct Program {
    std::vector<Statement> statements;
};

} // LV2::

LV2::Program consumeProgram(LV1::Program&, const context_t& = context_t{});

#endif // LV2_PROGRAM_H
