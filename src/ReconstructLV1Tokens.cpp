#include <monlang-parser/ReconstructLV1Tokens.h>

#include <monlang-LV1/Program.h>
#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>
#include <monlang-LV1/Word.h>

/* impl only */
#include <monlang-LV1/Atom.h>
#include <monlang-LV1/SquareBracketsTerm.h>
#include <monlang-LV1/SquareBracketsGroup.h>
#include <monlang-LV1/ParenthesesGroup.h>
#include <monlang-LV1/CurlyBracketsGroup.h>
#include <monlang-LV1/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/PostfixParenthesesGroup.h>
#include <monlang-LV1/Association.h>

#include <utils/assert-utils.h>

#define token tokens._vec.at(tokenId)

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<Program>>& prog) {
    /* reset state */
    tokens = {};
    curPos = 0;

    auto tokenId = newToken(prog);
    token.is_malformed = prog.has_error();
    token.name = token.is_malformed? "Malformed Program" : "Program";

    if (token.is_malformed) {
        token.err_desc = prog.error().fmt; // TODO: map this to the actual error description
    }

    token.start = curPos;
    for (auto sentence: prog.val.sentences) {
        operator()(sentence);
    }
    // special case, no _tokenLen
    token.end = curPos;
}

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<ProgramSentence>>& sentence) {
    auto tokenId = newToken(sentence);
    token.is_malformed = sentence.has_error();
    token.name = sentence.has_error()? "Malformed ProgramSentence" : "ProgramSentence";

    if (token.is_malformed) {
        token.err_desc = sentence.error().fmt; // TODO: map this to the actual error description
    }

    curPos += sentence.val._tokenLeadingNewlines;
    curPos += sentence.val._tokenIndentSpaces;

    token.start = curPos;
    auto backupCurPos = curPos;
    for (auto pw: sentence.val.programWords) {
        operator()(pw);
    }
    curPos = backupCurPos;
    curPos += sentence.val._tokenLen;
    token.end = curPos;

    curPos += sentence.val._tokenTrailingNewlines;
}

void ReconstructLV1Tokens::operator()(const MayFail<ProgramWord_>& pw) {
    isProgramWord = true;
    curWord = pw; // needed by word handlers
    std::visit(*this, pw.val);
}

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<Term>>& term) {
    auto tokenId = newToken(term);
    token.is_malformed = term.has_error();
    token.name = token.is_malformed? "Malformed Term" : "Term";

    if (token.is_malformed) {
        token.err_desc = term.error().fmt; // TODO: map this to the actual error description
    }

    token.start = curPos;
    curPos += term.val._tokenLen;
    token.end = curPos;
}

void ReconstructLV1Tokens::operator()(const MayFail<Word_>& word) {
    isProgramWord = false;
    curWord = mayfail_cast<ProgramWord_>(word); // needed by word handlers
    std::visit(*this, word.val);
}

///////////////////////////////////////////////////////////////

void ReconstructLV1Tokens::operator()(Atom* atom) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = token.is_malformed? "Malformed Atom" : "Atom";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = curPos;
    curPos += atom->_tokenLen;
    token.end = curPos;
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsTerm>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<ParenthesesGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<CurlyBracketsGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixSquareBracketsGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixParenthesesGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<Association>*) {
    TODO();
}


///////////////////////////////////////////////////////////////

ReconstructLV1Tokens::ReconstructLV1Tokens(LV1Tokens& tokens) : tokens(tokens){}

TokenId ReconstructLV1Tokens::newToken(const LV1::Ast_& entity) {
    tokens._vec.push_back(Token{});
    return tokens._map[entity] = tokens._vec.size() - 1;
}
