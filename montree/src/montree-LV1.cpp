#include <montree/montree-LV1.h>

#include <montree/PrintLV1.h>
#include <montree/LV1AstGenerator.h>

#include <iostream>

std::string montree::astToString(LV1::Ast_ ast, int TAB_SIZE) {
    std::ostringstream oss;
    visitAst_(PrintLV1(oss, TAB_SIZE), ast);
    auto res = oss.str();
    // remove potential trailing newline
    if (!res.empty() && res.back() == NEWLINE) {
        res.pop_back();
    }
    return res;
}

LV1::Program generateLV1Program(int seed) {
    (void)seed;
    TODO();
}
