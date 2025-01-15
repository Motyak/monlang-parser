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
#include <utils/loop-utils.h>

#define token tokens._vec.at(tokenId)

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<Program>>& prog) {
    /* reset state */
    tokens = {};
    curPos = 0;

    auto tokenId = newToken(prog);
    token.is_malformed = prog.has_error();
    token.name = "Program";

    if (token.is_malformed) {
        token.err_desc = prog.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    for (auto sentence: prog.val.sentences) {
        operator()(sentence);
    }
    // special case, no _tokenLen
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<ProgramSentence>>& sentence) {
    auto tokenId = newToken(sentence);
    token.is_malformed = sentence.has_error();
    token.name = "ProgramSentence";

    if (token.is_malformed) {
        token.err_desc = sentence.error().fmt; // TODO: map this to the actual error description
    }

    curPos += sentence.val._tokenLeadingNewlines;
    curPos += sentence.val._tokenIndentSpaces;

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    LOOP for (auto pw: sentence.val.programWords) {
        if (!__first_it) {
            curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        }
        operator()(pw);
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += sentence.val._tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += sentence.val._tokenTrailingNewlines;

    if (token.is_malformed) {
        if (tokens._vec.empty() || tokens._vec.back().is_malformed) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.back().end + 1);
        }
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV1Tokens::operator()(const MayFail<ProgramWord_>& pw) {
    isProgramWord = true;
    curWord = pw; // needed by word handlers
    std::visit(*this, pw.val);
}

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<Term>>& term) {
    auto tokenId = newToken(term);
    token.is_malformed = term.has_error();
    token.name = "Term";

    if (token.is_malformed) {
        token.err_desc = term.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    LOOP for (auto word: term.val.words) {
        if (!__first_it) {
            curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
        }
        operator()(word);
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += term.val._tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        if (tokens._vec.empty() || tokens._vec.back().is_malformed) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.back().end + 1);
        }
        tokens.traceback.push_back(token);
    }
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
    token.name = "Atom";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    curPos += atom->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsTerm>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<ParenthesesGroup>* pg) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = "ParenthesesGroup";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto term: pg->terms) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        operator()(term);
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += pg->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        if (tokens._vec.empty() || tokens._vec.back().is_malformed) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.back().end + 1); // TODO: not always correct ?
        }
        tokens.traceback.push_back(token);
    }
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

ReconstructLV1Tokens::ReconstructLV1Tokens(LV1Tokens& tokens, const std::vector<size_t>& newlinesPos)
        : tokens(tokens), newlinesPos(newlinesPos){}

TokenId ReconstructLV1Tokens::newToken(const LV1::Ast_& entity) {
    tokens._vec.push_back(Token{});
    return tokens._map[entity] = tokens._vec.size() - 1;
}

TokenPosition ReconstructLV1Tokens::asTokenPosition(size_t index) {
    if (newlinesPos.empty()) {
        return TokenPosition{index, 1, index + 1};
    }
    auto lower_bound = std::lower_bound(newlinesPos.begin(), newlinesPos.end(), index);
    size_t line = lower_bound - newlinesPos.begin() + 1;
    size_t column = lower_bound == newlinesPos.begin()? index + 1 : index - *(lower_bound - 1);

    return TokenPosition{index, line, column};
}
