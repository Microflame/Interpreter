#pragma once

#include <string>
#include <sstream>

#include "token_type.h"
#include "util/string_tools.h"

namespace ilang
{


using TokenId = int32_t;
using TokenStrId = int32_t;

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
    return GetTokenTypeName(meta_.type_);
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


} // ilang