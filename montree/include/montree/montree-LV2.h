#ifndef MONTREE_LV2_H
#define MONTREE_LV2_H

#include <monlang-LV2/visitors/visitor.h>

namespace montree {
    std::string astToString(LV2::Ast, int TAB_SIZE = 2);

    LV1::Program buildProgram(std::string, int TAB_SIZE = 2);
    ProgramSentence buildProgramSentence(std::string, int TAB_SIZE = 2);
    ProgramWord buildProgramWord(std::string, int TAB_SIZE = 2);

    // inline LV1::Program generateProgram(int seed);
    // inline LV1::Program generateProgram(); // random seed
}

#endif // MONTREE_LV2_H
