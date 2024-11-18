#include <monlang-LV2/common.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

ProgramSentence consumeSentence(LV1::Program& prog) {
    ASSERT (prog.sentences.size() > 0);
    auto res = prog.sentences[0];
    prog.sentences = std::vector(
        prog.sentences.begin() + 1,
        prog.sentences.end()
    );
    return res;
}

static Word pw2w(ProgramWord pw) {
    return std::visit(overload{
        [](SquareBracketsTerm*) -> Word {SHOULD_NOT_HAPPEN();},
        [](auto word) -> Word {return word;},
    }, pw);
}

Term toTerm(const ProgramSentence& sentence) {
    ASSERT (sentence.programWords.size() > 0);
    std::vector<Word> words;
    for (auto e: sentence.programWords) {
        words.push_back(pw2w(e));
    }
    return Term{words};
}
