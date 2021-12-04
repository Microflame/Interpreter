#pragma once

#include <string>

#include "expr.h"
#include "stmt.h"
#include "types.h"

namespace ilang {

struct ExprStmtPool;

std::string StrBlockToString(StrBlockId str_block, const ExprStmtPool& pool);
std::string ExprBlockToString(ExprBlockId id, const ExprStmtPool& pool);
std::string ExprToString(Expr expr, const ExprStmtPool& pool);
std::string ExprToString(ExprId id, const ExprStmtPool& pool);
std::string TokenStrToString(StrId id, const ExprStmtPool& pool);
std::string StmtBlockToString(StmtBlockId id, const ExprStmtPool& pool);
std::string StmtToString(Stmt stmt, const ExprStmtPool& pool);
std::string StmtToString(StmtId id, const ExprStmtPool& pool);

}  // namespace ilang
