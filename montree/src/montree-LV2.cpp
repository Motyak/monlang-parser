#include <montree/montree-LV2.h>

#include <montree/PrintLV2.h>
#include <montree/LV1AstBuilder.h>

#include <iostream>

namespace {
    constexpr char NEWLINE = 10;
}

std::string montree::astToString(LV2::Ast_ ast, int TAB_SIZE) {
    std::ostringstream oss;
    LV2::visitAst_(PrintLV2(oss, TAB_SIZE), ast);
    auto res = oss.str();
    // remove potential trailing newline
    if (!res.empty() && res.back() == NEWLINE) {
        res.pop_back();
    }
    return res;
}

LV1::Ast montree::buildLV1Ast(const std::string& tree) {
    auto builder = LV1AstBuilder(tree);
    return builder.buildAst();
}
