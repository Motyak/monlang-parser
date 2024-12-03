#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H

#include <monlang-LV2/Expression.h>
#include <monlang-LV2/context.h>

#include <optional>

struct ReturnStatement {
    std::optional<Expression> value;
};

bool peekReturnStatement(const ProgramSentence&);

ReturnStatement buildReturnStatement(const ProgramSentence&, context_t* = new context_t{});

#endif // RETURN_STATEMENT_H
