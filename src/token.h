#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "token_type.h"
#include "types.h"

namespace ilang
{

struct TokenMeta
{
  TokenType type_;
  TokenId id_;
};

union TokenData
{
  double fp_;
  int64_t int_;
  TokenStrId str_idx_;
};

struct Token
{
  TokenMeta meta_;
  TokenData data_;

  const char* GetTypeName() const;
  TokenType GetType() const;
};

class TokenSpawner
{
public:
  TokenSpawner();

  Token Spawn(TokenType type, std::string&& str);
  Token Spawn(TokenType type, int64_t num);
  Token Spawn(TokenType type, double num);
  Token Spawn(TokenType type);

  std::string ToString(Token token) const;
  const std::string& GetString(TokenStrId id) const;

  TokenMeta MakeTokenMeta(TokenType type);
  TokenStrId StoreString(std::string&& str);

private:
  TokenId cur_token_id_;
  std::vector<std::string> str_buffer_;
};


} // ilang