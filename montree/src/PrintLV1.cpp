#include <montree/PrintLV1.h>

#include <monlang-LV1/common.h>
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

#include <utils/nb-utils.h>
#include <utils/str-utils.h>
#include <cstdarg>

#define until(x) while(!(x))

#define SERIALIZE_ERR(malformedMayfail) malformedMayfail.error().fmt.c_str()

#define outputLine(...) \
    output(__VA_ARGS__ __VA_OPT__(,) "\n", nullptr); \
    startOfNewLine = true

#define output(a, ...) output(a, __VA_ARGS__ __VA_OPT__(,) nullptr)
void (PrintLV1::output)(const char* strs...) {
    if (startOfNewLine) {
        out << std::string(currIndent * TAB_SIZE, SPACE);
    }

    va_list args;
    va_start(args, strs);
    const char* currArg = strs;
    until (currArg == nullptr) {
        out << currArg;
        currArg = va_arg(args, const char*);
    }
    va_end(args);

    startOfNewLine = false;
}

///////////////////////////////////////////////////////////////

void PrintLV1::operator()(const MayFail<MayFail_<Program>>& program) {
    const MayFail_<Program>& prog = program.val;
    outputLine(program.has_error()? "~> Program" : "-> Program");

    if (prog.sentences.size() > 0) {
        currIndent++;
    }

    if (prog.sentences.size() > 1) {
        for (int n : range(prog.sentences.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    for (auto programSentence : prog.sentences) {
        operator()(programSentence);
    }
    
    if (prog.sentences.size() > 0) {
        currIndent--;
    }
}

void PrintLV1::operator()(const MayFail<MayFail_<ProgramSentence>>& programSentence) {
    const MayFail_<ProgramSentence>& progSentence = programSentence.val;
    output(programSentence.has_error()? "~> " : "-> ");

    if (numbering.empty()) {
        outputLine("ProgramSentence");
    } else {
        if (int n = numbering.top(); n == NO_NUMBERING) {
            outputLine("ProgramSentence");
        } else {
            outputLine("ProgramSentence #", itoa(n));
        }
        numbering.pop();
    }

    // note: should always enter since a ProgramSentence cannot be empty
    if (progSentence.programWords.size() > 0 || programSentence.has_error()) {
        currIndent++;
    }

    if (progSentence.programWords.size() > 1) {
        for (int n : range(progSentence.programWords.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    int malformedProgramWords = 0;
    for (auto programWord: progSentence.programWords) {
        operator()(MayFail<ProgramWord_>(programWord));
        if (programWord.has_error()) {
            malformedProgramWords += 1;
        }
    }

    if (programSentence.has_error() && malformedProgramWords == 0) {
        outputLine("~> ", SERIALIZE_ERR(programSentence));
    }

    // note: should always enter since a ProgramSentence cannot be empty
    if (progSentence.programWords.size() > 0 || programSentence.has_error()) {
        currIndent--;
    }
}

void PrintLV1::operator()(const MayFail<ProgramWord_>& pw) {
    this->curWord = pw; // needed by word handlers
    output(pw.has_error()? "~> " : "-> ");

    if (numbering.empty()) {
        /* then, it's a stand-alone word */
        std::visit(*this, pw.val); // not a copy as it may seem..
                                   // ,..actually mutates 'this'
        return;
    }

    output("ProgramWord");
    if (int n = numbering.top(); n != NO_NUMBERING) {
        output(" #", itoa(n));
    }
    numbering.pop();
    output(": ");

    std::visit(*this, pw.val); // in case of malformed word,...
                              // ...will still print its partial value
}

void PrintLV1::operator()(const MayFail<MayFail_<Term>>& term) {
    output(term.has_error()? "~> " : "-> ");

    if (numbering.empty()) {
        outputLine("Term");
    } else {
        if (int n = numbering.top(); n == NO_NUMBERING) {
            outputLine("Term");
        } else {
            outputLine("Term #", itoa(n));
        }
        numbering.pop();
    }

    if (term.val.words.size() > 0 || term.has_error()) {
        currIndent++;
    }

    if (term.val.words.size() > 1) {
        for (int n : range(term.val.words.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    int nb_of_malformed_words = 0;
    for (auto word: term.val.words) {
        if (word.has_error()) {
            nb_of_malformed_words++;
        }
        operator()(word);
    }

    if (nb_of_malformed_words == 0 && term.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(term));
    }

    if (term.val.words.size() > 0 || term.has_error()) {
        currIndent--;
    }
}

void PrintLV1::operator()(const MayFail<Word_>& word) {
    this->curWord = mayfail_cast<ProgramWord_>(word); // needed by word handlers
    output(word.has_error()? "~> " : "-> ");

    if (numbering.empty()) {
        /* then, it's a stand-alone word */
        std::visit(*this, word.val); // not a copy as it may seem..
                                     // ,..actually mutates 'this'
        return;
    }

    output("Word");
    if (int n = numbering.top(); n != NO_NUMBERING) {
        output(" #", itoa(n));
    }
    numbering.pop();
    output(": ");

    std::visit(*this, word.val); // in case of malformed word,...
                              // ...will still print its partial value
}

///////////////////////////////////////////////////////////////

void PrintLV1::operator()(MayFail_<SquareBracketsTerm>* sbt) {
    auto curWord_ = curWord; // backup because it gets overriden by `handleTerm`..
                             // ..(which calls operator()(Word))

    outputLine("SquareBracketsTerm");
    currIndent++;

    if (curWord_.has_error() && !sbt->term.has_error()) {
        outputLine("~> ", SERIALIZE_ERR(curWord_));
        return;
    }

    numbering.push(NO_NUMBERING);
    operator()(sbt->term);

    currIndent--;
}

void PrintLV1::operator()(MayFail_<SquareBracketsGroup>* sbg) {
    auto curWord_ = curWord; // backup because it gets overriden by `handleTerm`..
                             // ..(which calls operator()(Word))

    output("SquareBracketsGroup");
    if (sbg->terms.size() == 0 && !curWord_.has_error()) {
        outputLine(" (empty)");
        return;
    }
    outputLine();

    currIndent++;

    if (sbg->terms.size() > 1) {
        for (int n : range(sbg->terms.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    if (sbg->terms.size() == 0) {
        ASSERT(curWord_.has_error());
        outputLine("~> ", SERIALIZE_ERR(curWord_));
    } else {
        int nb_of_malformed_terms = 0;
        for (auto term : sbg->terms) {
            if (term.has_error()) {
                nb_of_malformed_terms++;
            }
            operator()(term);
        }
        if (nb_of_malformed_terms == 0 && curWord_.has_error()) {
            outputLine("~> ", SERIALIZE_ERR(curWord_));
        }
    }

    currIndent--;
}

void PrintLV1::operator()(MayFail_<ParenthesesGroup>* pg) {
    auto curWord_ = curWord; // backup because it gets overriden by `handleTerm`..
                             // ..(which calls operator()(Word))

    output("ParenthesesGroup");
    if (pg->terms.size() == 0 && !curWord_.has_error()) {
        outputLine(" (empty)");
        return;
    }
    outputLine();

    currIndent++;

    if (pg->terms.size() > 1) {
        for (int n : range(pg->terms.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    if (pg->terms.size() == 0) {
        ASSERT(curWord_.has_error());
        outputLine("~> ", SERIALIZE_ERR(curWord_));
    } else {
        int nb_of_malformed_terms = 0;
        for (auto term : pg->terms) {
            if (term.has_error()) {
                nb_of_malformed_terms++;
            }
            operator()(term);
        }
        if (nb_of_malformed_terms == 0 && curWord_.has_error()) {
            outputLine("~> ", SERIALIZE_ERR(curWord_));
        }
    }

    currIndent--;
}

void PrintLV1::operator()(MayFail_<CurlyBracketsGroup>* cbg) {
    auto curWord_ = curWord; // backup because it gets overriden by `handleTerm`..
                             // ..(which calls operator()(Word))

    output("CurlyBracketsGroup");

    if (cbg->sentences.size() == 0 && !curWord_.has_error()) {
        outputLine(" (empty)");
        return;
    }
    outputLine();

    currIndent++;

    /* handle single term */
    if (cbg->term) {
        auto term = cbg->term.value();
        numbering.push(NO_NUMBERING);
        operator()(term);
        if (!term.has_error() && curWord_.has_error()) {
            outputLine("~> ", SERIALIZE_ERR(curWord_));
        }
        currIndent--;
        return;
    }

    if (cbg->sentences.size() > 1) {
        for (int n : range(cbg->sentences.size(), 0)) {
            numbering.push(n);
        }
    } else {
        numbering.push(NO_NUMBERING);
    }

    if (cbg->sentences.size() == 0) {
        ASSERT(curWord_.has_error());
        outputLine("~> ", SERIALIZE_ERR(curWord_));
    } else {
        int nb_of_malformed_sentences = 0;
        for (auto sentence : cbg->sentences) {
            if (sentence.has_error()) {
                nb_of_malformed_sentences++;
            }
            operator()(sentence);
        }
        if (nb_of_malformed_sentences == 0 && curWord_.has_error()) {
            outputLine("~> ", SERIALIZE_ERR(curWord_));
        }
    }

    currIndent--;
}

void PrintLV1::operator()(Atom* atom) {
    outputLine("Atom: `", atom->value.c_str(), "`");
    if (curWord.has_error()) {
        currIndent++;
        outputLine("~> ", SERIALIZE_ERR(curWord));
        currIndent--;
    }
}

void PrintLV1::operator()(MayFail_<PostfixSquareBracketsGroup>* psbg) {
    outputLine("PostfixSquareBracketsGroup");

    auto savedStack = numbering;
    /* add `Word: ` prefix in tree */
    numbering = std::stack<int>({NO_NUMBERING});

    currIndent++;
    operator()(MayFail(wrap_w(psbg->leftPart)));
    currIndent--;

    currIndent++;
    operator()(mayfail_convert<Word_>(psbg->rightPart));
    currIndent--;

    numbering = savedStack;
}

void PrintLV1::operator()(MayFail_<PostfixParenthesesGroup>* ppg) {
    outputLine("PostfixParenthesesGroup");

    auto savedStack = numbering;
    /* add `Word: ` prefix in tree */
    numbering = std::stack<int>({NO_NUMBERING});

    currIndent++;
    operator()(MayFail(wrap_w(ppg->leftPart)));
    currIndent--;

    currIndent++;
    operator()(mayfail_convert<Word_>(ppg->rightPart));
    currIndent--;

    numbering = savedStack;
}

void PrintLV1::operator()(MayFail_<Association>* assoc) {
    outputLine("Association");

    auto savedStack = numbering;

    /* add `Word: ` prefix in tree */
    numbering = std::stack<int>({NO_NUMBERING});

    currIndent++;
    operator()(MayFail(wrap_w(variant_cast(assoc->leftPart))));
    currIndent--;

    /* add `Word: ` prefix in tree */
    numbering = std::stack<int>({NO_NUMBERING});

    currIndent++;
    operator()(mayfail_cast<Word_>(assoc->rightPart));
    currIndent--;

    numbering = savedStack;
}

void PrintLV1::operator()(auto) {
    outputLine("<ENTITY NOT IMPLEMENTED YET>");
}

///////////////////////////////////////////////////////////////

PrintLV1::PrintLV1(std::ostream& os, int TAB_SIZE) : TAB_SIZE(TAB_SIZE), out(os){}
