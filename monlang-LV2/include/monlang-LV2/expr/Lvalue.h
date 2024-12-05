#ifndef LVALUE_H
#define LVALUE_H

#include <monlang-LV2/ast/expr/Lvalue.h>

#include <monlang-LV2/common.h>

#include <monlang-LV1/ast/Word.h>

bool peekLvalue(const Word&);

MayFail<Lvalue> buildLvalue(const Word&);

#endif // LVALUE_H
