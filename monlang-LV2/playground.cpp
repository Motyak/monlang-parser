
#include <monlang-LV1/Program.h>

#include <utils/assert-utils.h>
#include <utils/defer-util.h>

#include <string>

// tmp
struct Statement {
    std::string type;
    ProgramSentence sentence;
};

struct Assignment {
    Lvalue lvalue;
    Rvalue rvalue;
};

std::optional<ProgramSentence> nextSentence(LV1::Program* prog) {
    auto& sentences = prog->sentences;
    if (sentences.empty()) {
        return {};
    }
    auto res = sentences[0];
    sentences = std::vector(sentences.begin() + 1, sentences.end());
    return res;
}

std::optional<ProgramWord> nextWord(ProgramSentence* sentence) {
    auto& words = sentence->programWords;
    if (words.empty()) {
        return {};
    }
    auto res = words[0];
    words = std::vector(words.begin() + 1, words.end());
    return res;
}

template <typename T>
bool check_if_matches(T entity, Pattern<T> pattern) {

}

bool check_if_matches(ProgramSentence sentence, Pattern<ProgramSentence> pattern) {

}

Statement consumeStatement(LV1::Program& prog) {
    /*
        either returns a valid statement (syntactically)..
        ..or, if even an rvalue would be malformed,..
        ..throw an exception ?
    */

    ASSERT (prog.sentences.size() > 0);
    auto currSentence = *nextSentence(&prog);

    if (currSentence =~ "LVALUE Atom<`:=`> RVALUE"_) {
        /* return consumeAssignment(&currSentence) */
        auto lvalue = consumeLvalue(&currSentence);
        consumeAtom(&currSentence);
        auto rvalue = consumeRvalue(&currSentence);
        return Assignment{lvalue, rvalue};
    }

    // fall-through statement
    return consumeRvalueStatement(&currSentence);
}

int main()
{

}
