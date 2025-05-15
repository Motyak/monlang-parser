/*
    standalone header, no .cpp
*/

#ifndef TOKENS_H
#define TOKENS_H

#include <string>
#include <cstdint>
#include <vector>

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

class Tokens {
  public:
    std::vector<Token> _vec; // the list of tokens
    std::vector<Token> traceback; // stack of malformed tokens
    
    Token& operator[](TokenId id){return _vec.at(id);}
    size_t size() const{return _vec.size();}
};

#endif // TOKENS_H
