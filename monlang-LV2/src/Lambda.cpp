#include <monlang-LV2/Lambda.h>

#include <monlang-LV1/ast/Association.h>
#include <monlang-LV1/ast/ParenthesesGroup.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/Atom.h>

#include <utils/assert-utils.h>

#define unless(x) if(!(x))
#define until(x) while(!(x))

bool peekLambda(const Word& word) {
    unless (std::holds_alternative<Association*>(word)) {
        return false;
    }

    auto assoc = *std::get<Association*>(word);
    unless (std::holds_alternative<ParenthesesGroup*>(assoc.leftPart) \
            && std::holds_alternative<CurlyBracketsGroup*>(assoc.rightPart)) {
        return false;
    }

    auto leftPart = *std::get<ParenthesesGroup*>(assoc.leftPart);
    std::vector<identifier_t> parameters;
    for (auto term: leftPart.terms) {
        unless (term.words.size() == 1 && std::holds_alternative<Atom*>(term.words[0])) {
            return false;
        }
    }

    return true;
}

Lambda buildLambda(const Word& word, const context_t& cx) {
    ASSERT (std::holds_alternative<Association*>(word));
    auto assoc = *std::get<Association*>(word);

    ASSERT (std::holds_alternative<ParenthesesGroup*>(assoc.leftPart));
    ASSERT (std::holds_alternative<CurlyBracketsGroup*>(assoc.rightPart));

    auto leftPart = *std::get<ParenthesesGroup*>(assoc.leftPart);
    std::vector<identifier_t> parameters;
    for (auto term: leftPart.terms) {
        ASSERT (term.words.size() == 1);
        ASSERT (std::holds_alternative<Atom*>(term.words[0]));
        parameters.push_back(std::get<Atom*>(term.words[0])->value);
    }

    auto rightPart = *std::get<CurlyBracketsGroup*>(assoc.rightPart);
    LambdaBlock body;
    until (rightPart.sentences.empty()) {
        auto statement = consumeStatement(rightPart, cx);
        body.statements.push_back(statement);
    }

    return Lambda{parameters, body};
}
