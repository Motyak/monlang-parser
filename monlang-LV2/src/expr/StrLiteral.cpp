#include <monlang-LV2/expr/StrLiteral.h>

/* impl only */
#include <monlang-LV1/ast/Quotation.h>

#include <utils/assert-utils.h>

bool peekStrLiteral(const Word& word) {
    return std::holds_alternative<Quotation*>(word);
}

StrLiteral buildStrLiteral(const Word& word) {
    ASSERT (std::holds_alternative<Quotation*>(word));
    auto quotation = *std::get<Quotation*>(word);

    auto strLiteral = StrLiteral{quotation.quoted};
    strLiteral._tokenLen = quotation._tokenLen;
    return strLiteral;
}

StrLiteral::StrLiteral(const std::string& str) : str(str){}
