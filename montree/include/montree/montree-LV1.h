#ifndef MONTREE_LV1_H
#define MONTREE_LV1_H

#include <monlang-LV1/visitors/visitor.h>

namespace montree {
    std::string astToString(LV1::Ast_, int TAB_SIZE = 2);

    LV1::Program buildProgram(std::string, int TAB_SIZE = 2);
    ProgramSentence buildProgramSentence(std::string, int TAB_SIZE = 2);
    ProgramWord buildProgramWord(std::string, int TAB_SIZE = 2);
}

#endif // MONTREE_LV1_H
