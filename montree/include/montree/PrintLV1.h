#ifndef PRINT_LV1_H
#define PRINT_LV1_H

#include <monlang-LV1/common.h>
#include <monlang-LV1/Program.h>
#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Word.h>

#include <monlang-LV1/Term.h>

/* interface only */
#include <monlang-LV1/visitors/visitor.h>
#include <stack>

class PrintLV1 : public AstVisitor_<void> {
  public:
    PrintLV1(std::ostream&, int TAB_SIZE=2);

    void operator()(const MayFail<MayFail_<Program>>&) override;
    void operator()(const MayFail<MayFail_<ProgramSentence>>&) override;
    void operator()(const MayFail<ProgramWord_>&) override;
    void operator()(const MayFail<MayFail_<Term>>&) override;
    void operator()(const MayFail<Word_>&) override;

    void operator()(MayFail_<SquareBracketsTerm>*);
    void operator()(MayFail_<SquareBracketsGroup>*);
    void operator()(MayFail_<ParenthesesGroup>*);
    void operator()(MayFail_<CurlyBracketsGroup>*);
    void operator()(Atom*);
    void operator()(MayFail_<PostfixSquareBracketsGroup>*);
    void operator()(MayFail_<PostfixParenthesesGroup>*);
    void operator()(MayFail_<Association>*);

    void operator()(auto); // fall-through

  private:
    static constexpr int NO_NUMBERING = -1;
    const int TAB_SIZE;

    void output(const char* strs...);

    std::ostream& out;
    std::stack<int> numbering;
    bool startOfNewLine = true;
    int currIndent = 0;
    MayFail<ProgramWord_> curWord; // May be a ProgramWord or a Word, so let's name it `word`
};

#endif // PRINT_LV1_H
