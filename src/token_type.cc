#include "token_type.h"

namespace ilang {

const char* GetTokenTypeName(TokenType type) {
  switch (type) {
#define INTERP_PUT_TOKEN_NAME(name) \
  case TokenType::name: {           \
    return #name;                   \
  }
    ILANG_FORALL_TOKEN_TYPES(INTERP_PUT_TOKEN_NAME)
#undef INTERP_PUT_TOKEN_NAME
  }
  throw std::runtime_error("invalid token type");
}

}  // namespace ilang
