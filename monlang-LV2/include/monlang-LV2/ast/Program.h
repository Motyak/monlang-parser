#ifndef LV2_AST_PROGRAM_H
#define LV2_AST_PROGRAM_H

#include <monlang-LV2/ast/Statement.h>

#include <vector>

namespace LV2
{

struct Program {
    std::vector<Statement> statements;

    size_t _tokenId = 123456789;
    Program() = default;
    Program(const std::vector<Statement>&);
};

} // LV2::

#endif // LV2_AST_PROGRAM_H
