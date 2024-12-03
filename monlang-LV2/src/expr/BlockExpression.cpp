#include <monlang-LV2/expr/BlockExpression.h>

#include <monlang-LV1/ast/CurlyBracketsGroup.h>

#include <utils/assert-utils.h>

#define until(x) while(!(x))

bool peekBlockExpression(const Word& word) {
    // don't we need to differenciate different types of "block" ? will see
    return std::holds_alternative<CurlyBracketsGroup*>(word);
}

BlockExpression buildBlockExpression(const Word& word, context_t* cx) {
    ASSERT (!*cx->fallthrough);
    ASSERT (std::holds_alternative<CurlyBracketsGroup*>(word));
    auto cbg = *std::get<CurlyBracketsGroup*>(word);

    // create fresh context
    auto child_cx = context_t{}; // we use context_t{} instead of context_init_t{}..
               // ..because we don't want to deallocate memory at the end of scope..
               // ..(it's used by the returned entity)
    // save context
    auto parent_cx = *cx;
    // switch context
    *cx = child_cx;

    auto& statements = *std::any_cast<std::vector<Statement>*>(child_cx.statements);
    until (cbg.sentences.empty()) {
        auto statement = consumeStatement((Subprogram&)cbg, cx);
        if (*child_cx.malformed_stmt || *child_cx.fallthrough) {
            return BlockExpression(); // stub
        }
        statements.push_back(statement);
    }

    // restore context
    *cx = parent_cx;

    return BlockExpression{statements};
}
