#include "slip/token_type.hpp"

namespace slip {

const char* GetTokenTypeName(TokenType type) {
  switch (type) {
#define SLIP_PUT_TOKEN_NAME(name)   \
  case TokenType::name: {           \
    return #name;                   \
  }
    SLIP_FORALL_TOKEN_TYPES(SLIP_PUT_TOKEN_NAME)
#undef SLIP_PUT_TOKEN_NAME
  }
  throw std::runtime_error("invalid token type");
}

}  // namespace slip
