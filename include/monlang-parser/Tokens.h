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
    TokenPosition() : i(0), line(0), column(0){}
    TokenPosition(size_t i, size_t line, size_t column) : i(i), line(line), column(column){}
    operator size_t() { return i; }
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
    Token& operator[](TokenId id) { return _vec.at(id); }
    Token& operator[](T entity) { return this[_map.at(entity)]; }
    size_t size() const { return _vec.size(); }

    std::vector<Token> traceback; // stack of malformed tokens

    std::vector<Token> _vec;
    EntityMap<T, TokenId> _map;
};
using LV1Tokens = Tokens<LV1::Ast_>;
using LV2Tokens = Tokens<LV2::Ast_>;

#endif // TOKENS_H
