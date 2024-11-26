#ifndef LV2_VISITOR_H
#define LV2_VISITOR_H

#include <monlang-LV2/Program.h>
#include <monlang-LV2/Statement.h>
#include <monlang-LV2/Expression.h>

namespace LV2
{

using Ast = std::variant<LV2::Program, Statement, Expression>;

template <typename T>
class AstVisitor;

template <>
class AstVisitor<void> {
  public:
    static constexpr bool returnsSomething = false;

    virtual void operator()(const LV2::Program&) = 0;
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

#endif // LV2_VISITOR_H
