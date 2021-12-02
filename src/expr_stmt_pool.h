#pragma once

#include <vector>

#include "expr.h"
#include "stmt.h"
#include "token_type.h"

namespace ilang
{

struct ExprStmtPool
{
  StmtId PushStmt(Stmt stmt)
  {
    StmtId id = statements_.size();
    statements_.push_back(stmt);
    return id;
  }

  ExprId PushExpr(Expr expr)
  {
    ExprId id = expressions_.size();
    expr.id_ = id;
    expressions_.push_back(expr);
    return id;
  }

  StmtBlockId MakeNewStmtBlock()
  {
    StmtBlockId id = stmt_blocks_.size();
    stmt_blocks_.emplace_back();
    return id;
  }

  ExprBlockId MakeNewExprBlock()
  {
    ExprBlockId id = expr_blocks_.size();
    expr_blocks_.emplace_back();
    return id;
  }

  StrBlockId MakeNewStrBlock()
  {
    StrBlockId id = str_blocks_.size();
    str_blocks_.emplace_back();
    return id;
  }

  TokenTypeBlockId MakeNewTokenTypeBlock()
  {
    TokenTypeBlockId id = token_type_blocks_.size();
    token_type_blocks_.emplace_back();
    return id;
  }

  std::vector<Stmt> statements_;
  std::vector<Expr> expressions_;
  std::vector<std::vector<Stmt>> stmt_blocks_;
  std::vector<std::vector<Expr>> expr_blocks_;
  std::vector<std::vector<TokenStrId>> str_blocks_;
  std::vector<std::vector<TokenType>> token_type_blocks_;
};

} // namespace ilang
