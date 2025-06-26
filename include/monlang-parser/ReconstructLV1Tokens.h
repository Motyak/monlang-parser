#ifndef RECONSTRUCT_LV1_TOKENS_H
#define RECONSTRUCT_LV1_TOKENS_H

#include <monlang-parser/Tokens.h>

#include <monlang-LV1/Program.h>
#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>
#include <monlang-LV1/Word.h>
#include <monlang-LV1/ast/_PostfixLeftPart.h>

class ReconstructLV1Tokens {
  public:
    ReconstructLV1Tokens(Tokens&, const std::vector<size_t>& newlinesPos = {}, const std::vector<std::pair<size_t, unsigned>>& = {});

    // entrypoint
    void operator()(MayFail<MayFail_<Program>>&);
    void operator()(MayFail<MayFail_<ProgramSentence>>&);
    void operator()(const MayFail<ProgramWord_>&); // we can copy here (ptrs)
    void operator()(MayFail<MayFail_<Term>>&);
    void operator()(const MayFail<Word_>&); // we can copy here (ptrs)
    void operator()(const PostfixLeftPart&); // we can copy here (ptrs)

    void operator()(Atom*);
    void operator()(Quotation*);
    void operator()(MayFail_<SquareBracketsTerm>*);
    void operator()(MayFail_<SquareBracketsGroup>*);
    void operator()(MayFail_<MultilineSquareBracketsGroup>*);
    void operator()(MayFail_<ParenthesesGroup>*);
    void operator()(MayFail_<CurlyBracketsGroup>*);
    void operator()(MayFail_<PostfixSquareBracketsGroup>*);
    void operator()(MayFail_<PostfixParenthesesGroup>*);
    void operator()(MayFail_<Path>*);
    void operator()(MayFail_<Association>*);

  private:
    Tokens& tokens;
    const std::vector<size_t> newlinesPos; // sorted asc
    const std::vector<std::pair<size_t, unsigned>> unicharBytesPos; // key sorted asc

    size_t curPos = 0;
    bool isProgramWord;
    MayFail<ProgramWord_> curWord;
    TokenId lastCorrectToken = -1;

    TokenId newToken();
    TokenPosition asTokenPosition(size_t index);
};

#endif // RECONSTRUCT_LV1_TOKENS_H
