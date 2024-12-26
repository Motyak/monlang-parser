#include <monlang-LV2/precedence.h>

#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/ParenthesesGroup.h>

/* require knowing all words for token_len() */
#include <monlang-LV1/ast/SquareBracketsGroup.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/Association.h>

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

// explicit cast
Atom asAtom(Word word) {
    ASSERT (std::holds_alternative<Atom*>(word));
    return *std::get<Atom*>(word);
}

Word asWord(Atom atom) {
    return move_to_heap(atom);
}

// TERM SELECTORS /////////////////////////////////////////

struct Operator {
    // std::string val;
    Atom val;
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

    return Operator{asAtom(term.words[pos]), pos};
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
    return Operator{asAtom(term.words[pos]), pos};
}

std::optional<Operator> next_optr(Term term, Operator op) {
    if (op.pos >= term.words.size() - 2) {
        return {};
    }
    UINT pos = op.pos + 2;
    return Operator{asAtom(term.words[pos]), pos};
}

// TERM MUTATOR ///////////////////////////////////////////

// extract operation from Term, as a sub-Term
Term extract_optn(Term term, Operator op) {
    std::vector<Word> words;

    words.push_back(left_opnd(term, op));
    words.push_back(asWord(op.val));
    words.push_back(right_opnd(term, op));

    auto res_term = Term{words};
    res_term._tokenLen = token_len(words[0])
            + op.val._tokenLen
            + token_len(words[2])
            + 2 * sequenceLen(Term::CONTINUATOR_SEQUENCE);
    return res_term;
}

ParenthesesGroup ParenthesesGroup_(Term term) {
    auto pg = ParenthesesGroup{{term}};
    pg._tokenLen = term._tokenLen
            + sequenceLen(ParenthesesGroup::INITIATOR_SEQUENCE)
            + sequenceLen(ParenthesesGroup::TERMINATOR_SEQUENCE);
    return pg;
}

// explicit cast
Word asWord(ParenthesesGroup pg) {
    return move_to_heap(pg);
}

void parenthesize_optn(Term* term, Operator op) {
    auto operation = ParenthesesGroup_(extract_optn(*term, op));

    // copy all words BEFORE operation
    auto left_words = std::vector<Word>(term->words.begin(), term->words.begin() + op.pos - 1);

    // copy all words AFTER operation
    auto right_words = std::vector<Word>(term->words.begin() + op.pos + 2, term->words.end());

    // concat both ends with the new parenthesized operation
    std::vector<Word> new_words = vec_concat({
        left_words,
        {asWord(operation)},
        right_words,
    });

    // update &term with new value
    term->words = new_words;
}

} // anonymous namespace

#ifdef TEST_OPTR
// g++ -D TEST_OPTR -o main.elf src/precedence.cpp lib/monlang-LV1/dist/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include

Term Term_(std::string operation) {
    auto str_vec = split(operation, " ");
    std::vector<Word> words;
    for (auto token: str_vec) {
        words.push_back(Atom_(token));
    }
    return Term{words};
}

int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");

    {
        auto op = optr(input, 1);
        std::cout << "#1 <=> [" << op.pos << "]: " << op.val.value << std::endl;
    }

    {
        auto op = optr(input, -6);
        std::cout << "#-6 <=> [" << op.pos << "]: " << op.val.value << std::endl;
    }

    {
        auto op = optr(input, -1);
        std::cout << "#-1 <=> [" << op.pos << "]: " << op.val.value << std::endl;
    }

    {
        auto op = optr(input, 6);
        std::cout << "#6 <=> [" << op.pos << "]: " << op.val.value << std::endl;
    }
}
#endif // TEST_OPTR

#ifdef TEST_OPND
// g++ -D TEST_OPND -o main.elf src/precedence.cpp lib/monlang-LV1/dist/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include

Term Term_(std::string operation) {
    auto str_vec = split(operation, " ");
    std::vector<Word> words;
    for (auto token: str_vec) {
        words.push_back(Atom_(token));
    }
    return Term{words};
}

