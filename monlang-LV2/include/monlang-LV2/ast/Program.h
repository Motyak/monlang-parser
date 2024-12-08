#ifndef LV2_AST_PROGRAM_H
#define LV2_AST_PROGRAM_H

#include <monlang-LV2/ast/Statement.h>

#include <vector>

namespace LV2
{

struct Program {
    std::vector<Statement> statements;
};

} // LV2::

#endif // LV2_AST_PROGRAM_H
