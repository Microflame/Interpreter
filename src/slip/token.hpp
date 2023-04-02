#pragma once

#include <string>

#include "slip/expr_stmt_pool.hpp"
#include "slip/token_type.hpp"
#include "slip/indexing.hpp"

namespace slip {

class ExprStmtPool;

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
  std::string ToString(const ExprStmtPool& pool) const;
};

}  // namespace slip