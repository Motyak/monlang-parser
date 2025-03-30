#include <monlang-LV2/Expression.h>

/* impl only */
#include <monlang-LV2/precedence.h> // fixPrecedence()
#include <monlang-LV2/expr/Operation.h>
#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/expr/Lambda.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/expr/StrLiteral.h>
#include <monlang-LV2/expr/MapLiteral.h>
#include <monlang-LV2/expr/ListLiteral.h>
#include <monlang-LV2/expr/Numeral.h>
#include <monlang-LV2/expr/SpecialSymbol.h>
#include <monlang-LV2/expr/Symbol.h>

/* require knowing all words for token_len() */
#include <monlang-LV1/ast/token_len.h>

#include <utils/assert-utils.h>
#include <utils/mem-utils.h>
#include <utils/vec-utils.h>
#include <utils/variant-utils.h>

#define unless(x) if (!(x))

MayFail<Expression_> buildExpression(const Term& term) {
    ASSERT (term.words.size() > 0);
    auto term_ = term; // local non-const working variable
    size_t groupNesting = 0;

    BEGIN:

    unless (term_.words.size() % 2 == 1) {
        auto expr = StubExpression_();
        set_group_nesting(expr, groupNesting);
        return Malformed<Expression_>(expr, ERR(161));
    }

    /* odd-indexed words, starting at [1], must be Atoms..
        ..which value matches an operator from the precedence table */
    for (size_t i = 1; i < term_.words.size(); i += 2) {
        auto optr_found = false;

        for (auto [operators, _]: PRECEDENCE_TABLE) {
            unless (std::holds_alternative<Atom*>(term_.words[i])) {
                auto expr = StubExpression_();
                set_group_nesting(expr, groupNesting);
                return Malformed<Expression_>(expr, ERR(162));
            }
            auto optr = std::get<Atom*>(term_.words[i])->value;

            if (vec_contains(operators, optr)) {
                optr_found = true;
                break;
            }
        }

        unless (optr_found) {
            auto expr = StubExpression_();
            set_group_nesting(expr, groupNesting);
            auto error = ERR(163);
            auto err_offset = size_t(0);
            for (size_t j = 0; j < i; ++j) {
                err_offset += token_len(term_.words[j]);
                err_offset += sequenceLen(Term::CONTINUATOR_SEQUENCE);
            }
            error._info["err_offset"] = err_offset;
            return Malformed<Expression_>(expr, error);
        }
    }

    // ASSERT (term_ =~ "Word (OPERATOR Word)*"_);

    fixPrecedence(term_);
    ASSERT (term_.words.size() == 1 || term_.words.size() == 3);

    // if (term_ =~ "Word Word Word"_) {
    //     ..
    // }

    if (peekOperation(term_)) {
        auto operation = buildOperation(term_);
        operation.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(operation);
    }

    // 'Operation' was the only Expression with multiple words
    ASSERT (term_.words.size() == 1);
    auto word = (Word)term_;

    // if (word =~ "PostfixParenthesesGroup"_) {
    //    ...
    // }

    if (peekFunctionCall(word)) {
        auto functionCall = buildFunctionCall(word);
        functionCall.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(functionCall);
    }

    if (peekLambda(word)) {
        auto lambda = buildLambda(word);
        lambda.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(lambda);
    }

    // if (word =~ "CurlyBracketsGroup"_) {
    //     ...
    // }

    if (peekBlockExpression(word)) {
        auto blockExpr = buildBlockExpression(word);
        blockExpr.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(blockExpr);
    }

    // if (word =~ "Quotation"_) {
    //     ...
    // }

    if (peekStrLiteral(word)) {
        auto strLiteral = buildStrLiteral(word);
        strLiteral._groupNesting = groupNesting;
        return (Expression_)move_to_heap(strLiteral);
    }

    if (peekMapLiteral(word)) {
        auto mapLiteral = buildMapLiteral(word);
        mapLiteral.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(mapLiteral);
    }

    // if (word =~ "SquareBracketsGroup"_) {
    //     ...
    // }

    // SBG expression fall-through
    if (std::holds_alternative<SquareBracketsGroup*>(word)) {
        auto listLiteral = buildListLiteral(word);
        listLiteral.val._groupNesting = groupNesting;
        return mayfail_convert<Expression_>(listLiteral);
    }

    // if (word =~ "Atom<$.*>"_) {
    //     ...
    // }

    if (peekSpecialSymbol(word)) {
        auto specialSymbol = buildSpecialSymbol(word);
        specialSymbol._groupNesting = groupNesting;
        return (Expression_)move_to_heap(specialSymbol);
    }

    // if (word =~ "Atom<[0-9]+>"_) {
    //     ...
    // }

    if (peekNumeral(word)) {
        auto numeral = buildNumeral(word);
        numeral._groupNesting = groupNesting;
        return (Expression_)move_to_heap(numeral);
    }

    // if (word =~ "Atom"_) {
    //     ...
    // }

    // Atom expression fall-through
    if (std::holds_alternative<Atom*>(word)) {
        auto symbol = buildSymbol(word);
        symbol._groupNesting = groupNesting;
        symbol._lvalue = true; // TODO: tmp, need more sophisticated approach
        return (Expression_)move_to_heap(symbol);
    }

    // if (word =~ "ParenthesesGroup<Term<Word+>+>"_) {
    //     ...
    // }

    if (std::holds_alternative<ParenthesesGroup*>(word)) {
        auto group = *std::get<ParenthesesGroup*>(word);
        // if grouped expression => unwrap then go back to beginning
        if (group.terms.size() == 1) {
            if (!group._implicit) {
                groupNesting += 1;
            }
            auto saveTokenLen = term_._tokenLen;
            term_ = group.terms[0];
            term_._tokenLen = saveTokenLen;

            goto BEGIN; // prevent unnecessary recursive call
        }
    }

    /* reached fall-through */
    auto expr = StubExpression_();
    set_group_nesting(expr, groupNesting);
    return Malformed<Expression_>(expr, ERR(169));
}

