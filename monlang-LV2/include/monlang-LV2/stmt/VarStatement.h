#ifndef VAR_STATEMENT_H
#define VAR_STATEMENT_H

#include <monlang-LV2/ast/stmt/VarStatement.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<VarStatement> {
    identifier_t identifier;
    MayFail<Expression_> value;
};

bool peekVarStatement(const ProgramSentence&);

MayFail<MayFail_<VarStatement>> consumeVarStatement(LV1::Program&);

#endif // VAR_STATEMENT_H
