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

/*
    string that can be cast to false when empty..
    ..,with .peekchar() to peek a character..
    ..and .getchar() to extract a character
*/
class mystring : public std::string {
  public:
    mystring() : std::string(){}
    mystring(std::string& str) : std::string(str){}
    mystring(const std::string& str) : std::string(str){}
    operator bool() const;
    char peekchar() const;
    char getchar();
};

// consume string while pattern matches, return extracted characters
using consume_fn_t = const std::function< mystring (mystring&) >;

/* all functions for consuming numerals */

extern consume_fn_t digit_group_sep; // '
extern consume_fn_t fraction_bar; // /
extern consume_fn_t exponent_caret; // ^
extern consume_fn_t decimal_sep; // .

extern consume_fn_t int_sign; // + and -
extern consume_fn_t int_digits; // 0..9 and digit group separators (but at least one digit)
extern consume_fn_t int_numeral; // <int_sign> <int_digits>
extern consume_fn_t opt_int_digits; // 0..9 and digit group separators

/* TODO: unused */
extern consume_fn_t dec_numeral; // <int_numeral> <DECIMAL_SEPARATOR> (<opt_int_digits> | (<PERIODIC_PART_START> <int_digits> <PERIODIC_PART_END>))
extern consume_fn_t frac_numeral; // <int_numeral> <FRACTION_BAR> <int_digits>
extern consume_fn_t pow_numeral; // <int_numeral> <EXPONENT_CARET> <int_digits>

extern consume_fn_t hex_prefix; // 0x
extern consume_fn_t hex_digits; // 0..9, A..F
/* TODO: unused */ extern consume_fn_t hex_numeral; // <hex_prefix> <hex_digits>

extern consume_fn_t oct_prefix; // 0o
extern consume_fn_t oct_digits; // 0..7
/* TODO: unused */ extern consume_fn_t oct_numeral; // <oct_prefix> <oct_digits>

extern consume_fn_t bin_prefix; // 0b
extern consume_fn_t bin_digits; // 0 1
/* TODO: unused */ extern consume_fn_t bin_numeral; // <bin_prefix> <bin_digits>

} // end of anonymous namespace

bool peekNumeral(const Word& word) {
    unless (std::holds_alternative<Atom*>(word)) {
        return false;
    }
    mystring atom_value = std::get<Atom*>(word)->value;
    ASSERT (atom_value.size() > 0);

    if (hex_prefix(atom_value)) {
        return hex_digits(atom_value) && !atom_value;
    }
    if (oct_prefix(atom_value)) {
        return oct_digits(atom_value) && !atom_value;
    }
    if (bin_prefix(atom_value)) {
        return bin_digits(atom_value) && !atom_value;
    }

    unless (int_numeral(atom_value)) return false;

    if (!atom_value) {
        return true;
    }

    if (fraction_bar(atom_value)) {
        return int_digits(atom_value) && !atom_value;
    }
    if (exponent_caret(atom_value)) {
        return int_digits(atom_value) && !atom_value;
    }
    if (decimal_sep(atom_value)) {
        return true; // guaranteed because the only way to get a dot in an Atom..
                     // ..is through fixDecimalNumeral(), which already performs the checks
    }

    return false;
}

Numeral buildNumeral(const Word& word) {
    // TODO: update struct Numeral to add .fmt (with text representation)..
    // .., along with "numeric" representations (fraction as two str, or int as one str)

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

void FixDecimalNumeral::operator()() {
    std::visit(*this, this->word);
}

void FixDecimalNumeral::operator()(PostfixParenthesesGroup* ppg) {
    unless (!this->periodic_dec_part.has_value()) return; // we already traversed a PPG
    this->periodic_dec_part = ""; // to support the above check ^^^
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
    auto fixed_dec_part = opt_int_digits(path_right_part); /*
        we allow empty fixed dec part (digit group separators only) as in 0.'(3)..
        ..but only when there is a periodic dec part
    */
   unless (!path_right_part) return;
   unless (fixed_dec_part) return;

    unless (std::holds_alternative<Atom*>(path->leftPart)) return;
    mystring path_left_part = std::get<Atom*>(path->leftPart)->value;
    ASSERT (path_left_part.size() > 0);
    auto int_part = int_numeral(path_left_part);
    unless (!path_left_part) return;
    unless (int_part) return;

    auto fixed_dec_part_ = fixed_dec_part;
    unless (int_digits(fixed_dec_part_) || this->periodic_dec_part) return;
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

consume_fn_t constant(const std::string& const_) {
    return [const_](mystring& str){
        auto res = mystring();

        if (str.starts_with(const_)) {
            res += str.substr(0, const_.size());
            str.erase(0, const_.size());
        }

        return res;    
    };
}

consume_fn_t digit_group_sep = constant("'");

consume_fn_t int_sign = [](mystring& str) -> mystring {
    auto res = mystring();
    unless (str) return res;

    auto c = str.peekchar();
    if (c == '+' || c == '-') {
        res += str.getchar();
    }

    return res;
};

consume_fn_t opt_int_digits = [](mystring& str) -> mystring {
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

consume_fn_t int_digits = [](mystring& str) -> mystring {
    auto res = mystring();

    bool at_least_one_digit = false;
    while (str) {
        if (auto sep = digit_group_sep(str)) {
            res += sep;
            continue;
        }
        auto c = str.peekchar();
        unless ('0' <= c && c <= '9') break;
        at_least_one_digit = true;
        res += str.getchar();
    }
    unless (at_least_one_digit) return mystring();

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

consume_fn_t hex_prefix = constant("0x");
consume_fn_t hex_digits = [](mystring& str) -> mystring {
    auto res = mystring();

    bool at_least_one_digit = false;
    while (str) {
        if (auto sep = digit_group_sep(str)) {
            res += sep;
            continue;
        }
        auto c = str.peekchar();
        unless (('0' <= c && c <= '9') || ('A' <= c && c <= 'F')) break;
        at_least_one_digit = true;
        res += str.getchar();
    }
    unless (at_least_one_digit) return mystring();

    return res;
};

consume_fn_t oct_prefix = constant("0o");
consume_fn_t oct_digits = [](mystring& str) -> mystring {
    auto res = mystring();

    bool at_least_one_digit = false;
    while (str) {
        if (auto sep = digit_group_sep(str)) {
            res += sep;
            continue;
        }
        auto c = str.peekchar();
        unless ('0' <= c && c <= '7') break;
        at_least_one_digit = true;
        res += str.getchar();
    }
    unless (at_least_one_digit) return mystring();

    return res;
};

consume_fn_t bin_prefix = constant("0b");
consume_fn_t bin_digits = [](mystring& str) -> mystring {
    auto res = mystring();

    bool at_least_one_digit = false;
    while (str) {
        if (auto sep = digit_group_sep(str)) {
            res += sep;
            continue;
        }
        auto c = str.peekchar();
        unless (c == '0' || c == '1') break;
        at_least_one_digit = true;
        res += str.getchar();
    }
    unless (at_least_one_digit) return mystring();

    return res;
};

consume_fn_t fraction_bar = constant("/");
consume_fn_t exponent_caret = constant("^");
consume_fn_t decimal_sep = constant(".");

mystring::operator bool() const {
    return !this->empty();
}

char mystring::peekchar() const {
    unless (!this->empty()) return -1;
    auto ret = this->at(0);
    return ret;
}

char mystring::getchar() {
    ASSERT (!this->empty());
    auto ret = this->at(0);
    this->erase(0, 1);
    return ret;
}

} // end of anonymous namespace
