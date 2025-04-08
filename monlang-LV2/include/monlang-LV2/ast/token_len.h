/*
    standalone header, no .cpp

    This header should be included when one..
    ..needs to use token_len on ALL definitions..
    ..of LV2 ast/

    If you only need token_len for a couple of entities..
    .., then simply include monlang-LV1/ast/common.h and..
    ..the corresponding entity headers
*/

#ifndef LV2_AST_TOKEN_LEN_H
#define LV2_AST_TOKEN_LEN_H

#include <monlang-LV1/ast/common.h> // token_len() set_token_len() templates

/* all LV2 ast/ entities */

#include <monlang-LV2/ast/expr/Operation.h>
#include <monlang-LV2/ast/expr/FunctionCall.h>
#include <monlang-LV2/ast/expr/Lambda.h>
#include <monlang-LV2/ast/expr/BlockExpression.h>
#include <monlang-LV2/ast/expr/FieldAccess.h>
#include <monlang-LV2/ast/expr/Subscript.h>
#include <monlang-LV2/ast/expr/MapLiteral.h>
#include <monlang-LV2/ast/expr/ListLiteral.h>
#include <monlang-LV2/ast/expr/StrLiteral.h>
#include <monlang-LV2/ast/expr/Numeral.h>
#include <monlang-LV2/ast/expr/SpecialSymbol.h>
#include <monlang-LV2/ast/expr/Symbol.h>

#include <monlang-LV2/ast/stmt/Assignment.h>
#include <monlang-LV2/ast/stmt/Accumulation.h>
#include <monlang-LV2/ast/stmt/LetStatement.h>
#include <monlang-LV2/ast/stmt/VarStatement.h>
#include <monlang-LV2/ast/stmt/ReturnStatement.h>
#include <monlang-LV2/ast/stmt/BreakStatement.h>
#include <monlang-LV2/ast/stmt/ContinueStatement.h>
#include <monlang-LV2/ast/stmt/DieStatement.h>
#include <monlang-LV2/ast/stmt/Guard.h>
#include <monlang-LV2/ast/stmt/ForeachStatement.h>
#include <monlang-LV2/ast/stmt/WhileStatement.h>
#include <monlang-LV2/ast/stmt/ExpressionStatement.h>

#endif // LV2_AST_TOKEN_LEN_H
