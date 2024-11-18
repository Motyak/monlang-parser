#include <montree/PrintLV2.h>

void PrintLV2::operator()(const LV2::Program& prog) {
    // todo
}

void PrintLV2::operator()(const Statement& statement) {
    // todo
}

void PrintLV2::operator()(const Rvalue& rvalue) {
    // todo
}

void PrintLV2::operator()(auto) {

}

///////////////////////////////////////////////////////////

PrintLV2::PrintLV2(std::ostream& os, int TAB_SIZE) : TAB_SIZE(TAB_SIZE), out(os){}
