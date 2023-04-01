#pragma once

#include <string>

#include "slip/expr.hpp"
#include "slip/stmt.hpp"
#include "slip/indexing.hpp"

namespace slip {

struct ExprStmtPool;

std::string StrBlockToString(StrBlockId str_block, const ExprStmtPool& pool);
std::string ExprBlockToString(ExprBlockId id, const ExprStmtPool& pool);
std::string ExprToString(Expr expr, const ExprStmtPool& pool);
std::string ExprToString(ExprId id, const ExprStmtPool& pool);
std::string TokenStrToString(StrId id, const ExprStmtPool& pool);
std::string StmtBlockToString(StmtBlockId id, const ExprStmtPool& pool);
std::string StmtToString(Stmt stmt, const ExprStmtPool& pool);
std::string StmtToString(StmtId id, const ExprStmtPool& pool);

}  // namespace slip
