#pragma once

#include "token_type.h"
#include "util/strong_typedef.h"

namespace ilang {

DEFINE_INDEX(TokenId, int32_t);
DEFINE_INDEX(StrId, int32_t);
DEFINE_INDEX(StrBlockId, int32_t);
DEFINE_INDEX(ExprId, int32_t);
DEFINE_INDEX(ResolveId, int32_t);
DEFINE_INDEX(FrameInfoId, int32_t);
DEFINE_INDEX(ExprBlockId, int32_t);
DEFINE_INDEX(TokenTypeBlockId, int32_t);
DEFINE_INDEX(StmtId, int32_t);
DEFINE_INDEX(StmtBlockId, int32_t);
DEFINE_INDEX(StackFrameId, int32_t);
DEFINE_INDEX(VariableIdx, int16_t);

}  // namespace ilang
