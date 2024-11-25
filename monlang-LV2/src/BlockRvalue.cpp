#include <monlang-LV2/BlockRvalue.h>

#include <monlang-LV1/ast/CurlyBracketsGroup.h>

#include <utils/assert-utils.h>

bool peekBlockRvalue(const Word& word) {
    // don't we need to differenciate different types of "block" ? will see
    return std::holds_alternative<CurlyBracketsGroup*>(word);
}

BlockRvalue buildBlockRvalue(const Word& word, const context_t& cx) {
    ASSERT (!cx.fallthrough);
    ASSERT (std::holds_alternative<CurlyBracketsGroup*>(word));
    auto cbg = *std::get<CurlyBracketsGroup*>(word);

    std::vector<Statement> statements;
    for (auto sentence: cbg.sentences) {
        auto statement = consumeStatement((Subprogram&)cbg, cx);
        if (cx.fallthrough) {
            return BlockRvalue(); // stub
        }
        statements.push_back(statement);
    }

    return BlockRvalue{statements};
}
