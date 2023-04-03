#pragma once

#include <vector>

#include "slip/expr.hpp"
#include "slip/stmt.hpp"
#include "slip/token_type.hpp"
#include "slip/builtin/functions.hpp"

namespace slip {

using StmtBlock = std::vector<Stmt>;
using ExprBlock = std::vector<Expr>;
using StrBlock = std::vector<StrId>;
using TokenTypeBlock = std::vector<TokenType>;

class Context {
public:
  StrId FindStrId(const std::string& str) const;
  const std::string& GetStr(StrId id) const;

  StmtId PushStmt(Stmt stmt);
  ExprId PushExpr(Expr expr);
  StrId RegisterStr(std::string str);

  StmtBlockId AddStmtBlock();
  ExprBlockId AddExprBlock();
  StrBlockId AddStrBlock();
  TokenTypeBlockId AddTokenTypeBlock();

  std::vector<Stmt> stmts_;
  std::vector<Expr> exprs_;
  std::vector<StmtBlock> stmt_blocks_;
  std::vector<ExprBlock> expr_blocks_;
  std::vector<StrBlock> str_blocks_;
  std::vector<TokenTypeBlock> token_type_blocks_;
  std::vector<BuiltinFunction> builtin_functions_;

private:
  std::vector<std::string> strs_;
};

}  // namespace slip
