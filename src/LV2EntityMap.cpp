#include <monlang-parser/LV2EntityMap.h>

//////////////////////////////////////////////////////////////
// EntityEqual
//////////////////////////////////////////////////////////////

bool EntityEqual<LV2::Ast_>::operator()(const LV2::Ast_& a, const LV2::Ast_& b) const {
    if (a.index() != b.index()) {
        return false;
    }

    return std::visit(
        [this, &b](auto mf_a){
            auto mf_b = std::get<decltype(mf_a)>(b);
            return operator()(mf_a, mf_b);
        },
        a
    );
}

//////////////////////////////////////////////////////////////

bool EntityEqual<LV2::Ast_>::operator()(const MayFail<MayFail_<LV2::Program>>& prog_a, const MayFail<MayFail_<LV2::Program>>& prog_b) const {
    if (prog_a.has_error() != prog_b.has_error()) {
        return false;
    }
    if (prog_a.has_error() && prog_a.err->code != prog_b.err->code) {
        return false;
    }
    if (prog_a.val.statements.size() != prog_b.val.statements.size()) {
        return false;
    }
    for (size_t i = 0; i < prog_a.val.statements.size(); ++i) {
        if (false == operator()(prog_a.val.statements[i], prog_b.val.statements[i])) {
            return false;
        }
    }
    return true;
}

bool EntityEqual<LV2::Ast_>::operator()(const MayFail<Statement_>& stmt_a, const MayFail<Statement_>& stmt_b) const {
    if (stmt_a.has_error() != stmt_b.has_error()) {
        return false;
    }
    if (stmt_a.has_error() && stmt_a.err->code != stmt_b.err->code) {
        return false;
    }
    return std::visit(
        [&stmt_b](auto* ptr_a){
            auto* ptr_b = std::get<decltype(ptr_a)>(stmt_b.val);
            return reinterpret_cast<uintptr_t>(ptr_a)
                    == reinterpret_cast<uintptr_t>(ptr_b);
        },
        stmt_a.val
    );
}

bool EntityEqual<LV2::Ast_>::operator()(const MayFail<Expression_>& expr_a, const MayFail<Expression_>& expr_b) const {
    if (expr_a.has_error() != expr_b.has_error()) {
        return false;
    }
    if (expr_a.has_error() && expr_a.err->code != expr_b.err->code) {
        return false;
    }
    return std::visit(
        [&expr_b](auto* ptr_a){
            auto* ptr_b = std::get<decltype(ptr_a)>(expr_b.val);
            return reinterpret_cast<uintptr_t>(ptr_a)
                    == reinterpret_cast<uintptr_t>(ptr_b);
        },
        expr_a.val
    );
}

//////////////////////////////////////////////////////////////
// std::hash
//////////////////////////////////////////////////////////////

size_t std::hash<MayFail<MayFail_<LV2::Program>>>::operator()(const MayFail<MayFail_<LV2::Program>>& prog) const {
    for (auto stmt: prog.val.statements) {
        return hash_stmt(stmt);
    }
    return prog.has_error()? 9000 : 1000;
}

size_t std::hash<MayFail<Statement_>>::operator()(const MayFail<Statement_>& stmt) const {
    return std::visit(
        [](auto* ptr){return reinterpret_cast<uintptr_t>(ptr);},
        stmt.val
    );
}

size_t std::hash<MayFail<Expression_>>::operator()(const MayFail<Expression_>& expr) const {
    return std::visit(
        [](auto* ptr){return reinterpret_cast<uintptr_t>(ptr);},
        expr.val
    );
}
