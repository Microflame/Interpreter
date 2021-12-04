#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "expr_stmt_pool.h"
#include "token_type.h"
#include "types.h"

namespace ilang {

struct TokenMeta {
  TokenType type_;
  TokenId id_;
};

union TokenData {
  double fp_;
  int64_t int_;
  StrId str_idx_;
};

struct Token {
  TokenMeta meta_;
  TokenData data_;

  const char* GetTypeName() const;
  TokenType GetType() const;
};

class TokenSpawner {
 public:
  TokenSpawner(ExprStmtPool* pool);

  Token Spawn(TokenType type, std::string&& str);
  Token Spawn(TokenType type, int64_t num);
  Token Spawn(TokenType type, double num);
  Token Spawn(TokenType type);

  std::string ToString(Token token) const;
  const std::string& GetString(StrId id) const;

  TokenMeta MakeTokenMeta(TokenType type);
  StrId StoreString(std::string&& str);

 private:
  ExprStmtPool* pool_;
  TokenId cur_token_id_;
};

}  // namespace ilang