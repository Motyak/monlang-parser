/*
    standalone header, no .cpp
*/

#ifndef TOKENS_H
#define TOKENS_H

#include <monlang-parser/EntityMap.h>

#include <monlang-LV1/visitors/visitor.h>
#include <monlang-LV2/visitors/visitor.h>

#include <string>
#include <cstdint>

using TokenId = size_t;

struct TokenPosition {
    size_t i; // stringstream absolute index
    size_t line;
    size_t column;
    TokenPosition() : i(0), line(0), column(0){} // TODO: create .cpp
    TokenPosition(size_t i, size_t line, size_t column) : i(i), line(line), column(column){} // TODO: create .cpp
    operator size_t(){return i;} // TODO: create .cpp
};

struct Token {
    std::string name;
    TokenPosition start;
    TokenPosition end;

    bool is_malformed = false;
    TokenPosition err_start = {}; // only malformed token, not necessarily equal to start
    std::string err_desc = "";
};

template <typename T>
class Tokens {
  public:
    Token& operator[](TokenId); // TODO: returns a copy?
    Token& operator[](T); // TODO: returns a copy?
    // TODO: add a .size() that returns vec size ?

    std::vector<Token> traceback; // stack of malformed tokens

    std::vector<Token> _vec;
    EntityMap<T, TokenId> _map;
};
using LV1Tokens = Tokens<LV1::Ast_>;
using LV2Tokens = Tokens<LV2::Ast_>;

#endif // TOKENS_H
