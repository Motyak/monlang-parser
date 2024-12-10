#ifndef PRINT_LV2_H
#define PRINT_LV2_H

#include <stack>

/* interface only */
#include <monlang-LV2/visitors/visitor.h>

class PrintLV2 : public LV2::AstVisitor_<void> {
  public:
    PrintLV2(std::ostream&, int TAB_SIZE=2);

    void operator()(const MayFail<MayFail_<LV2::Program>>&) override;
    void operator()(const MayFail<Statement_>&) override;
    void operator()(const MayFail<Expression_>&) override;

    /* statements */
    void operator()(MayFail_<Assignment>*);
    void operator()(MayFail_<Accumulation>*);
    void operator()(MayFail_<LetStatement>*);
    void operator()(MayFail_<VarStatement>*);
    void operator()(MayFail_<ReturnStatement>*);
    void operator()(BreakStatement*);
    void operator()(ContinueStatement*);
    void operator()(DieStatement*);
    void operator()(MayFail_<ForeachStatement>*);
    void operator()(MayFail_<WhileStatement>*);
    void operator()(MayFail_<DoWhileStatement>*);
    void operator()(MayFail_<ExpressionStatement>*);

    /* expressions */
    void operator()(MayFail_<Operation>*);
    void operator()(MayFail_<FunctionCall>*);
    void operator()(MayFail_<Lambda>*);
    void operator()(MayFail_<BlockExpression>*);
    void operator()(SpecialSymbol*);
    void operator()(Literal*);
    void operator()(Lvalue*);

    void operator()(auto); // fall-through

  private:
    void output(const char* strs...);

    static constexpr int NO_NUMBERING = -1;
    const int TAB_SIZE;

    std::ostream& out;
    std::stack<int> numbering;
    int currIndent = 0;
    bool startOfNewLine = true;
    MayFail<Statement_> currStatement;
    MayFail<Expression_> currExpression;
};

#endif // PRINT_LV2_H
