#ifndef LV2_COMMON_H
#define LV2_COMMON_H

#include <monlang-LV1/ast/Program.h>
#include <monlang-LV1/ast/Term.h>

ProgramSentence consumeSentence(LV1::Program&);

Term toTerm(const ProgramSentence&);

#endif // LV2_COMMON_H
