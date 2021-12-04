#include "token.h"

namespace ilang {

const char* Token::GetTypeName() const { return GetTokenTypeName(meta_.type_); }

TokenType Token::GetType() const { return meta_.type_; }

TokenSpawner::TokenSpawner() : cur_token_id_(0), str_buffer_() {}

Token TokenSpawner::Spawn(TokenType type, std::string&& str) {
  Token t = {.meta_ = MakeTokenMeta(type)};
  t.data_.str_idx_ = StoreString(std::move(str));
  return t;
}

Token TokenSpawner::Spawn(TokenType type, int64_t num) {
  Token t = {.meta_ = MakeTokenMeta(type)};
  t.data_.int_ = num;
  return t;
}

Token TokenSpawner::Spawn(TokenType type, double num) {
  Token t = {.meta_ = MakeTokenMeta(type)};
  t.data_.fp_ = num;
  return t;
}

Token TokenSpawner::Spawn(TokenType type) {
  Token t = {.meta_ = MakeTokenMeta(type)};
  return t;
}

std::string TokenSpawner::ToString(Token token) const {
  std::stringstream ss;

  ss << token.GetTypeName();

  ss << "(";

  switch (token.meta_.type_) {
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

const std::string& TokenSpawner::GetString(TokenStrId id) const {
  return str_buffer_[id];
}

TokenMeta TokenSpawner::MakeTokenMeta(TokenType type) {
  return {type, cur_token_id_++};
}

TokenStrId TokenSpawner::StoreString(std::string&& str) {
  str_buffer_.emplace_back(std::move(str));
  return str_buffer_.size() - 1;
}

}  // namespace ilang
