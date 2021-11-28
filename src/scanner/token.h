#pragma once

#include <string>
#include <sstream>

#include "util/string_tools.h"

namespace scanner
{


#define INTERP_FORALL_TOKEN_TYPES(_) \
  /* Single-character tokens. */ \
  _(LEFT_PAREN) \
  _(RIGHT_PAREN) \
  _(LEFT_BRACE) \
  _(RIGHT_BRACE) \
  _(LEFT_BRAKET) \
  _(RIGHT_BRAKET) \
  _(COMMA) \
  _(DOT) \
  _(MINUS) \
  _(PLUS) \
  _(COLON) \
  _(SEMICOLON) \
  _(SLASH) \
  _(STAR) \
  /* One or two character tokens. */ \
  _(BANG) \
  _(BANG_EQUAL) \
  _(EQUAL) \
  _(EQUAL_EQUAL) \
  _(GREATER) \
  _(GREATER_EQUAL) \
  _(LESS) \
  _(LESS_EQUAL) \
  /* Literals. */ \
  _(IDENTIFIER) \
  _(STRING) \
  _(INT_LITERAL) \
  _(FLOAT_LITERAL) \
  /* Keywords. */ \
  _(AND) \
  _(OR) \
  _(NOT) \
  _(IF) \
  _(ELSE) \
  _(TRUE) \
  _(FALSE) \
  _(CLASS) \
  _(DEF) \
  _(RETURN) \
  _(FOR) \
  _(IN) \
  _(WHILE) \
  _(NONE) \
  _(SUPER) \
  _(THIS) \
  /* Non lang. */ \
  _(INDENT) \
  _(UNINDENT) \
  _(END_OF_FILE) \
  _(EMPTY_TOKEN) \
  _(COMMENT) \
  _(BAD_TOKEN)


using TokenId = int32_t;
using TokenStrId = int32_t;

enum class TokenType : uint8_t
{
#define INTERP_PUT_WITH_COMMA(_) _,
  INTERP_FORALL_TOKEN_TYPES(INTERP_PUT_WITH_COMMA)
#undef INTERP_PUT_WITH_COMMA
};

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

  const char* GetTypeName() const
  {
    switch (meta_.type_)
    {
#define INTERP_PUT_TOKEN_NAME(_) case TokenType::_: { return #_; }
      INTERP_FORALL_TOKEN_TYPES(INTERP_PUT_TOKEN_NAME)
#undef INTERP_PUT_TOKEN_NAME
    }
    throw std::runtime_error("invalid token type");
  }

  TokenType GetType() const { return meta_.type_; }
};

class TokenSpawner
{
public:
  TokenSpawner() :
    cur_token_id_(0),
    str_buffer_()
  {}

  Token Spawn(TokenType type, std::string&& str)
  {
    Token t;
    t.meta_ = MakeTokenMeta(type);
    t.data_.str_idx_ = StoreString(std::move(str));
    return t;
  }

  Token Spawn(TokenType type, int64_t num)
  {
    Token t;
    t.meta_ = MakeTokenMeta(type);
    t.data_.int_ = num;
    return t;
  }

  Token Spawn(TokenType type, double num)
  {
    Token t;
    t.meta_ = MakeTokenMeta(type);
    t.data_.fp_ = num;
    return t;
  }

  Token Spawn(TokenType type)
  {
    Token t;
    t.meta_ = MakeTokenMeta(type);
    return t;
  }


  std::string ToString(Token token) const
  {
    std::stringstream ss;

    ss << token.GetTypeName();

    ss << "(";

    switch (token.meta_.type_)
    {
      case TokenType::INT_LITERAL:
        ss << std::to_string(token.data_.int_);
        break;
      case TokenType::FLOAT_LITERAL:
        ss << std::to_string(token.data_.fp_);
        break;
      case TokenType::COMMENT:
      case TokenType::STRING:
      case TokenType::IDENTIFIER:
        ss << "\"" << str_buffer_[token.data_.str_idx_] << "\"";
        break;
      default:
        break;
    }

    ss << ")";
    return ss.str();
  }


  TokenMeta MakeTokenMeta(TokenType type)
  {
    return {type, cur_token_id_++};
  }

  TokenStrId StoreString(std::string&& str)
  {
    str_buffer_.emplace_back(std::move(str));
    return str_buffer_.size() - 1;
  }

private:
  TokenId cur_token_id_;

  std::vector<std::string> str_buffer_;
};

#undef INTERP_FORALL_TOKEN_TYPES

} // scanner