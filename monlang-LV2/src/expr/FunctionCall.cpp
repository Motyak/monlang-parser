#include <monlang-LV2/expr/FunctionCall.h>

/* impl only */
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
/* required by FixPassByRef */
#include <monlang-LV2/expr/FieldAccess.h>
#include <monlang-LV2/expr/Subscript.h>
/* require knowing all words for token_len() */
#include <monlang-LV2/token_len.h>

#include <utils/assert-utils.h>
#include <utils/variant-utils.h>

#define unless(x) if(!(x))

bool peekFunctionCall(const Word& word) {
    return std::holds_alternative<PostfixParenthesesGroup*>(word);
}

static MayFail<MayFail_<FunctionCall>::Argument>
buildArgument(const MayFail<Expression_>& expr);

MayFail<MayFail_<FunctionCall>> buildFunctionCall(const Word& word) {
    ASSERT (std::holds_alternative<PostfixParenthesesGroup*>(word));
    auto ppg = *std::get<PostfixParenthesesGroup*>(word);
    auto function = buildExpression((Term)variant_cast(ppg.leftPart));
    if (function.has_error()) {
        return Malformed(MayFail_<FunctionCall>{function, {}}, ERR(621));
    }

    std::vector<MayFail<MayFail_<FunctionCall>::Argument>> arguments;
    for (auto term: ppg.rightPart.terms) {
        auto expr = buildExpression(term);
        auto argument = buildArgument(expr);
        arguments.push_back(argument);
        if (argument.has_error()) {
            return Malformed(MayFail_<FunctionCall>{function, arguments}, ERR(622));
        }
    }

    auto functionCall = MayFail_<FunctionCall>{function, arguments};
    functionCall._tokenLen = ppg._tokenLen;
    return functionCall;
}

namespace {
/*
    Responsible for correcting potential pass by ref..
    ..(cutting leading ampersand from leftmost Symbol..
    ..and removing one from each token_len in the hierarchy)..
    ..AND setting the appropriate error to Argument..
    ..(malformed expr OR non-lvalue passed by ref)..
    ..if any.

    As well as setting the _tokenLen field.
*/
class FixPassByRef {
  private:
    MayFail<MayFail_<FunctionCall>::Argument>& arg;

  public:
    FixPassByRef(MayFail<MayFail_<FunctionCall>::Argument>& arg) : arg(arg) {}
    void operator()(); // doit

    void operator()(MayFail_<FunctionCall>*);
    void operator()(MayFail_<FieldAccess>*);
    void operator()(MayFail_<Subscript>*);
    void operator()(Symbol*);
    void operator()(auto*);
};
} // end of anonymous namespace

static MayFail<MayFail_<FunctionCall>::Argument>
buildArgument(const MayFail<Expression_>& expr) {
    auto passByRef = false; // initialize
    auto arg = MayFail<MayFail_<FunctionCall>::Argument>{{expr, passByRef}};
    auto fixPassByRef = FixPassByRef(/*OUT*/arg);
    fixPassByRef();
    return arg;
}

namespace {
void FixPassByRef::operator()() {
    /* prevent double call */
    ASSERT (!this->arg.has_error());
    ASSERT (!this->arg.val.passByRef);

    std::visit(*this, this->arg.val.expr.val);

    if (this->arg.val.expr.has_error()) {
        this->arg.err = ERR(731);
        return;
    }

    this->arg.val._tokenLen = this->arg.val.passByRef
            + token_len(this->arg.val.expr.val);

    unless (this->arg.val.passByRef) return;
    if (!is_lvalue(this->arg.val.expr.value())) {
        // TODO: maybe need to pass some info in err._info ?
        this->arg.err = ERR(732);
    }
}

void FixPassByRef::operator()(MayFail_<FunctionCall>* functionCall) {
    std::visit(*this, functionCall->function.val);
    if (this->arg.val.passByRef) {
        functionCall->_tokenLen -= 1; // &
    }
}

void FixPassByRef::operator()(MayFail_<FieldAccess>* fieldAccess) {
    std::visit(*this, fieldAccess->object.val);
    if (this->arg.val.passByRef) {
        fieldAccess->_tokenLen -= 1; // &
    }
}

void FixPassByRef::operator()(MayFail_<Subscript>* subscript) {
    std::visit(*this, subscript->array.val);
    if (this->arg.val.passByRef) {
        subscript->_tokenLen -= 1; // &
    }
}

/* terminals */

void FixPassByRef::operator()(auto*) {
    ;
}

void FixPassByRef::operator()(Symbol* symbol) {
    unless (symbol->value.size() >= 2) return;
    unless (symbol->value.at(0) == '&') return;
    unless (symbol->value.at(1) != '&') return;

    symbol->value = symbol->value.substr(1); // remove leading '&'
    symbol->_tokenLen -= 1; // &
    this->arg.val.passByRef = true;
}
} // end of anonymous namespace

///////////////////////////////////////////////////////////

FunctionCall::Argument::Argument(const Expression& expr, bool passByRef)
        : expr(expr), passByRef(passByRef){}

FunctionCall::FunctionCall(const Expression& function, const std::vector<Argument>& arguments)
        : function(function), arguments(arguments){}

MayFail_<FunctionCall>::Argument::Argument(const MayFail<Expression_>& expr, bool passByRef)
        : expr(expr), passByRef(passByRef){}

MayFail_<FunctionCall>::MayFail_(const MayFail<Expression_>& function, const std::vector<MayFail<Argument>>& arguments)
        : function(function), arguments(arguments){}

MayFail_<FunctionCall>::MayFail_(FunctionCall functionCall) {
    auto function = wrap_expr(functionCall.function);
    auto arguments = std::vector<MayFail<Argument>>();
    for (auto arg: functionCall.arguments) {
        arguments.push_back(MayFail<Argument>({wrap_expr(arg.expr), arg.passByRef}));
    }

    this->function = function;
    this->arguments = arguments;
    this->_tokenLen = functionCall._tokenLen;
}

MayFail_<FunctionCall>::operator FunctionCall() const {
    auto function = unwrap_expr(this->function.value());
    auto arguments = std::vector<FunctionCall::Argument>();
    for (auto arg: this->arguments) {
        auto arg_ = arg.value();
        arguments.push_back({unwrap_expr(arg_.expr.value()), arg_.passByRef});
    }
    auto functionCall = FunctionCall{function, arguments};
    functionCall._tokenLen = this->_tokenLen;
    return functionCall;
}
