#include <montree/montree-LV1.h>
#include <montree/montree-LV2.h>

#include <montree/PrintLV1.h>
#include <montree/PrintLV2.h>
#include <montree/LV1AstGenerator.h>
#include <montree/LV1AstBuilder.h>

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

std::string montree::astToString(LV2::Ast ast, int TAB_SIZE) {
    std::ostringstream oss;
    LV2::visitAst(PrintLV2(oss, TAB_SIZE), ast);
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
