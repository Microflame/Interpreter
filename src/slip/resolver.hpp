#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "slip/expr.hpp"
#include "slip/context.hpp"
#include "slip/stmt.hpp"

#include "slip/indexing.hpp"

namespace slip {

struct ScopeInfo {
  VariableIdx size;
};

enum class VarScope: uint8_t {
  NOT_FOUND,
  LOCAL,
  GLOBAL,
};

struct VarLocation {
  VariableIdx idx;
  VarScope scope;
};

const char* ToString(VarScope scope);

class Resolver {
public:
  Resolver(const Context& ctx);

  ScopeInfo GetScopeInfo(ScopeInfoId id) const;
  VarLocation GetVarLocation(ResolveId id) const;

  void ResolveStmts(const std::vector<StmtId>& stmts);

private:
  using Scope = std::vector<StrId>;
  const Context& ctx_;

  std::vector<Scope> scopes_;
  std::vector<VarLocation> resolved_vars_;
  std::vector<ScopeInfo> resolved_scopes_;

  void ResolveStmt(StmtId id);
  void ResolveStmt(Stmt stmt);
  void ResolveStmtBlock(StmtBlockId id);
  void ResolveFunctionDef(DefStmt s);
  void ResolveExpr(ExprId id);
  void ResolveExprBlock(ExprBlockId id);
  void ResolveExpr(Expr expr);
  void Resolve(ResolveId id, VarScope scope, VariableIdx idx);

  void Resolve(ResolveId id, VarLocation vl);
  void FinishScope(ScopeInfoId id, VariableIdx size);

  VariableIdx FindVariableIdx(StrId name, const Scope& scope) const;
  VarLocation FindVarLocation(StrId name) const;

  VariableIdx PushVar(StrId name);

  Scope& LocalScope();
  Scope& GlobalScope();
  const Scope& LocalScope() const;
  const Scope& GlobalScope() const;
  void PushScope();
  void PopScope();
};

}  // namespace slip
