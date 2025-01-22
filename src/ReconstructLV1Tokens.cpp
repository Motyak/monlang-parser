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
    // lastCorrectToken = -1;

    auto tokenId = newToken(prog);
    token.is_malformed = prog.has_error();
    token.name = "Program";

    if (token.is_malformed) {
        token.err_desc = prog.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupLastCorrectToken = lastCorrectToken;
    for (auto sentence: prog.val.sentences) {
        auto sentenceTokenId = tokens._vec.size();
        operator()(sentence);
        if (!tokens._vec.at(sentenceTokenId).is_malformed) {
            lastCorrectToken = sentenceTokenId;
        }
    }
    // special case, no _tokenLen
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
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
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    LOOP for (auto pw: sentence.val.programWords) {
        if (!__first_it) {
            curPos += sequenceLen(ProgramSentence::CONTINUATOR_SEQUENCE);
        }
        auto pwTokenId = tokens._vec.size();
        operator()(pw);
        if (!tokens._vec.at(pwTokenId).is_malformed) {
            lastCorrectToken = pwTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += sentence.val._tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    curPos += sentence.val._tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
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
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    LOOP for (auto word: term.val.words) {
        if (!__first_it) {
            curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
        }
        auto wordTokenId = tokens._vec.size();
        operator()(word);
        if (!tokens._vec.at(wordTokenId).is_malformed) {
            lastCorrectToken = wordTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += term.val._tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
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

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsTerm>* sbt) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = "SquareBracketsTerm";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(SquareBracketsTerm::INITIATOR_SEQUENCE);
    operator()(sbt->term);
    curPos = backupCurPos;
    curPos += sbt->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsGroup>* sbg) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = "SquareBracketsGroup";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE);
    LOOP for (auto term: sbg->terms) {
        if (!__first_it) {
            curPos += sequenceLen(SquareBracketsGroup::CONTINUATOR_SEQUENCE);
        }
        auto termTokenId = tokens._vec.size();
        operator()(term);
        if (!tokens._vec.at(termTokenId).is_malformed) {
            lastCorrectToken = termTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += sbg->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
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
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto term: pg->terms) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        auto termTokenId = tokens._vec.size();
        operator()(term);
        if (!tokens._vec.at(termTokenId).is_malformed) {
            lastCorrectToken = termTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += pg->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<CurlyBracketsGroup>* cbg) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = "CurlyBracketsGroup";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(CurlyBracketsGroup::INITIATOR_SEQUENCE);
    if (cbg->term.has_value()) {
        /* oneline cbg */
        operator()(cbg->term.value());
    }
    else {
        /* multiline cbg */
        curPos += 1; // newline
        for (auto sentence: cbg->sentences) {
            auto sentenceTokenId = tokens._vec.size();
            operator()(sentence);
            if (!tokens._vec.at(sentenceTokenId).is_malformed) {
                lastCorrectToken = sentenceTokenId;
            }
        }
    }
    curPos = backupCurPos;
    curPos += cbg->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens._vec.at(lastCorrectToken).end + 1);
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixSquareBracketsGroup>* psbg) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = "PostfixSquareBracketsGroup";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(MayFail(wrap_w(psbg->leftPart)));
    operator()(mayfail_convert<Word_>(psbg->rightPart));
    curPos = backupCurPos;
    curPos += psbg->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixParenthesesGroup>* ppg) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = "PostfixParenthesesGroup";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(MayFail(wrap_w(ppg->leftPart)));
    operator()(mayfail_convert<Word_>(ppg->rightPart));
    curPos = backupCurPos;
    curPos += ppg->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<Association>* assoc) {
    auto entity = isProgramWord? (LV1::Ast_)curWord : mayfail_cast<Word_>(curWord);
    auto tokenId = newToken(entity);
    token.is_malformed = curWord.has_error();
    token.name = "Association";

    if (token.is_malformed) {
        token.err_desc = curWord.error().fmt; // TODO: map this to the actual error description
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(MayFail(wrap_w(variant_cast(assoc->leftPart))));
    curPos += sequenceLen(Association::SEPARATOR_SEQUENCE);
    operator()(assoc->rightPart);
    curPos = backupCurPos;
    curPos += assoc->_tokenLen;
    token.end = asTokenPosition(curPos - !!curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
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
