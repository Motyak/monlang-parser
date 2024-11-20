#ifndef MONTREE_LV1_H
#define MONTREE_LV1_H

#include <monlang-LV1/visitors/visitor.h>

namespace montree {
    std::string astToString(LV1::Ast_, int TAB_SIZE = 2);

    // seed can be generated from time or std::random_device{}()
    LV1::Program generateLV1Program(int seed);
}

#endif // MONTREE_LV1_H
