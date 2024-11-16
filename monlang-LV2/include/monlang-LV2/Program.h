#ifndef LV2_PROGRAM_H
#define LV2_PROGRAM_H

#include <monlang-LV2/Statement.h>

#include <vector>

namespace LV2
{

struct Program {
    std::vector<Statement> statements;
};

} // LV2::

LV2::Program consumeProgram(LV1::Program&);

#endif // LV2_PROGRAM_H
