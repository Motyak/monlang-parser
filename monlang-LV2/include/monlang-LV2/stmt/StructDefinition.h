#ifndef STRUCT_DEFINITION_H
#define STRUCT_DEFINITION_H

#include <monlang-LV2/ast/stmt/StructDefinition.h>
#include <monlang-LV2/common.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<StructDefinition> {
    Symbol struct_;
    std::vector<MayFail<StructDefinition::Field>> fields;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0; // TODO: no need
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const Symbol&, const std::vector<MayFail<StructDefinition::Field>>&);

    explicit MayFail_(const StructDefinition&);
    explicit operator StructDefinition() const;
};

bool peekStructDefinition(const ProgramSentence&);

MayFail<MayFail_<StructDefinition>> consumeStructDefinition(LV1::Program&);

#endif // STRUCT_DEFINITION_H
