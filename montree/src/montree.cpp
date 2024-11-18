#include "Print.cpp"

#include <montree/montree-LV1.h>
// #include <montree/montree-LV2.h>

#include <monlang-LV1/visitors/visitor.h>
// #include <monlang-LV2/visitors/visitor.h>

#include <iostream>

std::string montree::astToString(Ast_ ast, int TAB_SIZE) {
    std::ostringstream oss;
    visitAst_(Print(oss, TAB_SIZE), ast);
    auto res = oss.str();
    // remove potential trailing newline
    if (!res.empty() && res.back() == NEWLINE) {
        res.pop_back();
    }
    return res;
}
