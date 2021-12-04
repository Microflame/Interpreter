#pragma once

#include "token_type.h"
#include "util/strong_typedef.h"

namespace ilang {

DEFINE_INDEX(TokenId, int32_t);
DEFINE_INDEX(StrId, int32_t);
DEFINE_INDEX(StrBlockId, int32_t);
DEFINE_INDEX(ExprId, int32_t);
DEFINE_INDEX(ExprBlockId, int32_t);
DEFINE_INDEX(TokenTypeBlockId, int32_t);
DEFINE_INDEX(StmtId, int32_t);
DEFINE_INDEX(StmtBlockId, int32_t);

}  // namespace ilang
