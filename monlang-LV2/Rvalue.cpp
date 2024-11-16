#include <monlang-LV2/Rvalue.h>

#include <utils/assert-utils.h>

Rvalue buildRvalue(const Term& term) {
    ASSERT (term.words.size() > 0);

    // if (term =~ "RVALUE (BINARY-OPERATOR)*"_) {
    //     return buildOperation(term);
    // }

    // ...

    // throw exception
}
