#ifndef PRINT_LV2_H
#define PRINT_LV2_H

#include <monlang-LV2/stmt/ExpressionStatement.h>
#include <monlang-LV2/expr/Operation.h>
#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/expr/Lambda.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/expr/Literal.h>
#include <monlang-LV2/expr/Lvalue.h>

/* interface only */
#include <monlang-LV2/visitors/visitor.h>

class PrintLV2 : public LV2::AstVisitor<void> {
  public:
    PrintLV2(std::ostream&, int TAB_SIZE=2);

    void operator()(const LV2::Program&) override;
    void operator()(const Statement&) override;
    void operator()(const Expression&) override;

    /* statements */
    void operator()(ExpressionStatement*);

    /* expressions */
    void operator()(Operation*);
    void operator()(FunctionCall*);
    void operator()(Lambda*);
    void operator()(BlockExpression*);
    void operator()(Literal*);
    void operator()(Lvalue*);

    void operator()(auto); // fall-through

  private:
    void output(const char* strs...);

    const int TAB_SIZE;

    /* consider a State struct with all these fields as ref types..
       .. + struct that holds the actual values (scope variable) */
    std::ostream& out;
    int currIndent = 0;
    bool startOfNewLine = true;
};

#endif // PRINT_LV2_H