int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");

    {
        auto op = optr(input, 1);
        auto leftopnd = std::get<Atom*>(left_opnd(input, op))->value;
        auto rightopnd = std::get<Atom*>(right_opnd(input, op))->value;
        std::cout << "#1: " << leftopnd << " " << op.val.value << " " << rightopnd << std::endl;
    }

    {
        auto op = optr(input, -1);
        auto leftopnd = std::get<Atom*>(left_opnd(input, op))->value;
        auto rightopnd = std::get<Atom*>(right_opnd(input, op))->value;
        std::cout << "#-1: " << leftopnd << " " << op.val.value << " " << rightopnd << std::endl;
    }
}
#endif // TEST_OPND

#ifdef TEST_PREV_NEXT_OPTR
// g++ -D TEST_PREV_NEXT_OPTR -o main.elf src/precedence.cpp lib/monlang-LV1/dist/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include

Term Term_(std::string operation) {
    auto str_vec = split(operation, " ");
    std::vector<Word> words;
    for (auto token: str_vec) {
        words.push_back(Atom_(token));
    }
    return Term{words};
}

int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");

    {
        auto op = optr(input, 2);
        std::cout << "#2: " << op.val.value << std::endl;
        std::cout << "<prev>: " << prev_optr(input, op).value().val.value << std::endl;
        std::cout << "<next>: " << next_optr(input, op).value().val.value << std::endl;
        std::cout << "---" << std::endl;
    }

    // // ERR
    // {
    //     auto op = optr(input, 1);
    //     std::cout << "#1: " << op.val.value << std::endl;
    //     std::cout << "<prev>: " << prev_optr(input, op).value().val.value << std::endl;
    // }

    // // ERR
    // {
    //     auto op = optr(input, -1);
    //     std::cout << "#-1: " << op.val.value << std::endl;
    //     std::cout << "<next>: " << next_optr(input, op).value().val.value << std::endl;
    // }
}
#endif // TEST_PREV_NEXT_OPTR

#ifdef TEST_PARENTHESIZE
// g++ -D TEST_PARENTHESIZE -o main.elf src/precedence.cpp lib/monlang-LV1/dist/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include

#include <monlang-LV1/ast/visitors/Unparse.h>

Term Term_(std::string operation) {
    auto str_vec = split(operation, " ");
    std::vector<Word> words;
    for (auto token: str_vec) {
        words.push_back(Atom_(token));
    }
    return Term{words};
}

std::ostream& operator<<(std::ostream& os, Term term) {
    /* first wrap term into parentheses group, to convert it into an AST (Word) */
    auto pg = ParenthesesGroup{{term}};
    auto word = (ProgramWord)move_to_heap(pg);

    std::ostringstream oss;
    visitAst(Unparse(oss), word);

    /* then remove extra parentheses */
    std::string tmp = oss.str();
    auto res = std::string(tmp.begin() + 1, tmp.end() - 1);
    return os << res;
}

int main()
{
    Term input = Term_("1 + 2 * 2 ^ 3 ^ 2 + 91 ^ 1");
    std::cerr << "Term: " << input << std::endl;

    {
        auto op = optr(input, 1);
        ASSERT (std::holds_alternative<Atom*>(input.words[0]));
        parenthesize_optn(&input, op);
        ASSERT (std::holds_alternative<ParenthesesGroup*>(input.words[0]));
    }

    std::cerr << "Term: " << input << std::endl;

    {
        auto op = optr(input, 1);
        auto first_word_inside_pg = std::get<ParenthesesGroup*>(input.words[0])->terms[0].words[0];
        ASSERT (std::holds_alternative<Atom*>(first_word_inside_pg));
        parenthesize_optn(&input, op);
        first_word_inside_pg = std::get<ParenthesesGroup*>(input.words[0])->terms[0].words[0];
        ASSERT (std::holds_alternative<ParenthesesGroup*>(first_word_inside_pg));
    }

    std::cerr << "Term: " << input << std::endl;
}
#endif // TEST_PARENTHESIZE

