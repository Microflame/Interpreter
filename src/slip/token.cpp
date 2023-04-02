#include "slip/token.hpp"

#include "slip/context.hpp"

#include <sstream>

namespace slip {

const char* Token::GetTypeName() const { return GetTokenTypeName(meta_.type_); }

TokenType Token::GetType() const { return meta_.type_; }

std::string Token::ToString(const Context& ctx) const {
  std::stringstream ss;

  ss << GetTypeName();

  ss << "(";

  switch (meta_.type_) {
    case TokenType::INT_LITERAL:
      ss << std::to_string(data_.int_);
      break;
    case TokenType::FLOAT_LITERAL:
      ss << std::to_string(data_.fp_);
      break;
    case TokenType::STRING:
    case TokenType::IDENTIFIER:
      ss << "\"" << ctx.GetStr(data_.str_idx_) << "\"";
      break;
    default:
      break;
  }

  ss << ")";
  return ss.str();
}

}  // namespace slip
