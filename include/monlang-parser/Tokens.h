#ifndef TOKENS_H
#define TOKENS_H

#include <monlang-parser/EntityMap.h>

#include <monlang-LV1/visitors/visitor.h>
#include <monlang-LV2/visitors/visitor.h>

#include <string>
#include <cstdint>

using TokenId = size_t;

struct Token {
    std::string name;
    size_t start;
    size_t end;

    bool is_malformed = false;
    std::string err_desc = "";
};

template <typename T>
class Tokens {
  public:
    Tokens() = default;
    Tokens(const std::vector<Token>&, const EntityMap<T, TokenId>&);
    Token& operator[](TokenId); // TODO: returns a copy?
    Token& operator[](T); // TODO: returns a copy?

    std::vector<Token> _vec;
    EntityMap<T, TokenId> _map;
};
using LV1Tokens = Tokens<LV1::Ast_>;
using LV2Tokens = Tokens<LV2::Ast_>;

#endif // TOKENS_H
