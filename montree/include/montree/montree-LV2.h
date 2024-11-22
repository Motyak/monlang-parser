#ifndef MONTREE_LV2_H
#define MONTREE_LV2_H

#include <monlang-LV2/visitors/visitor.h>
#include <monlang-LV1/ast/visitors/visitor.h>

namespace montree {
    std::string astToString(LV2::Ast, int TAB_SIZE = 2);

    LV1::Ast buildLV1Ast(const std::string& tree);
}

#endif // MONTREE_LV2_H
