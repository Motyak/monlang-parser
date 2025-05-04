#ifndef AST_NUMERAL_H
#define AST_NUMERAL_H

#include <string>

struct Numeral {
    std::string fmt; // original text
    /* individual part of the numeral */
    std::string type; // int,hex,bin,oct,frac,pow,fix_only,per_only,fix_and_per
    std::string int1; // used everywhere
    std::string int2; // used in 'pow and 'div
    std::string fixed; // used in 'fix_only and 'fix_and_per
    std::string periodic; // used in 'per_only and 'fix_and_per

    size_t _tokenLen = 0;
    size_t _groupNesting = 0; // special case, because can't be malformed
    Numeral() = default;
    Numeral(
        const std::string& fmt,
        const std::string& type,
        const std::string& int1,
        const std::string& int2,
        const std::string& fixed,
        const std::string& periodic
    );
};

#endif // AST_NUMERAL_H
