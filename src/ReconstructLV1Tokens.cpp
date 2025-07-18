#include <monlang-parser/ReconstructLV1Tokens.h>

#include <monlang-LV1/Program.h>
#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>
#include <monlang-LV1/Word.h>

/* impl only */
#include <monlang-LV1/Atom.h>
#include <monlang-LV1/Quotation.h>
#include <monlang-LV1/SquareBracketsTerm.h>
#include <monlang-LV1/SquareBracketsGroup.h>
#include <monlang-LV1/MultilineSquareBracketsGroup.h>
#include <monlang-LV1/ParenthesesGroup.h>
#include <monlang-LV1/CurlyBracketsGroup.h>
#include <monlang-LV1/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/PostfixParenthesesGroup.h>
#include <monlang-LV1/Path.h>
#include <monlang-LV1/Association.h>

#include <utils/assert-utils.h>
#include <utils/loop-utils.h>

#include <algorithm>
#include <numeric>

#define token tokens[tokenId]

void ReconstructLV1Tokens::operator()(MayFail<MayFail_<Program>>& prog) {
    /* reset state */
    tokens = {};
    curPos = 0;
    // lastCorrectToken = -1;

    auto tokenId = newToken();
    prog.val._tokenId = tokenId;
    token.is_malformed = prog.has_error();
    token.name = "Program";

    if (token.is_malformed) {
        token.err_fmt = prog.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupLastCorrectToken = lastCorrectToken;
    for (auto& sentence: prog.val.sentences) {
        auto sentenceTokenId = tokens.size();
        operator()(sentence);
        if (!tokens[sentenceTokenId].is_malformed) {
            lastCorrectToken = sentenceTokenId;
        }
    }
    // special case, no _tokenLen
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail<MayFail_<ProgramSentence>>& sentence) {
    auto tokenId = newToken();
    sentence.val._tokenId = tokenId;
    token.is_malformed = sentence.has_error();
    token.name = "ProgramSentence";

    if (token.is_malformed) {
        token.err_fmt = sentence.error().fmt; // TODO: we will need to fill token.err_desc as well
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
        auto pwTokenId = tokens.size();
        operator()(pw);
        if (!tokens[pwTokenId].is_malformed) {
            lastCorrectToken = pwTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += sentence.val._tokenLen;
    token.end = asTokenPosition(curPos);

    curPos += sentence.val._tokenTrailingNewlines;

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
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

void ReconstructLV1Tokens::operator()(MayFail<MayFail_<Term>>& term) {
    auto tokenId = newToken();
    term.val._tokenId = tokenId;
    token.is_malformed = term.has_error();
    token.name = "Term";

    if (token.is_malformed) {
        token.err_fmt = term.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    LOOP for (auto word: term.val.words) {
        if (!__first_it) {
            curPos += sequenceLen(Term::CONTINUATOR_SEQUENCE);
        }
        auto wordTokenId = tokens.size();
        operator()(word);
        if (!tokens[wordTokenId].is_malformed) {
            lastCorrectToken = wordTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += term.val._tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
            // using this logic, err_start can then be before token.start
            // e.g.: `[a, <EOF>` (because the last correct token would then be `a`)
            // Therefore, in this case, we will set err_start to token.start
            token.err_start = token.err_start < token.start? token.start : token.err_start;
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

void ReconstructLV1Tokens::operator()(const PostfixLeftPart& postfixLeftPart) {
    auto mf_word = MayFail(wrap_w(variant_cast(postfixLeftPart)));
    operator()(mf_word);

    /* then propagate the change back to the parameter */

    // Word to PostfixLeftPart (similar to pw2w())
    auto newVal = std::visit(overload{
        [](Association*) -> PostfixLeftPart {SHOULD_NOT_HAPPEN();},
        [](auto* ptr) -> PostfixLeftPart {return ptr;},
    }, unwrap_w(mf_word.value()));

    std::visit(
        [newVal](auto* ptr){*ptr = *std::get<decltype(ptr)>(newVal);},
        postfixLeftPart
    );
}

///////////////////////////////////////////////////////////////

void ReconstructLV1Tokens::operator()(Atom* atom) {
    auto tokenId = newToken();
    atom->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "Atom";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    curPos += atom->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV1Tokens::operator()(Quotation* quot) {
    auto tokenId = newToken();
    quot->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "Quotation";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    curPos += quot->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = asTokenPosition(token.end); // !!! SPECIAL CASE (end + 1 instead of start)
        tokens.traceback.push_back(token);
    }
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsTerm>* sbt) {
    auto tokenId = newToken();
    sbt->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "SquareBracketsTerm";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(SquareBracketsTerm::INITIATOR_SEQUENCE);
    operator()(sbt->term);
    curPos = backupCurPos;
    curPos += sbt->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsGroup>* sbg) {
    auto tokenId = newToken();
    sbg->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "SquareBracketsGroup";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(SquareBracketsGroup::INITIATOR_SEQUENCE);
    LOOP for (auto& term: sbg->terms) {
        if (!__first_it) {
            curPos += sequenceLen(SquareBracketsGroup::CONTINUATOR_SEQUENCE);
        }
        auto termTokenId = tokens.size();
        operator()(term);
        if (!tokens[termTokenId].is_malformed) {
            lastCorrectToken = termTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += sbg->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<MultilineSquareBracketsGroup>* msbg) {
    auto tokenId = newToken();
    msbg->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "MultilineSquareBracketsGroup";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(MultilineSquareBracketsGroup::INITIATOR_SEQUENCE);
    for (auto& sentence: msbg->sentences) {
        auto sentenceTokenId = tokens.size();
        operator()(sentence);
        if (!tokens[sentenceTokenId].is_malformed) {
            lastCorrectToken = sentenceTokenId;
        }
    }
    curPos = backupCurPos;
    curPos += msbg->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<ParenthesesGroup>* pg) {
    auto tokenId = newToken();
    pg->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "ParenthesesGroup";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    curPos += sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE);
    LOOP for (auto& term: pg->terms) {
        if (!__first_it) {
            curPos += sequenceLen(ParenthesesGroup::CONTINUATOR_SEQUENCE);
        }
        auto termTokenId = tokens.size();
        operator()(term);
        if (!tokens[termTokenId].is_malformed) {
            lastCorrectToken = termTokenId;
        }
        ENDLOOP
    }
    curPos = backupCurPos;
    curPos += pg->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<CurlyBracketsGroup>* cbg) {
    auto tokenId = newToken();
    cbg->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "CurlyBracketsGroup";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
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
        for (auto& sentence: cbg->sentences) {
            auto sentenceTokenId = tokens.size();
            operator()(sentence);
            if (!tokens[sentenceTokenId].is_malformed) {
                lastCorrectToken = sentenceTokenId;
            }
        }
    }
    curPos = backupCurPos;
    curPos += cbg->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        if (lastCorrectToken == size_t(-1)) {
            token.err_start = token.start;
        }
        else {
            token.err_start = asTokenPosition(tokens[lastCorrectToken].end);
            token.err_start = token.err_start < token.start? token.start : token.err_start;
        }
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixSquareBracketsGroup>* psbg) {
    auto tokenId = newToken();
    psbg->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "PostfixSquareBracketsGroup";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(psbg->leftPart);
    operator()(mayfail_cast_by_ref<Word_>(psbg->rightPart));
    curPos = backupCurPos;
    curPos += psbg->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixParenthesesGroup>* ppg) {
    auto tokenId = newToken();
    ppg->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "PostfixParenthesesGroup";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(ppg->leftPart);
    operator()(mayfail_cast_by_ref<Word_>(ppg->rightPart));
    curPos = backupCurPos;
    curPos += ppg->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<Path>* path) {
    auto tokenId = newToken();
    path->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "Path";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
    }

    token.start = asTokenPosition(curPos);
    auto backupCurPos = curPos;
    auto backupLastCorrectToken = lastCorrectToken;
    // lastCorrectToken = -1;
    operator()(path->leftPart);
    curPos += sequenceLen(Path::SEPARATOR_SEQUENCE);
    operator()(mayfail_cast_by_ref<Word_>(path->rightPart));
    curPos = backupCurPos;
    curPos += path->_tokenLen;
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}

void ReconstructLV1Tokens::operator()(MayFail_<Association>* assoc) {
    auto tokenId = newToken();
    assoc->_tokenId = tokenId;
    token.is_malformed = curWord.has_error();
    token.name = "Association";

    if (token.is_malformed) {
        token.err_fmt = curWord.error().fmt; // TODO: we will need to fill token.err_desc as well
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
    token.end = asTokenPosition(curPos);

    if (token.is_malformed) {
        token.err_start = token.start;
        tokens.traceback.push_back(token);
    }

    lastCorrectToken = backupLastCorrectToken;
}


///////////////////////////////////////////////////////////////

ReconstructLV1Tokens::ReconstructLV1Tokens(Tokens& tokens, const std::vector<size_t>& newlinesPos, const std::vector<std::pair<size_t, unsigned>>& unicharBytesPos)
        : tokens(tokens), newlinesPos(newlinesPos), unicharBytesPos(unicharBytesPos){}

TokenId ReconstructLV1Tokens::newToken() {
    tokens._vec.push_back(Token{});
    return tokens.size() - 1;
}

TokenPosition ReconstructLV1Tokens::asTokenPosition(size_t index) {
    static const auto cmp_lt_key = [](const std::pair<size_t, unsigned>& a, size_t b){
        return a.first < b;
    };
    static const auto op_add_val = [](size_t a, const std::pair<size_t, unsigned>& b){
        return a + b.second;
    };

    auto lower_bound = std::lower_bound(newlinesPos.begin(), newlinesPos.end(), index);
    size_t line = lower_bound - newlinesPos.begin() + 1;
    size_t lineStartIndex = lower_bound == newlinesPos.begin()? 0 : *(lower_bound - 1);

    auto begin = std::lower_bound(unicharBytesPos.begin(), unicharBytesPos.end(), lineStartIndex, cmp_lt_key);
    auto end = std::lower_bound(begin, unicharBytesPos.end(), index, cmp_lt_key);
    unsigned unicharBytes = std::accumulate(begin, end, 0, op_add_val);
    size_t column = (lower_bound == newlinesPos.begin()? index + 1 : index - *(lower_bound - 1)) - unicharBytes;

    return TokenPosition{index, line, column};
}
