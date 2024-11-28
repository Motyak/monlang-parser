#include <monlang-LV2/precedence.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ParenthesesGroup.h>

#include <utils/assert-utils.h>
#include <utils/vec-utils.h>
#include <utils/mem-utils.h>
#include <utils/str-utils.h>

#include <cstdint>

#define UINT uint32_t
#define INT int32_t

namespace
{

// TERM CTOR //////////////////////////////////////////////

Word Atom_(std::string val) {
    return move_to_heap(Atom{val});
}

// explicit cast
Atom Atom_(Word word) {
    ASSERT (std::holds_alternative<Atom*>(word));
    return *std::get<Atom*>(word);
}

// TERM SELECTORS /////////////////////////////////////////

struct Operator {
    std::string val;
    UINT pos;
};

Operator optr(Term term, INT nth) {
    ASSERT (nth != 0);
    ASSERT (abs(nth) * 2 <= term.words.size());

    UINT pos;
    if (nth > 0) {
        pos = UINT(nth) * 2 - 1;
    }
    else /* if nth < 0 */ {
        pos = term.words.size() - 2 * abs(nth);
    }

    return Operator{Atom_(term.words[pos]).value, pos};
}

Word left_opnd(Term term, Operator op) {
    ASSERT (term.words.size() >= op.pos);
    return term.words[op.pos - 1];
}

Word right_opnd(Term term, Operator op) {
    ASSERT (term.words.size() >= op.pos + 2);
    return term.words[op.pos + 1];
}

std::optional<Operator> prev_optr(Term term, Operator op) {
    if (op.pos < 2) {
        return {};
    }
    UINT pos = op.pos - 2;
    return Operator{Atom_(term.words[pos]).value, pos};
}

std::optional<Operator> next_optr(Term term, Operator op) {
    if (op.pos >= term.words.size() - 2) {
        return {};
    }
    UINT pos = op.pos + 2;
    return Operator{Atom_(term.words.at(pos)).value, pos};
}

// TERM MUTATOR ///////////////////////////////////////////

// extract operation from Term, as a sub-Term
Term Term_(Term term, Operator op) {
    std::vector<Word> words;

    words.push_back(left_opnd(term, op));
    words.push_back(Atom_(op.val));
    words.push_back(right_opnd(term, op));

    return Term{words};
}

// extract operation from Term, as a grouped sub-Term
ParenthesesGroup ParenthesesGroup_(Term term, Operator op) {
    return ParenthesesGroup{{Term_(term, op)}};
}

// explicit cast
Word Word_(ParenthesesGroup pg) {
    return move_to_heap(pg);
}

void parenthesize_optn(Term* term, Operator op) {
    auto operation = ParenthesesGroup_(*term, op);

    // copy all words BEFORE operation
    auto left_words = std::vector<Word>(term->words.begin(), term->words.begin() + op.pos - 1);

    // copy all words AFTER operation
    auto right_words = std::vector<Word>(term->words.begin() + op.pos + 2, term->words.end());

    // concat both ends with the new parenthesized operation
    std::vector<Word> new_words = vec_concat({
        left_words,
        {Word_(operation)},
        right_words,
    });

    // update &term with new value
    term->words = new_words;
}

} // anonymous namespace

#ifdef TEST_OPTR
// g++ -D TEST_OPTR -o main.elf src/Precedence.cpp lib/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");

    {
        auto op = optr(input, 1);
        std::cout << "#1 <=> [" << op.pos << "]: " << op.val << std::endl;
    }

    {
        auto op = optr(input, -6);
        std::cout << "#-6 <=> [" << op.pos << "]: " << op.val << std::endl;
    }

    {
        auto op = optr(input, -1);
        std::cout << "#-1 <=> [" << op.pos << "]: " << op.val << std::endl;
    }

    {
        auto op = optr(input, 6);
        std::cout << "#6 <=> [" << op.pos << "]: " << op.val << std::endl;
    }
}
#endif // TEST_OPTR

#ifdef TEST_OPND
// g++ -D TEST_OPND -o main.elf src/Precedence.cpp lib/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");

    {
        auto op = optr(input, 1);
        auto leftopnd = std::get<Atom*>(left_opnd(input, op))->value;
        auto rightopnd = std::get<Atom*>(right_opnd(input, op))->value;
        std::cout << "#1: " << leftopnd << " " << op.val << " " << rightopnd << std::endl;
    }

    {
        auto op = optr(input, -1);
        auto leftopnd = std::get<Atom*>(left_opnd(input, op))->value;
        auto rightopnd = std::get<Atom*>(right_opnd(input, op))->value;
        std::cout << "#-1: " << leftopnd << " " << op.val << " " << rightopnd << std::endl;
    }
}
#endif // TEST_OPND

