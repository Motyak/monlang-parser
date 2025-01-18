#ifndef LV2_ENTITY_MAP_H
#define LV2_ENTITY_MAP_H

#include <monlang-LV2/visitors/visitor.h>

template <typename T>
struct EntityEqual;

template <>
struct EntityEqual<LV2::Ast_> {
    // the entrypoint
    bool operator()(const LV2::Ast_& a,
                    const LV2::Ast_& b) const;

    ///

    bool operator()(const MayFail<MayFail_<LV2::Program>>& prog_a,
                    const MayFail<MayFail_<LV2::Program>>& prog_b) const;

    bool operator()(const MayFail<Statement_>& stmt_a,
                    const MayFail<Statement_>& stmt_b) const;

    bool operator()(const MayFail<Expression_>& expr_a,
                    const MayFail<Expression_>& expr_b) const;
};

//////////////////////////////////////////////////////////////

template <>
struct std::hash<MayFail<Expression_>> {
    size_t operator()(const MayFail<Expression_>&) const;
};

template <>
struct std::hash<MayFail<Statement_>> {
    size_t operator()(const MayFail<Statement_>&) const;
};

template <>
struct std::hash<MayFail<MayFail_<LV2::Program>>> {
    std::hash<MayFail<Statement_>> hash_stmt;

    size_t operator()(const MayFail<MayFail_<LV2::Program>>&) const;
};

#endif // LV2_ENTITY_MAP_H
