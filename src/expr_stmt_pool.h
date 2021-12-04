#pragma once

#include <vector>

#include "expr.h"
#include "stmt.h"
#include "token_type.h"

namespace ilang {

using StmtBlock = std::vector<Stmt>;
using ExprBlock = std::vector<Expr>;
using StrBlock = std::vector<StrId>;
using TokenTypeBlock = std::vector<TokenType>;

struct ExprStmtPool {
  StmtId PushStmt(Stmt stmt) {
    StmtId id = stmts_.size();
    stmts_.push_back(stmt);
    return id;
  }

  ExprId PushExpr(Expr expr) {
    ExprId id = exprs_.size();
    expr.id_ = id;
    exprs_.push_back(expr);
    return id;
  }

  StrId PushStr(std::string&& str) {
    StrId id = strs_.size();
    strs_.emplace_back(std::move(str));
    return id;
  }

  StmtBlockId MakeNewStmtBlock() {
    StmtBlockId id = stmt_blocks_.size();
    stmt_blocks_.emplace_back();
    return id;
  }

  ExprBlockId MakeNewExprBlock() {
    ExprBlockId id = expr_blocks_.size();
    expr_blocks_.emplace_back();
    return id;
  }

  StrBlockId MakeNewStrBlock() {
    StrBlockId id = str_blocks_.size();
    str_blocks_.emplace_back();
    return id;
  }

  TokenTypeBlockId MakeNewTokenTypeBlock() {
    TokenTypeBlockId id = token_type_blocks_.size();
    token_type_blocks_.emplace_back();
    return id;
  }

  std::vector<Stmt> stmts_;
  std::vector<Expr> exprs_;
  std::vector<std::string> strs_;
  std::vector<StmtBlock> stmt_blocks_;
  std::vector<ExprBlock> expr_blocks_;
  std::vector<StrBlock> str_blocks_;
  std::vector<TokenTypeBlock> token_type_blocks_;
};

}  // namespace ilang
