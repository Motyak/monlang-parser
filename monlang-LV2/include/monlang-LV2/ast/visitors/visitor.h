#ifndef LV2_AST_VISITOR_H
#define LV2_AST_VISITOR_H

#include <monlang-LV2/ast/visitors/visitor.h>

#include <monlang-LV2/ast/Program.h>
#include <monlang-LV2/ast/Statement.h>
#include <monlang-LV2/ast/Expression.h>

namespace LV2
{

using Ast = std::variant<Program, Statement, Expression>;

template <typename T>
class AstVisitor;

template <>
class AstVisitor<void> {
  public:
    static constexpr bool returnsSomething = false;

    virtual void operator()(const Program&) = 0;
    virtual void operator()(const Statement&) = 0;
    virtual void operator()(const Expression&) = 0;
};

template <typename T>
class AstVisitor : public AstVisitor<void> {
  public:
    static constexpr bool returnsSomething = true;

    T res;
};

template <typename T>
auto visitAst(T visitor, const Ast& tree) {
    static_assert(std::is_base_of_v<AstVisitor<void>, T>);
    std::visit(visitor, tree);
    if constexpr (visitor.returnsSomething) {
        return visitor.res;
    }
}

} // LV2::

#endif // LV2_AST_VISITOR_H
