#ifndef LV2_COMMON_H
#define LV2_COMMON_H

#include <monlang-LV1/Program.h>
#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>

ProgramSentence consumeSentence(LV1::Program&);

Term toTerm(const ProgramSentence&);

#endif // LV2_COMMON_H
