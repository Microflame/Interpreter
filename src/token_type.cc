#include "token_type.h"

namespace ilang
{

const char* GetTokenTypeName(TokenType type)
{
  switch (type)
  {
#define INTERP_PUT_TOKEN_NAME(_) case TokenType::_: { return #_; }
    ILANG_FORALL_TOKEN_TYPES(INTERP_PUT_TOKEN_NAME)
#undef INTERP_PUT_TOKEN_NAME
  }
  throw std::runtime_error("invalid token type");
}

} // namespace ilang
