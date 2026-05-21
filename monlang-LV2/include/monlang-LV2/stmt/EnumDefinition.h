#ifndef ENUM_DEFINITION_H
#define ENUM_DEFINITION_H

#include <monlang-LV2/ast/stmt/EnumDefinition.h>

#include <monlang-LV2/common.h>
#include <monlang-LV2/Expression.h>

#include <monlang-LV1/ast/Program.h>

template <>
struct MayFail_<EnumDefinition> {
    Symbol enum_;
    struct EnumValue {
        struct Pair {
            Symbol enumerator; // enum value "name"
            MayFail<Expression_> enumerate; // enum value "value"
        };
        std::optional<Pair> pair;

        size_t _tokenLeadingNewlines = 0;
        size_t _tokenIndentSpaces = 0;
        size_t _tokenLen = 0;
        size_t _tokenTrailingNewlines = 0;
        size_t _tokenId = 123456789;

        EnumValue() = default;
        EnumValue(const Symbol&, const MayFail<Expression_>&);
    };
    std::vector<MayFail<MayFail_<EnumDefinition>::EnumValue>> enumValues;

    size_t _tokenLeadingNewlines = 0;
    size_t _tokenIndentSpaces = 0; // TODO: no need
    size_t _tokenLen = 0;
    size_t _tokenTrailingNewlines = 0;
    size_t _tokenId = 123456789;
    MayFail_() = default;
    explicit MayFail_(const Symbol&, const std::vector<MayFail<MayFail_<EnumDefinition>::EnumValue>>&);

    explicit MayFail_(const EnumDefinition&);
    explicit operator EnumDefinition() const;
};

bool peekEnumDefinition(const ProgramSentence&);

MayFail<MayFail_<EnumDefinition>> consumeEnumDefinition(LV1::Program&);

#endif // ENUM_DEFINITION_H
