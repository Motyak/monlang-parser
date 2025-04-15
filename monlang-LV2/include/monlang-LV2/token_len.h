/*
    standalone header, no .cpp

    This header should be included when one..
    ..needs to use token_len on ALL definitions..
    ..of LV2

    If you only need token_len for a couple of entities..
    .., then simply include monlang-LV1/ast/common.h and..
    ..the corresponding entity headers
*/

#ifndef LV2_TOKEN_LEN_H
#define LV2_TOKEN_LEN_H

#include <monlang-LV1/ast/common.h> // token_len() set_token_len() templates

/* all LV2  entities */

#include <monlang-LV2/expr/Operation.h>
#include <monlang-LV2/expr/FunctionCall.h>
#include <monlang-LV2/expr/Lambda.h>
#include <monlang-LV2/expr/BlockExpression.h>
#include <monlang-LV2/expr/FieldAccess.h>
#include <monlang-LV2/expr/Subscript.h>
#include <monlang-LV2/expr/MapLiteral.h>
#include <monlang-LV2/expr/ListLiteral.h>
#include <monlang-LV2/expr/StrLiteral.h>
#include <monlang-LV2/expr/Numeral.h>
#include <monlang-LV2/expr/SpecialSymbol.h>
#include <monlang-LV2/expr/Symbol.h>

#include <monlang-LV2/stmt/Assignment.h>
#include <monlang-LV2/stmt/Accumulation.h>
#include <monlang-LV2/stmt/LetStatement.h>
#include <monlang-LV2/stmt/VarStatement.h>
#include <monlang-LV2/stmt/ReturnStatement.h>
#include <monlang-LV2/stmt/BreakStatement.h>
#include <monlang-LV2/stmt/ContinueStatement.h>
#include <monlang-LV2/stmt/DieStatement.h>
#include <monlang-LV2/stmt/Guard.h>
#include <monlang-LV2/stmt/ForeachStatement.h>
#include <monlang-LV2/stmt/WhileStatement.h>
#include <monlang-LV2/stmt/ExpressionStatement.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

template <>
size_t token_len(const Expression_& variant) {
    return std::visit(overload{
        [](_StubExpression_*) -> size_t {SHOULD_NOT_HAPPEN();},
        [](auto* expr) -> size_t {return expr->_tokenLen;},
    }, variant);
}

#endif // LV2_TOKEN_LEN_H
