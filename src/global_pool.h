#pragma once

#include "scanner/token.h"
#include "parser/expr_stmt_pool.h"

namespace ilang
{

class GlobalPool
{
public:
  static const TokenSpawner* token_spawner;
  static const ExprStmtPool* expr_stmt_pool;
};

} // namespace ilang
