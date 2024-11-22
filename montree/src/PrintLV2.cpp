#include <montree/PrintLV2.h>

void PrintLV2::operator()(const LV2::Program& prog) {
    out << "-> Program" << "\n";
    operator()(prog.statements.at(0));
}

void PrintLV2::operator()(const Statement& statement) {
    out << "-> Statement" << "\n";
    std::visit(*this, statement);
}

void PrintLV2::operator()(const RvalueStatement& rvalueStatement) {
    out << "-> RvalueStatement" << "\n";
    operator()(rvalueStatement.rvalue);
}

void PrintLV2::operator()(const Rvalue& rvalue) {
    std::visit(*this, rvalue);
}

void PrintLV2::operator()(const Lvalue& lvalue) {
    out << "-> Lvalue: `" << lvalue.identifier << "`\n";
}

void PrintLV2::operator()(auto) {
    out << "<ENTITY NOT IMPLEMENTED YET>\n";
}

///////////////////////////////////////////////////////////

PrintLV2::PrintLV2(std::ostream& os, int TAB_SIZE) : TAB_SIZE(TAB_SIZE), out(os){}
