#pragma once

#include <cstdint>

#include "slip/util/strong_typedef.hpp"

namespace slip {

SLIP_DEFINE_INDEX(TokenId, int32_t);
SLIP_DEFINE_INDEX(StrId, int32_t);
SLIP_DEFINE_INDEX(StrBlockId, int32_t);
SLIP_DEFINE_INDEX(ExprId, int32_t);
SLIP_DEFINE_INDEX(ResolveId, int32_t);
SLIP_DEFINE_INDEX(ScopeInfoId, int32_t);
SLIP_DEFINE_INDEX(ExprBlockId, int32_t);
SLIP_DEFINE_INDEX(TokenTypeBlockId, int32_t);
SLIP_DEFINE_INDEX(StmtId, int32_t);
SLIP_DEFINE_INDEX(StmtBlockId, int32_t);
SLIP_DEFINE_INDEX(StackFrameId, int32_t);
SLIP_DEFINE_INDEX(VariableIdx, int16_t);

}  // namespace slip
