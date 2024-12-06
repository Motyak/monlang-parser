#include <monlang-LV2/expr/Lambda.h>

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
    for (auto term: leftPart.terms) {
        unless (term.words.size() == 1 && std::holds_alternative<Atom*>(term.words[0])) {
            return false;
        }
    }

    return true;
}

MayFail<MayFail_<Lambda>> buildLambda(const Word& word) {
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
    MayFail_<LambdaBlock> body;
    until (rightPart.sentences.empty()) {
        auto statement = consumeStatement(rightPart);
        if (statement.has_error()) {
            return Malformed(MayFail_<Lambda>{parameters, body}, ERR(631));
        }
        body.statements.push_back(statement);
    }

    return MayFail_<Lambda>{parameters, body};
}

template <>
Lambda unwrap(const MayFail_<Lambda>& lambda) {
    std::vector<Statement> bodyStatements;
    for (auto e: lambda.body.statements) {
        bodyStatements.push_back(unwrap_stmt(e.value()));
    }
    return Lambda{lambda.parameters, LambdaBlock{bodyStatements}};
}

template <>
MayFail_<Lambda> wrap(const Lambda& lambda) {
    std::vector<MayFail<Statement_>> bodyStatements;
    for (auto e: lambda.body.statements) {
        bodyStatements.push_back(wrap_stmt(e));
    }
    return MayFail_<Lambda>{lambda.parameters, MayFail_<LambdaBlock>{bodyStatements}};
}