Expression_ StubExpression_() {
    return move_to_heap(_StubExpression_{});
}

Expression unwrap_expr(Expression_ expression) {
    return std::visit(overload{
        [](Numeral* expr) -> Expression {return expr;},
        [](StrLiteral* expr) -> Expression {return expr;},
        [](SpecialSymbol* expr) -> Expression {return expr;},
        [](Symbol* expr) -> Expression {return expr;},
        [](_StubExpression_*) -> Expression {SHOULD_NOT_HAPPEN();},
        [](auto* mf_) -> Expression {return move_to_heap(unwrap(*mf_));},
    }, expression);
}

Expression_ wrap_expr(Expression expression) {
    return std::visit(overload{
        [](Numeral* expr) -> Expression_ {return expr;},
        [](StrLiteral* expr) -> Expression_ {return expr;},
        [](SpecialSymbol* expr) -> Expression_ {return expr;},
        [](Symbol* expr) -> Expression_ {return expr;},
        [](auto* mf_) -> Expression_ {return move_to_heap(wrap(*mf_));},
    }, expression);
}

bool is_lvalue(Expression expr) {
    return std::visit(overload{
        [](Symbol* symbol){return symbol->_lvalue;},
        // [](Subscript* subscript){return subscript->_lvalue;},
        // [](FieldAccess* fieldAccess){return fieldAccess->_lvalue;},
        [](auto*){return false;},
    }, expr);
}

bool is_lvalue(Expression_ expr_) {
    return std::visit(overload{
        [](Symbol* symbol){return symbol->_lvalue;},
        // [](MayFail_<Subscript>* subscript){return subscript->_lvalue;},
        // [](MayFail_<FieldAccess>* fieldAccess){return fieldAccess->_lvalue;},
        [](auto*){return false;},
    }, expr_);
}

void delete_(Expression_ expr) {
    std::visit(
        [](auto* ptr){delete ptr;},
        expr
    );
}