#ifdef TEST_PRECEDENCE
// g++ -D TEST_PRECEDENCE -o main.elf src/precedence.cpp lib/monlang-LV1/dist/monlang-LV1.a --std=c++23 -Wall -Wextra -Og -ggdb3 -I include -I lib/monlang-LV1/dist

#include <monlang-LV1/Term.h>
#include <monlang-LV1/ast/visitors/Unparse.h>

std::ostream& operator<<(std::ostream& os, Term term) {
    std::ostringstream oss;
    visitAst(Unparse(oss), term);
    return os << oss.str();
}

int main()
{
    auto iss = std::istringstream("1 + 2 * (1 + 2 * 3)");
    Term input = (Term)consumeTerm(iss);
    std::cerr << "Term: " << input << std::endl;

    fixPrecedence(input);

    std::cerr << "Term: " << input << std::endl;
}
#endif // TEST_PRECEDENCE

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
    {{"<", "<=", ">", ">="}, LEFT_ASSOCIATIVE},
    {{"==", "<>"}, LEFT_ASSOCIATIVE},
};

namespace {
    enum Direction {
        ITERATE_LEFT_TO_RIGHT,
        ITERATE_RIGHT_TO_LEFT,
    };
}

// returns the position of the freshly parenthesized operation operator..
// ..or UINT(-1) if no operation were found
static UINT parenthesizeFirstEncounteredOp(Term*, std::vector<std::string> opvals, Direction);

void fixPrecedence(Term& term) {
    ASSERT (term.words.size() % 2 == 1);

    unless (term.words.size() > 3) {
        return; // nothing to do
    }

    for (auto [optrs, assoc]: PRECEDENCE_TABLE) {
        auto direction = assoc == LEFT_ASSOCIATIVE? ITERATE_LEFT_TO_RIGHT : ITERATE_RIGHT_TO_LEFT;
        while (term.words.size() > 3
                && UINT(-1) != parenthesizeFirstEncounteredOp(&term, optrs, direction)) {

            ; // until 3 words size term or no more operation found
        }
    }
}

static UINT parenthesizeFirstEncounteredOp(Term* term, std::vector<std::string> opvals, Direction direction) {
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
        if (vec_contains(opvals, op->val.value)) {
            parenthesize_optn(/*OUT*/term, *op);
            return op->pos;
        }
        op = next(*term, *op);
    }

    return -1; // no op found
}

// overloaded function to accept a tracing object by reference
void fixPrecedence(Term& term, std::stack<Alteration>& alterations) {
    ASSERT (term.words.size() % 2 == 1);

    unless (term.words.size() > 3) {
        alterations.push(Alteration::NONE);
        return; // nothing to do
    }

    auto first_it = true;
    auto prev_optr_pos = UINT();
    auto cur_optr_pos = UINT();
    for (auto [optrs, assoc]: PRECEDENCE_TABLE) {
        auto direction = assoc == LEFT_ASSOCIATIVE? ITERATE_LEFT_TO_RIGHT : ITERATE_RIGHT_TO_LEFT;
        while (term.words.size() > 3
                && UINT(-1) != (cur_optr_pos = parenthesizeFirstEncounteredOp(&term, optrs, direction))) {

            ; // until 3 words size term or no more operation found

            if (first_it) {
                first_it = false;
                alterations.push(Alteration::DONE); // last alteration first
            }

            if (cur_optr_pos < prev_optr_pos) {
                alterations.push(Alteration::LEFT_OPND);
            }
            else if (cur_optr_pos > prev_optr_pos) {
                alterations.push(Alteration::RIGHT_OPND);
            }
            else {
                SHOULD_NOT_HAPPEN();
            }

            // save cur_optr_pos for next iteration
            prev_optr_pos = cur_optr_pos;
        }
    }
}
