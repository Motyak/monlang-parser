#include <monlang-LV2/expr/Numeral.h>

/* impl only */
#include <monlang-LV1/ast/Atom.h>
/* FixDecimalNumeral only */
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/Path.h>

#include <utils/assert-utils.h>

#include <functional>

#define unless(x) if(!(x))

namespace {

class mystring; // string that can be cast to false when empty..
                // ..,with .peekchar() to peek a character..
                // ..and .getchar() to extract a character

// consume string while pattern matches, return extracted characters
using consume_fn_t = std::function< mystring (mystring&) >;

/* all functions for consuming numerals */
extern consume_fn_t digit_group_sep; // '?
extern consume_fn_t int_sign; // [+-]?
extern consume_fn_t int_digits; // [0-9]*
extern consume_fn_t int_numeral; // ([+-]?[0-9]+) | \z

} // end of anonymous namespace

bool peekNumeral(const Word& word) {
    unless (std::holds_alternative<Atom*>(word)) {
        return false;
    }

    auto atom = *std::get<Atom*>(word);
    for (auto c: atom.value) {
        unless ('0' <= c && c <= '9') {
            return false;
        }
    }

    return true;
}

Numeral buildNumeral(const Word& word) {
    ASSERT (std::holds_alternative<Atom*>(word));
    auto atom = *std::get<Atom*>(word);

    auto numeral = Numeral{atom.value};
    numeral._tokenLen = atom._tokenLen;
    return numeral;
}

namespace {
class FixDecimalNumeral {
  private:
    Word& word;
    std::optional<std::string> periodic_dec_part;

  public:
    FixDecimalNumeral(Word& word) : word(word){}
    void operator()(); // doit
    void operator()(PostfixParenthesesGroup*);
    void operator()(Path*);
    void operator()(auto*);
};
} // end of anonymous namespace

void fixDecimalNumeral(Word& word) {
    auto doit = FixDecimalNumeral(word);
    doit();
}

Numeral::Numeral(const std::string& str) : str(str){}

///////////////////////////////////////////////////////////
// IMPL for file-scope declarations
///////////////////////////////////////////////////////////

namespace {
class mystring : public std::string {
  public:
    mystring() : std::string(){}
    mystring(std::string& str) : std::string(str){}
    mystring(const std::string& str) : std::string(str){}

    operator bool() const {
        return !this->empty();
    }

    char peekchar() {
        unless (!this->empty()) return -1;
        auto ret = this->at(0);
        return ret;
    }

    char getchar() {
        ASSERT (!this->empty());
        auto ret = this->at(0);
        this->erase(0, 1);
        return ret;
    }
};
} // end of anonymous namespace

void FixDecimalNumeral::operator()() {
    std::visit(*this, this->word);
}

void FixDecimalNumeral::operator()(PostfixParenthesesGroup* ppg) {
    unless (!this->periodic_dec_part) return; // we already traversed a PPG
    this->periodic_dec_part = ""; // to support the above check ^^^ // TODO: c'est pas bon !!!!!
    unless (std::holds_alternative<Path*>(ppg->leftPart)) return;

    unless (ppg->rightPart.terms.size() == 1) return;
    unless (ppg->rightPart.terms[0].words.size() == 1) return;
    unless (std::holds_alternative<Atom*>(ppg->rightPart.terms[0].words[0])) return;
    mystring ppg_right_part = std::get<Atom*>(ppg->rightPart.terms[0].words[0])->value;
    ASSERT (ppg_right_part.size() > 0);
    auto periodic_dec_part = int_digits(ppg_right_part);
    unless (!ppg_right_part) return;
    unless (periodic_dec_part) return;

    this->periodic_dec_part = periodic_dec_part;
    std::visit(*this, ppg->leftPart);
}

void FixDecimalNumeral::operator()(Path* path) {
    mystring path_right_part = path->rightPart.value;
    ASSERT (path_right_part.size() > 0);
    auto fixed_dec_part = int_digits(path_right_part);
    unless (!path_right_part) return; // we allow empty fixed dec part (digit group separators only)..
                                      // ..as in 0.'(3)

    unless (std::holds_alternative<Atom*>(path->leftPart)) return;
    mystring path_left_part = std::get<Atom*>(path->leftPart)->value;
    ASSERT (path_left_part.size() > 0);
    auto int_part = int_numeral(path_left_part);
    unless (!path_left_part) return;
    unless (int_part) return;

    unless (fixed_dec_part || this->periodic_dec_part) return;
    auto dec_numeral = int_part + "." + fixed_dec_part;
    if (this->periodic_dec_part) {
        dec_numeral += "(" + this->periodic_dec_part.value() + ")";
    }
    this->word = new Atom{dec_numeral};
}

void FixDecimalNumeral::operator()(auto*) {
    ;
}

namespace {

consume_fn_t digit_group_sep = [](mystring& str) -> mystring {
    auto res = mystring();
    unless (str) return res;

    auto c = str.peekchar();
    if (c == '\'') {
        res += str.getchar();
    }

    return res;
};

consume_fn_t int_sign = [](mystring& str) -> mystring {
    auto res = mystring();
    unless (str) return res;

    auto c = str.peekchar();
    if (c == '+' || c == '-') {
        res += str.getchar();
    }

    return res;
};

consume_fn_t int_digits = [](mystring& str) -> mystring {
    auto res = mystring();

    while (str) {
        if (auto sep = digit_group_sep(str)) {
            res += sep;
            continue;
        }
        auto c = str.peekchar();
        unless ('0' <= c && c <= '9') break;
        res += str.getchar();
    }

    return res;
};

consume_fn_t int_numeral = [](mystring& str) -> mystring {
    auto res = mystring();

    res += int_sign(str);

    auto digits = int_digits(str);
    unless (digits) return mystring();
    res += digits;

    return res;
};

} // end of anonymous namespace
