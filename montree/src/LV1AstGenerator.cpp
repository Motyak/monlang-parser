#include <montree/LV1AstGenerator.h>

#include <utils/mem-utils.h>
#include <utils/assert-utils.h>
#include <utils/defer-util.h>

static const std::vector<std::string> WORDS = {
    "sbt",
    "assoc",
    "ppg",
    "psbg",
    "pg",
    "sbg",
    "cbg",
    "atom"
};

static void update_word_proba(std::string key, long new_value, std::map<std::string, unsigned long>& proba) {
    ASSERT (proba.find(key) != proba.end());
    ASSERT (0 <= new_value && new_value <= 100'000);
    long diff = new_value >= proba[key]? +(new_value - proba[key]) : -(proba[key] - new_value);
    proba[key] = new_value;

    std::vector other_words(WORDS.begin(), WORDS.end());
    std::erase_if(other_words, [&key, &proba](auto word){return word == key || proba[word] == 0;});

    for (auto other_word: other_words) {
        proba[other_word] += double(-diff / other_words.size());
        ASSERT (proba[other_word] >= 0);
        // if (proba[other_word] < 0) {
        //     proba[other_word] = 0;
        // }
    }
}

// static std::map<std::string, long> backup_word_probas(const std::map<std::string, unsigned long>& proba) {
//     std::map<std::string, long> res;
//     for (auto word: WORDS) {
//         res[word] = proba.at(word);
//     }
//     return res;
// }

// static void restore_word_probas(const std::map<std::string, long>& new_proba, std::map<std::string, unsigned long>& proba) {
//     for (auto word: WORDS) {
//         proba[word] = new_proba.at(word);
//     }
// }

// static void init_words(std::map<std::string, unsigned long>& proba) {
//     /* all program words proba sum up to 100'000 */

//     // proba["sbt"] =   50'000;

//     // proba["assoc"] = 50'000.0 / 7;
//     // proba["ppg"] =   50'000.0 / 7;
//     // proba["psbg"] =  50'000.0 / 7;
//     // proba["pg"] =    50'000.0 / 7;

//     // proba["sbg"] =   50'000.0 / 7;
//     // proba["cbg"] =   50'000.0 / 7;
//     // proba["atom"] =  50'000.0 / 7;

//     proba["atom"] =  100'000; // tmp
// }

static void init_map(std::map<std::string, unsigned long>& proba) {
    /* each below proba is between 0 and 100'000 */
    proba["prog"] =     100'000;
    proba["sentence"] = 100'000;
    proba["pw"] =       100'000;
    proba["term"] =     100'000;
    proba["word"] =     100'000;

    // init_words(proba);
    /* all program words proba sum up to 100'000 */

    // proba["sbt"] =   50'000;

    // proba["assoc"] = 50'000.0 / 7;
    // proba["ppg"] =   50'000.0 / 7;
    // proba["psbg"] =  50'000.0 / 7;
    // proba["pg"] =    50'000.0 / 7;

    // proba["sbg"] =   50'000.0 / 7;
    // proba["cbg"] =   50'000.0 / 7;
    // proba["atom"] =  50'000.0 / 7;

    /* tmp */
    proba["cbg"] = 50'000;
    proba["atom"] =  50'000;
}

LV1AstGenerator::LV1AstGenerator(int seed) : rng(seed), rand(1, 100'000) {
    init_map(proba);
}

LV1AstGenerator::LV1AstGenerator() : rng(std::random_device{}()), rand(1, 100'000) {
    init_map(proba);
}

LV1AstGenerator::RoutineRAII::RoutineRAII(LV1AstGenerator* gen) : gen(gen), is_parent_call_val(gen->is_parent_call) {
    gen->is_parent_call = false;
}

LV1AstGenerator::RoutineRAII::~RoutineRAII() {
    if (is_parent_call_val) {
        init_map(gen->proba);
        gen->is_parent_call = true;
    }
}

#define ENTERING_GENERATION_ROUTINE() auto __routine_raii = RoutineRAII{this}

LV1::Program LV1AstGenerator::generateProgram() {
    ENTERING_GENERATION_ROUTINE();

    std::vector<ProgramSentence> sentences;

    while (proba["sentence"] >= rand(rng)) {
        sentences.push_back(generateProgramSentence());
    }

    return LV1::Program{sentences};
}

ProgramSentence LV1AstGenerator::generateProgramSentence() {
    ENTERING_GENERATION_ROUTINE();
    std::vector<ProgramWord> programWords;

    while (proba["pw"] >= rand(rng)) {
        programWords.push_back(generateProgramWord());
    }
    proba["pw"] = 100'000; // reset pw proba

    proba["sentence"] = proba["sentence"] == 100'000? 90'000:
                        proba["sentence"] ==          90'000? 80'000:
                        proba["sentence"] ==                  80'000? 50'000:
                        proba["sentence"] ==                          50'000? 10'000:
                                                                              10'000;
    return ProgramSentence{programWords};
}

ProgramWord LV1AstGenerator::generateProgramWord() {
    ENTERING_GENERATION_ROUTINE();
    long r = rand(rng);
    long acc = 0;

    ProgramWord pw;
    if ((acc += proba["sbt"]) >= r) {
        pw = move_to_heap(generateSquareBracketsTerm());
    }
    else if ((acc += proba["assoc"]) >= r) {
        pw = move_to_heap(generateAssociation());
    }
    else if ((acc += proba["ppg"]) >= r) {
        pw = move_to_heap(generatePostfixParenthesesGroup());
    }
    else if ((acc += proba["psbg"]) >= r) {
        pw = move_to_heap(generatePostfixSquareBracketsGroup());
    }
    else if ((acc += proba["pg"]) >= r) {
        pw = move_to_heap(generateParenthesesGroup());
    }
    else if ((acc += proba["sbg"]) >= r) {
        pw = move_to_heap(generateSquareBracketsGroup());
    }
    else if ((acc += proba["cbg"]) >= r) {
        pw = move_to_heap(generateCurlyBracketsGroup());
    }
    else if ((acc += proba["atom"]) >= r) {
        pw = move_to_heap(generateAtom());
    }
    else {
        SHOULD_NOT_HAPPEN(); // bug
    }

    proba["pw"] = proba["pw"] == 100'000? 70'000:
                  proba["pw"] ==          70'000? 50'000:
                  proba["pw"] ==                  50'000? 30'000:
                  proba["pw"] ==                          30'000? 10'000:
                                                                  10'000;
    return pw;
}

Atom LV1AstGenerator::generateAtom() {
    ENTERING_GENERATION_ROUTINE();
    static char c = 'a'; // a-z
    defer {c = (c - 'a' + 1) % 26 + 'a';};

    return Atom{std::string(1, c)};
}

CurlyBracketsGroup LV1AstGenerator::generateCurlyBracketsGroup() {
    ENTERING_GENERATION_ROUTINE();
    auto backup = std::map<std::string, unsigned long>(proba);
    init_map(proba); // reset probas
    update_word_proba("cbg", 0, proba);

    std::vector<ProgramSentence> sentences;
    while (proba["sentence"] >= rand(rng)) {
        sentences.push_back(generateProgramSentence());
    }

    proba = backup; // restore saved word probas
    update_word_proba("cbg", 0, proba);

    return CurlyBracketsGroup{sentences};
}

Term LV1AstGenerator::generateTerm() {
    // todo
}

Word LV1AstGenerator::generateWord() {
    // todo
}

SquareBracketsTerm LV1AstGenerator::generateSquareBracketsTerm() {
    // todo
}

Association LV1AstGenerator::generateAssociation() {
    // todo
}

PostfixParenthesesGroup LV1AstGenerator::generatePostfixParenthesesGroup() {
    // todo
}

PostfixSquareBracketsGroup LV1AstGenerator::generatePostfixSquareBracketsGroup() {
    // todo
}

ParenthesesGroup LV1AstGenerator::generateParenthesesGroup() {
    // todo
}

SquareBracketsGroup LV1AstGenerator::generateSquareBracketsGroup() {
    // todo
}
