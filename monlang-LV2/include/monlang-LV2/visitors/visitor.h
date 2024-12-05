#ifndef LV2_VISITOR_H
#define LV2_VISITOR_H

#include <monlang-LV2/Program.h>
#include <monlang-LV2/Statement.h>
#include <monlang-LV2/Expression.h>

namespace LV2
{

using Ast_ = std::variant<
    MayFail<MayFail_<Program>>,
    MayFail<Statement_>,
    MayFail<Expression_>
>;

template <typename T>
class AstVisitor_;

template <>
class AstVisitor_<void> {
  public:
    static constexpr bool returnsSomething = false;

    virtual void operator()(const MayFail<MayFail_<Program>>&) = 0;
    virtual void operator()(const MayFail<Statement_>&) = 0;
    virtual void operator()(const MayFail<Expression_>&) = 0;
};

template <typename T>
class AstVisitor_ : public AstVisitor_<void> {
  public:
    static constexpr bool returnsSomething = true;

    T res;
};

template <typename T>
auto visitAst_(T visitor, const Ast_& tree) {
    static_assert(std::is_base_of_v<AstVisitor_<void>, T>);
    std::visit(visitor, tree);
    if constexpr (visitor.returnsSomething) {
        return visitor.res;
    }
}

} // LV2::

#endif // LV2_VISITOR_H
