#include <monlang-LV2/expr/Lambda.h>

#include <monlang-LV2/expr/Symbol.h>

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
    unless (std::holds_alternative<ParenthesesGroup*>(assoc.leftPart)) {
        return false;
    }
    unless (std::holds_alternative<CurlyBracketsGroup*>(assoc.rightPart)) {
        return false;
    }

    auto rightPart = *std::get<CurlyBracketsGroup*>(assoc.rightPart);
    unless (!rightPart._dollars) {
        return false;
    }

    auto leftPart = *std::get<ParenthesesGroup*>(assoc.leftPart);
    for (auto term: leftPart.terms) {
        ASSERT (!term.words.empty());

        unless (term.words.size() <= 2) {
            return false;
        }

        if (term.words.size() == 2) {
            auto leading_word = term.words[0];
            unless (std::holds_alternative<Atom*>(leading_word)) {
                return false;
            }

            unless (std::get<Atom*>(leading_word)->value == "OUT") {
                return false;
            }
        }

        auto last_word = term.words.back();
        unless (peekSymbol(last_word)) {
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
    std::vector<Symbol> parameters;
    std::optional<Symbol> variadicParameters;
    for (size_t i = 0; i < leftPart.terms.size(); ++i) {
        auto term = leftPart.terms.at(i);
        ASSERT (term.words.size() > 0);
        Word last_word = term.words.back();
        ASSERT (std::holds_alternative<Atom*>(last_word));
        auto atom = *std::get<Atom*>(last_word);
        auto symbol = Symbol{atom.value};
        symbol._tokenLen = term._tokenLen;
        symbol._tokenId = atom._tokenId;
        if (i == leftPart.terms.size() - 1 && atom.value.ends_with("...")) {
            variadicParameters = symbol;
            break;
        }
        parameters.push_back(symbol);
    }

    auto rightPart = *std::get<CurlyBracketsGroup*>(assoc.rightPart);
    MayFail_<LambdaBlock> body;
    until (rightPart.sentences.empty()) {
        auto statement = consumeStatement(rightPart);
        body.statements.push_back(statement);
        if (statement.has_error()) {
            body._oneline = (bool)rightPart.term;
            return Malformed(MayFail_<Lambda>{parameters, variadicParameters, body}, ERR(631));
        }
    }

    body._dollars = rightPart._dollars;
    body._oneline = (bool)rightPart.term;
    body._tokenLen = rightPart._tokenLen;

    auto lambda = MayFail_<Lambda>{parameters, variadicParameters, body};
    lambda._tokenLen = assoc._tokenLen;
    return lambda;
}

Lambda::Lambda(
    const std::vector<Symbol>& parameters,
    const std::optional<Symbol>& variadicParameters, 
    const LambdaBlock& body
) : parameters(parameters), variadicParameters(variadicParameters), body(body){}

MayFail_<Lambda>::MayFail_(
    const std::vector<Symbol>& parameters,
    const std::optional<Symbol>& variadicParameters,
    const MayFail_<LambdaBlock>& body
) : parameters(parameters), variadicParameters(variadicParameters), body(body){}

MayFail_<Lambda>::MayFail_(const Lambda& lambda) {
    std::vector<MayFail<Statement_>> bodyStatements;
    for (auto e: lambda.body.statements) {
        bodyStatements.push_back(wrap_stmt(e));
    }
    this->parameters = lambda.parameters;
    this->variadicParameters = lambda.variadicParameters;
    this->body = MayFail_<LambdaBlock>{bodyStatements};
    this->_tokenLen = lambda._tokenLen;
    this->_tokenId = lambda._tokenId;
}

MayFail_<Lambda>::operator Lambda() const {
    std::vector<Statement> bodyStatements;
    for (auto e: this->body.statements) {
        bodyStatements.push_back(unwrap_stmt(e.value()));
    }
    auto lambda = Lambda{
        this->parameters,
        this->variadicParameters,
        LambdaBlock{bodyStatements}
    };
    lambda._tokenLen = this->_tokenLen;
    lambda._tokenId = this->_tokenId;
    return lambda;
}
