#ifndef PRINT_LV2_H
#define PRINT_LV2_H

#include <monlang-LV2/RvalueStatement.h>
#include <monlang-LV2/Lvalue.h>

/* interface only */
#include <monlang-LV2/visitors/visitor.h>

class PrintLV2 : public LV2::AstVisitor<void> {
  public:
    PrintLV2(std::ostream&, int TAB_SIZE=2);

    void operator()(const LV2::Program&) override;
    void operator()(const Statement&) override;
    void operator()(const Rvalue&) override;

    void operator()(const RvalueStatement&);

    void operator()(const Lvalue&);

    void operator()(auto); // fall-through

  private:
    const int TAB_SIZE;

    std::ostream& out;
};

#endif // PRINT_LV2_H
