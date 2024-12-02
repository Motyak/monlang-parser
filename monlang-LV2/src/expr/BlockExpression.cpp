#include <monlang-LV2/expr/BlockExpression.h>

#include <monlang-LV1/ast/CurlyBracketsGroup.h>

#include <utils/assert-utils.h>

#define until(x) while(!(x))

bool peekBlockExpression(const Word& word) {
    // don't we need to differenciate different types of "block" ? will see
    return std::holds_alternative<CurlyBracketsGroup*>(word);
}

BlockExpression buildBlockExpression(const Word& word, const context_t& cx) {
    ASSERT (!cx.fallthrough);
    ASSERT (std::holds_alternative<CurlyBracketsGroup*>(word));
    auto cbg = *std::get<CurlyBracketsGroup*>(word);


    std::vector<Statement> statements;
    until (cbg.sentences.empty()) {
        auto statement = consumeStatement((Subprogram&)cbg);
        // auto statement = consumeStatement((Subprogram&)cbg, cx);
        if (cx.malformed_stmt || cx.fallthrough) {
            return BlockExpression(); // stub
        }
        statements.push_back(statement);
    }

    return BlockExpression{statements};
}
