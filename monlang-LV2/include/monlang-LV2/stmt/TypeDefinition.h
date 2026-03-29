#ifndef TYPE_DEFINITION_H
#define TYPE_DEFINITION_H

#include <monlang-LV2/ast/stmt/TypeDefinition.h>
#include <monlang-LV2/common.h>

#include <monlang-LV1/ast/Program.h>

bool peekTypeDefinition(const ProgramSentence&);

MayFail<TypeDefinition> consumeTypeDefinition(LV1::Program&);

#endif // TYPE_DEFINITION_H
