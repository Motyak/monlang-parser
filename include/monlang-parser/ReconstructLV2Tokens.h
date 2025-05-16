#ifndef RECONSTRUCT_LV2_TOKENS_H
#define RECONSTRUCT_LV2_TOKENS_H

#include <monlang-parser/Tokens.h>

#include <monlang-LV2/Program.h>
#include <monlang-LV2/Statement.h>
#include <monlang-LV2/Expression.h>
#include <monlang-LV2/Lvalue.h>

class ReconstructLV2Tokens {
  public:
    ReconstructLV2Tokens(Tokens&, const std::vector<size_t>& newlinesPos={});

    // entrypoint
    void operator()(MayFail<MayFail_<LV2::Program>>&);
    void operator()(const MayFail<Statement_>&); // we can copy here (ptrs)
    void operator()(const MayFail<Expression_>&); // we can copy here (ptrs)
    void operator()(const MayFail<Lvalue_>&); // we can copy here (ptrs)

    /* statements */
    void operator()(_StubStatement_*);
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
    void operator()(_StubExpression_*);
    void operator()(MayFail_<Operation>*);
    void operator()(MayFail_<FunctionCall>*);
    void operator()(MayFail_<Lambda>*);
    void operator()(MayFail_<BlockExpression>*);
    void operator()(MayFail_<FieldAccess>*);
    void operator()(MayFail_<Subscript>*);
    void operator()(MayFail_<MapLiteral>*);
    void operator()(MayFail_<ListLiteral>*);
    void operator()(Numeral*);
    void operator()(StrLiteral*);
    void operator()(SpecialSymbol*);
    void operator()(Symbol*);

  private:
    Tokens& tokens;
    const std::vector<size_t> newlinesPos; // sorted asc

    size_t curPos = 0;
    MayFail<Statement_> curStmt;
    MayFail<Expression_> curExpr;
    TokenId lastCorrectToken = -1;
    size_t exprGroupNesting = 0;

    TokenId newToken();
    TokenPosition asTokenPosition(size_t index);
};

#endif // RECONSTRUCT_LV2_TOKENS_H
