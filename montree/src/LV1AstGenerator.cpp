#include <montree/LV1AstGenerator.h>

#include <utils/mem-utils.h>
#include <utils/assert-utils.h>
#include <utils/defer-util.h>

static void update_word_proba(std::string key, long new_value, std::map<std::string, long>& proba) {
    ASSERT (proba.find(key) != proba.end());
    ASSERT (0 <= new_value && new_value <= 100'000);
    long diff = new_value - proba[key];
    proba[key] = new_value;

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
    std::vector other_words(WORDS.begin(), WORDS.end());
    std::erase_if(other_words, [&key, &proba](auto word){return word == key || proba[word] == 0;});

    for (auto other_word: other_words) {
        proba[other_word] += double(diff / other_words.size());
    }
}

static void initmap(std::map<std::string, long>& proba) {
    /* each below proba is between 0 and 100'000 */
    proba["prog"] =     100'000;
    proba["sentence"] = 100'000;
    proba["pw"] =       100'000;
    proba["term"] =     100'000;
    proba["word"] =     100'000;

    /* all program words proba sum up to 100'000 */

    // proba["sbt"] =   50'000;

    // proba["assoc"] = 50'000.0 / 7;
    // proba["ppg"] =   50'000.0 / 7;
    // proba["psbg"] =  50'000.0 / 7;
    // proba["pg"] =    50'000.0 / 7;

    // proba["sbg"] =   50'000.0 / 7;
    // proba["cbg"] =   50'000.0 / 7;
    // proba["atom"] =  50'000.0 / 7;

    proba["atom"] =  100'000; // tmp
}

LV1AstGenerator::LV1AstGenerator(int seed) : rng(seed), rand(1, 100'000) {
    initmap(proba);
}

LV1AstGenerator::LV1AstGenerator() : rng(std::random_device{}()), rand(1, 100'000) {
    initmap(proba);
}

LV1::Program LV1AstGenerator::generateProgram() {
    std::vector<ProgramSentence> sentences;

    while (proba["sentence"] >= rand(rng)) {
        sentences.push_back(generateProgramSentence());
    }
    proba["sentence"] = 100'000; // reset sentence proba

    proba["prog"] = 0;
    return LV1::Program{sentences};
}

ProgramSentence LV1AstGenerator::generateProgramSentence() {
    std::vector<ProgramWord> programWords;

    while (proba["pw"] >= rand(rng)) {
        programWords.push_back(generateProgramWord());
    }
    proba["pw"] = 100'000; // reset pw proba
    // update_pw_proba("sbt", 50'000, proba); // reset sbt proba

    proba["sentence"] = proba["sentence"] == 100'000? 90'000:
                        proba["sentence"] ==          90'000? 80'000:
                        proba["sentence"] ==                  80'000? 50'000:
                        proba["sentence"] ==                          50'000? 10'000:
                                                                              10'000;
    return ProgramSentence{programWords};
}

ProgramWord LV1AstGenerator::generateProgramWord() {
    long r = rand(rng);
    long acc = 0;

    std::string key;
    ProgramWord pw;
    if ((acc += proba[key = "sbt"]) >= r) {
        pw = move_to_heap(generateSquareBracketsTerm());
    }
    else if ((acc += proba[key = "assoc"]) >= r) {
        pw = move_to_heap(generateAssociation());
    }
    else if ((acc += proba[key = "ppg"]) >= r) {
        pw = move_to_heap(generatePostfixParenthesesGroup());
    }
    else if ((acc += proba[key = "psbg"]) >= r) {
        pw = move_to_heap(generatePostfixSquareBracketsGroup());
    }
    else if ((acc += proba[key = "pg"]) >= r) {
        pw = move_to_heap(generateParenthesesGroup());
    }
    else if ((acc += proba[key = "sbg"]) >= r) {
        pw = move_to_heap(generateSquareBracketsGroup());
    }
    else if ((acc += proba[key = "cbg"]) >= r) {
        pw = move_to_heap(generateCurlyBracketsGroup());
    }
    else if ((acc += proba[key = "atom"]) >= r) {
        pw = move_to_heap(generateAtom());
    }
    else {
        SHOULD_NOT_HAPPEN(); // bug
    }

    if (key != "atom") {
        update_word_proba(key, 0, proba);
    }

    proba["pw"] = proba["pw"] == 100'000? 70'000:
                  proba["pw"] ==          70'000? 50'000:
                  proba["pw"] ==                  50'000? 30'000:
                  proba["pw"] ==                          30'000? 10'000:
                                                                  10'000;
    return pw;
}

Atom LV1AstGenerator::generateAtom() {
    static char c = 'a'; // a-z
    defer {c = (c - 'a' + 1) % 26 + 'a';};

    return Atom{std::string(1, c)};
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

CurlyBracketsGroup LV1AstGenerator::generateCurlyBracketsGroup() {
    // todo
}