#ifdef TEST_PREV_NEXT_OPTR
// g++ -D TEST_PREV_NEXT_OPTR -o main.elf src/Precedence.cpp lib/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");

    {
        auto op = optr(input, 2);
        std::cout << "#2: " << op.val << std::endl;
        std::cout << "<prev>: " << prev_optr(input, op).value().val << std::endl;
        std::cout << "---" << std::endl;
    }

    {
        auto op = optr(input, 2);
        std::cout << "#2: " << op.val << std::endl;
        std::cout << "<next>: " << next_optr(input, op).value().val << std::endl;
        std::cout << "---" << std::endl;
    }

    // // ERR
    // {
    //     auto op = optr(input, 1);
    //     std::cout << "#1: " << op.val << std::endl;
    //     std::cout << "<prev>: " << prev_optr(input, op).value().val << std::endl;
    // }

    // // ERR
    // {
    //     auto op = optr(input, -1);
    //     std::cout << "#-1: " << op.val << std::endl;
    //     std::cout << "<next>: " << next_optr(input, op).value().val << std::endl;
    // }
}
#endif // TEST_PREV_NEXT_OPTR

#ifdef TEST_PARENTHESIZE
// g++ -D TEST_PARENTHESIZE -o main.elf src/Precedence.cpp lib/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include
int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");

    {
        auto op = optr(input, 1);
        ASSERT (std::holds_alternative<Atom*>(input.words[0]));
        parenthesize_optn(&input, op);
        ASSERT (std::holds_alternative<ParenthesesGroup*>(input.words[0]));
    }

    {
        auto op = optr(input, 1);
        auto first_word_inside_pg = std::get<ParenthesesGroup*>(input.words[0])->terms[0].words[0];
        ASSERT (std::holds_alternative<Atom*>(first_word_inside_pg));
        parenthesize_optn(&input, op);
        first_word_inside_pg = std::get<ParenthesesGroup*>(input.words[0])->terms[0].words[0];
        ASSERT (std::holds_alternative<ParenthesesGroup*>(first_word_inside_pg));
    }

}
#endif // TEST_PARENTHESIZE


///////////////////////////////////////////////////////////
// NON-STATIC PART
///////////////////////////////////////////////////////////


#define unless(x) if(!(x))

const std::vector<std::pair<Operators,Associativity>>
PRECEDENCE_TABLE = {
    /* ordered from highest precedence to lowest */
    {{"^"}, RIGHT_ASSOCIATIVE},
    {{"*", "/", "%"}, LEFT_ASSOCIATIVE},
    {{"+", "-"}, LEFT_ASSOCIATIVE},
};

namespace {
    enum Direction {
        ITERATE_LEFT_TO_RIGHT,
        ITERATE_RIGHT_TO_LEFT,
    };
}

// returns false if no op found, true otherwise
static bool parenthesizeFirstEncounteredOp(Term*, std::vector<std::string> opvals, Direction);

void fixPrecedence(Term& term) {
    ASSERT (term.words.size() % 2 == 1);

    unless (term.words.size() > 3) {
        return; // nothing to do
    }

    for (auto [optrs, assoc]: PRECEDENCE_TABLE) {
        auto direction = assoc == LEFT_ASSOCIATIVE? ITERATE_LEFT_TO_RIGHT : ITERATE_RIGHT_TO_LEFT;
        while (term.words.size() > 3 && parenthesizeFirstEncounteredOp(&term, optrs, direction))
            ; // until 3 words size term or no more operator found
    }
}

static bool parenthesizeFirstEncounteredOp(Term* term, std::vector<std::string> opvals, Direction direction) {
    std::optional<Operator> op;
    std::optional<Operator> (*next)(Term, Operator);

    if (direction == ITERATE_LEFT_TO_RIGHT) {
        op = optr(*term, 1);
        next = next_optr;
    }

    if (direction == ITERATE_RIGHT_TO_LEFT) {
        op = optr(*term, -1);
        next = prev_optr;
    }

    while (op) {
        if (vec_contains(opvals, op->val)) {
            parenthesize_optn(/*OUT*/term, *op);
            return true;
        }
        op = next(*term, *op);
    }

    return false; // no op found
}