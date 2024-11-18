#include "PrintLV1.cpp"
#include "PrintLV2.cpp"

#include <montree/montree-LV1.h>
#include <montree/montree-LV2.h>

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

LV1::Program montree::buildProgram(std::string str, int TAB_SIZE) {
    //todo
    return LV1::Program{};
}

ProgramSentence montree::buildProgramSentence(std::string str, int TAB_SIZE) {
    // todo
    return ProgramSentence{};
}

ProgramWord montree::buildProgramWord(std::string str, int TAB_SIZE) {
    // todo
    return ProgramWord{};
}
