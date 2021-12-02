#pragma once

#include <string>

#include "parser/expr.h"
#include "parser/stmt.h"
#include "types.h"

namespace ilang
{

struct ExprStmtPool;
class TokenSpawner;

struct Pools
{
  const ExprStmtPool& es;
  const TokenSpawner& ts;
};


std::string StrBlockToString(StrBlockId str_block, Pools pools);
std::string ExprBlockToString(ExprBlockId id, Pools pools);
std::string ExprToString(Expr expr, Pools pools);
std::string ExprToString(ExprId id, Pools pools);
std::string TokenStrToString(TokenStrId id, Pools pools);
std::string StmtBlockToString(StmtBlockId id, Pools pools);
std::string StmtToString(Stmt stmt, Pools pools);
std::string StmtToString(StmtId id, Pools pools);

} // namespace ilang
