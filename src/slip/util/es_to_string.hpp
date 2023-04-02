#pragma once

#include <string>

#include "slip/expr.hpp"
#include "slip/stmt.hpp"
#include "slip/indexing.hpp"

namespace slip {

struct Context;

std::string StrBlockToString(StrBlockId str_block, const Context& ctx);
std::string ExprBlockToString(ExprBlockId id, const Context& ctx);
std::string ExprToString(Expr expr, const Context& ctx);
std::string ExprToString(ExprId id, const Context& ctx);
std::string TokenStrToString(StrId id, const Context& ctx);
std::string StmtBlockToString(StmtBlockId id, const Context& ctx);
std::string StmtToString(Stmt stmt, const Context& ctx);
std::string StmtToString(StmtId id, const Context& ctx);

}  // namespace slip
