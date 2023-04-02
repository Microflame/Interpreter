#include "slip/context.hpp"

namespace slip
{

StrId Context::FindStrId(const std::string& str) const {
  // TODO: add index
  for (size_t i = 0; i < strs_.size(); i++) {
    if (strs_[i] == str) {
      return i;
    }
  }
  return -1;
}

const std::string& Context::GetStr(StrId id) const {
  return strs_.at(id);
}

StmtId Context::PushStmt(Stmt stmt) {
  StmtId id = stmts_.size();
  stmts_.push_back(stmt);
  return id;
}

ExprId Context::PushExpr(Expr expr) {
  ExprId id = exprs_.size();
  expr.id_ = id;
  exprs_.push_back(expr);
  return id;
}

StrId Context::RegisterStr(std::string str) {
  StrId existing = FindStrId(str);
  if (existing != -1) {
    return existing;
  }

  StrId id = strs_.size();
  strs_.emplace_back(std::move(str));
  return id;
}

StmtBlockId Context::AddStmtBlock() {
  StmtBlockId id = stmt_blocks_.size();
  stmt_blocks_.emplace_back();
  return id;
}

ExprBlockId Context::AddExprBlock() {
  ExprBlockId id = expr_blocks_.size();
  expr_blocks_.emplace_back();
  return id;
}

StrBlockId Context::AddStrBlock() {
  StrBlockId id = str_blocks_.size();
  str_blocks_.emplace_back();
  return id;
}

TokenTypeBlockId Context::AddTokenTypeBlock() {
  TokenTypeBlockId id = token_type_blocks_.size();
  token_type_blocks_.emplace_back();
  return id;
}

} // namespace slip
