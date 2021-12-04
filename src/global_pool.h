#pragma once

namespace ilang {

class TokenSpawner;
class ExprStmtPool;

class GlobalPool {
 public:
  static const TokenSpawner* token_spawner;
  static const ExprStmtPool* expr_stmt_pool;
};

}  // namespace ilang
