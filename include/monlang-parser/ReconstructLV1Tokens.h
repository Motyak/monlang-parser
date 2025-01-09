#ifndef RECONSTRUCT_LV1_TOKENS_H
#define RECONSTRUCT_LV1_TOKENS_H

#include <monlang-parser/Tokens.h>

#include <monlang-LV1/visitors/visitor.h>

class ReconstructLV1Tokens : public /*LV1*/AstVisitor_<void> {
  public:
    ReconstructLV1Tokens(LV1Tokens&);

    void operator()(const MayFail<MayFail_<Program>>&) override;
    void operator()(const MayFail<MayFail_<ProgramSentence>>&) override;
    void operator()(const MayFail<ProgramWord_>&) override;
    void operator()(const MayFail<MayFail_<Term>>&) override;
    void operator()(const MayFail<Word_>&) override;

    void operator()(Atom*);
    void operator()(MayFail_<SquareBracketsTerm>*);
    void operator()(MayFail_<SquareBracketsGroup>*);
    void operator()(MayFail_<ParenthesesGroup>*);
    void operator()(MayFail_<CurlyBracketsGroup>*);
    void operator()(MayFail_<PostfixSquareBracketsGroup>*);
    void operator()(MayFail_<PostfixParenthesesGroup>*);
    void operator()(MayFail_<Association>*);

  private:
    LV1Tokens& tokens;
    MayFail<ProgramWord_> curWord; // May be a ProgramWord or a Word, so let's name it `word`
};

#endif // RECONSTRUCT_LV1_TOKENS_H
