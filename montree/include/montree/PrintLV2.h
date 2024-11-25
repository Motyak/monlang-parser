#ifndef PRINT_LV2_H
#define PRINT_LV2_H

#include <monlang-LV2/RvalueStatement.h>
#include <monlang-LV2/BlockRvalue.h>
#include <monlang-LV2/Literal.h>
#include <monlang-LV2/Lvalue.h>

/* interface only */
#include <monlang-LV2/visitors/visitor.h>

class PrintLV2 : public LV2::AstVisitor<void> {
  public:
    PrintLV2(std::ostream&, int TAB_SIZE=2);

    void operator()(const LV2::Program&) override;
    void operator()(const Statement&) override;
    void operator()(const Rvalue&) override;

    /* statements */
    void operator()(RvalueStatement*);

    /* rvalues */
    void operator()(BlockRvalue*);
    void operator()(Literal*);
    void operator()(Lvalue*);

    void operator()(auto); // fall-through

  private:
    void output(const char* strs...);

    const int TAB_SIZE;

    std::ostream& out;
    int currIndent = 0;
    bool startOfNewLine = true;
};

#endif // PRINT_LV2_H
