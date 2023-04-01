#pragma once

#include <stdexcept>

namespace slip {

#define SLIP_FORALL_TOKEN_TYPES(_)   \
  /* Single-character tokens. */     \
  _(LEFT_PAREN)                      \
  _(RIGHT_PAREN)                     \
  _(LEFT_BRACE)                      \
  _(RIGHT_BRACE)                     \
  _(LEFT_BRAKET)                     \
  _(RIGHT_BRAKET)                    \
  _(COMMA)                           \
  _(DOT)                             \
  _(MINUS)                           \
  _(PLUS)                            \
  _(COLON)                           \
  _(SEMICOLON)                       \
  _(SLASH)                           \
  _(STAR)                            \
  /* One or two character tokens. */ \
  _(BANG)                            \
  _(BANG_EQUAL)                      \
  _(EQUAL)                           \
  _(EQUAL_EQUAL)                     \
  _(GREATER)                         \
  _(GREATER_EQUAL)                   \
  _(LESS)                            \
  _(LESS_EQUAL)                      \
  /* Literals. */                    \
  _(IDENTIFIER)                      \
  _(STRING)                          \
  _(INT_LITERAL)                     \
  _(FLOAT_LITERAL)                   \
  /* Keywords. */                    \
  _(AND)                             \
  _(OR)                              \
  _(NOT)                             \
  _(IF)                              \
  _(ELSE)                            \
  _(TRUE)                            \
  _(FALSE)                           \
  _(CLASS)                           \
  _(DEF)                             \
  _(RETURN)                          \
  _(FOR)                             \
  _(IN)                              \
  _(WHILE)                           \
  _(NONE)                            \
  _(SUPER)                           \
  _(THIS)                            \
  /* Non lang. */                    \
  _(INDENT)                          \
  _(UNINDENT)                        \
  _(NEWLINE)                         \
  _(END_OF_FILE)                     \
  _(BAD_TOKEN)

enum class TokenType : uint8_t {
#define SLIP_PUT_WITH_COMMA(_) _,
  SLIP_FORALL_TOKEN_TYPES(SLIP_PUT_WITH_COMMA)
#undef SLIP_PUT_WITH_COMMA
};

const char* GetTokenTypeName(TokenType type);

}  // namespace slip
