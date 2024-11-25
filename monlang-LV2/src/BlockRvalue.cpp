#include <monlang-LV2/BlockRvalue.h>

#include <monlang-LV1/ast/CurlyBracketsGroup.h>

#include <utils/assert-utils.h>

bool peekBlockRvalue(const Word& word) {
    // don't we need to differenciate different types of "block" ? will see
    return std::holds_alternative<CurlyBracketsGroup*>(word);
}

BlockRvalue buildBlockRvalue(const Word& word, const context_t& cx) {
    ASSERT (std::holds_alternative<CurlyBracketsGroup*>(word));
    auto cbg = *std::get<CurlyBracketsGroup*>(word);
    ASSERT (cbg.sentences.size() > 0);

    std::vector<Statement> statements;
    for (auto sentence: cbg.sentences) {
        statements.push_back(consumeStatement((Subprogram&)cbg, cx));
    }

    return BlockRvalue{statements};
}
