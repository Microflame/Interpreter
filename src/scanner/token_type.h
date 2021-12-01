#pragma once

#include <stdexcept>

#include "types.h"

namespace ilang
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
  _(NEWLINE) \
  _(END_OF_FILE) \
  _(EMPTY_TOKEN) \
  _(COMMENT) \
  _(BAD_TOKEN)

enum class TokenType : uint8_t
{
#define INTERP_PUT_WITH_COMMA(_) _,
  INTERP_FORALL_TOKEN_TYPES(INTERP_PUT_WITH_COMMA)
#undef INTERP_PUT_WITH_COMMA
};

static const char* GetTokenTypeName(TokenType type)
{
  switch (type)
  {
#define INTERP_PUT_TOKEN_NAME(_) case TokenType::_: { return #_; }
    INTERP_FORALL_TOKEN_TYPES(INTERP_PUT_TOKEN_NAME)
#undef INTERP_PUT_TOKEN_NAME
  }
  throw std::runtime_error("invalid token type");
}

#undef INTERP_FORALL_TOKEN_TYPES

} // namespace ilang
