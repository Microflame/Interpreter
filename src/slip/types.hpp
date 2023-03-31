#pragma once

#include "slip/token_type.hpp"
#include "slip/util/strong_typedef.hpp"

namespace slip {

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

}  // namespace slip
