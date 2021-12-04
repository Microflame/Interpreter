#pragma once

#include "strong_typedef.h"
#include "token_type.h"

namespace ilang
{

DEFINE_INDEX(TokenId, int32_t);
DEFINE_INDEX(TokenStrId, int32_t);
DEFINE_INDEX(StrBlockId, int32_t);
DEFINE_INDEX(ExprId, int32_t);
DEFINE_INDEX(ExprBlockId, int32_t);
DEFINE_INDEX(TokenTypeBlockId, int32_t);
DEFINE_INDEX(StmtId, int32_t);
DEFINE_INDEX(StmtBlockId, int32_t);

} // namespace ilang
