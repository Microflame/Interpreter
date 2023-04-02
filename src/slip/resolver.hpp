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

const char* ToString(VarScope scope) {
  switch (scope) {
    case VarScope::NOT_FOUND: return "NOT_FOUND";
    case VarScope::LOCAL: return "LOCAL";
    case VarScope::GLOBAL: return "GLOBAL";
  }
  return "BAD_SCOPE";
}

class Resolver {
public:
  Resolver(const Context& ctx) : ctx_(ctx) {}

  ScopeInfo GetScopeInfo(ScopeInfoId id) const { return resolved_scopes_[id]; }
  VarLocation GetVarLocation(ResolveId id) const { return resolved_vars_[id]; }

  void ResolveStmts(const std::vector<StmtId>& stmts) {
    PushScope();
    StrId id = ctx_.FindStrId("print");
    if (id == -1) {
      throw std::runtime_error("print id not found");
    }
    PushVar(id);

    for (StmtId id : stmts) {
      ResolveStmt(id);
    }

    if (scopes_.size() != 1) {
      throw std::runtime_error("Wrong number of scopes in stack");
    }

    VariableIdx scope_size = LocalScope().size();
    FinishScope(0, scope_size);
  }

private:
  using Scope = std::vector<StrId>;
  const Context& ctx_;

  std::vector<Scope> scopes_;
  std::vector<VarLocation> resolved_vars_;
  std::vector<ScopeInfo> resolved_scopes_;

  void ResolveStmt(StmtId id) {
    if (id < 0) return;
    ResolveStmt(ctx_.stmts_[id]);
  }

  void ResolveStmt(Stmt stmt) {
    switch (stmt.type_) {
      case Stmt::RETURN: {
        ResolveExpr(stmt.return_.value_);
        break;
      }
      case Stmt::DEF: {
        ResolveFunctionDef(stmt.def_);
        break;
      }
      case Stmt::CLASS: {
        throw std::runtime_error("Classes are not implemented yet");
      }
      case Stmt::IF: {
        IfStmt s = stmt.if_;
        ResolveExpr(s.condition_);
        ResolveStmt(s.true_branch_);
        ResolveStmt(s.false_branch_);
        break;
      }
      case Stmt::BLOCK: {
        BlockStmt s = stmt.block_;
        ResolveStmtBlock(s.statements_);
        break;
      }
      case Stmt::EXPRESSION: {
        ExpressionStmt s = stmt.expression_;
        ResolveExpr(s.expr_);
        break;
      }
      case Stmt::WHILE: {
        WhileStmt s = stmt.while_;
        ResolveExpr(s.condition_);
        ResolveStmt(s.body_);
        break;
      }
    }
  }

  void ResolveStmtBlock(StmtBlockId id) {
    if (id < 0)
      return;
    const StmtBlock& block = ctx_.stmt_blocks_[id];
    for (Stmt s : block) {
      ResolveStmt(s);
    }
  }

  void ResolveFunctionDef(DefStmt s) {
    VariableIdx idx = PushVar(s.name_);
    Resolve(s.id_, VarLocation{.idx = idx, .scope = VarScope::LOCAL});
    PushScope();
    if (s.params_ != -1) {
      const StrBlock& block = ctx_.str_blocks_[s.params_];
      for (StrId id : block) {
        PushVar(id);
      }
    }
    ResolveStmtBlock(s.body_);

    VariableIdx scope_size = LocalScope().size();
    FinishScope(s.scope_info_, scope_size);
    PopScope();
  }

  void ResolveExpr(ExprId id) {
    if (id < 0) return;
    ResolveExpr(ctx_.exprs_[id]);
  }

  void ResolveExprBlock(ExprBlockId id) {
    if (id < 0) return;
    const ExprBlock& block = ctx_.expr_blocks_[id];
    for (Expr e : block) {
      ResolveExpr(e);
    }
  }

  void ResolveExpr(Expr expr) {
    switch (expr.type_) {
      case Expr::THIS: {
        // ThisExpr e = expr.this_;
        // TODO
        break;
      }
      case Expr::SUPER: {
        // SuperExpr e = expr.super_;
        // TODO
        break;
      }
      case Expr::GET: {
        GetExpr e = expr.get_;
        ResolveExpr(e.object_);
        break;
      }
      case Expr::SET: {
        SetExpr e = expr.set_;
        ResolveExpr(e.object_);
        ResolveExpr(e.value_);
        break;
      }
      case Expr::BINARY: {
        BinaryExpr e = expr.binary_;
        ResolveExpr(e.left_);
        ResolveExpr(e.right_);
        break;
      }
      case Expr::COMPARISON: {
        ComparisonExpr e = expr.comparison_;
        ResolveExprBlock(e.comparables_);
        break;
      }
      case Expr::LOGICAL: {
        LogicalExpr e = expr.logical_;
        ResolveExpr(e.left_);
        ResolveExpr(e.right_);
        break;
      }
      case Expr::GROUPING: {
        GroupingExpr e = expr.grouping_;
        ResolveExpr(e.expr_);
        break;
      }
      case Expr::LITERAL: {
        // No resolve needed
        break;
      }
      case Expr::UNARY: {
        UnaryExpr e = expr.unary_;
        ResolveExpr(e.right_);
        break;
      }
      case Expr::ASSIGN: {
        AssignExpr e = expr.assign_;
        VarLocation loc = FindVarLocation(e.name_);
        if (loc.scope == VarScope::NOT_FOUND) {
          loc.scope = VarScope::LOCAL;
          loc.idx = PushVar(e.name_);
        }
        Resolve(e.id_, loc);        
        ResolveExpr(e.value_);
        break;
      }
      case Expr::VARIABLE: {
        VariableExpr e = expr.variable_;
        VarLocation loc = FindVarLocation(e.name_);
        if (loc.scope == VarScope::NOT_FOUND) {
          throw std::runtime_error("Undefined variable " + ctx_.GetStr(e.name_));
        }
        Resolve(expr.variable_.id_, loc);
        break;
      }
      case Expr::CALL: {
        CallExpr e = expr.call_;
        ResolveExpr(e.callee_);
        ResolveExprBlock(e.args_);
        break;
      }
    }
  }

  void Resolve(ResolveId id, VarScope scope, VariableIdx idx) {
    Resolve(id, VarLocation{.idx = idx, .scope = scope});
  }

  void Resolve(ResolveId id, VarLocation vl) {
    size_t required_size = id + 1;
    if (resolved_vars_.size() < required_size) {
      resolved_vars_.resize(required_size);
    }
    resolved_vars_[id] = vl;
  }

  void FinishScope(ScopeInfoId id, VariableIdx size) {
    size_t required_size = id + 1;
    if (resolved_scopes_.size() < required_size) {
      resolved_scopes_.resize(required_size);
    }
    resolved_scopes_[id] = {.size = size};
  }

  VariableIdx FindVariableIdx(StrId name, const Scope& scope) {
    for (size_t i = 0; i < scope.size(); i++)
    {
      if (name == scope[i]) {
        return i;
      }
    }
    return -1;
  }

  VarLocation FindVarLocation(StrId name) {
    VariableIdx idx;

    idx = FindVariableIdx(name, LocalScope());
    if (idx != -1) {
      return VarLocation{.idx = idx, .scope = VarScope::LOCAL};
    }

    idx = FindVariableIdx(name, GlobalScope());
    if (idx != -1) {
      return VarLocation{.idx = idx, .scope = VarScope::GLOBAL};
    }

    return VarLocation{.idx = -1, .scope = VarScope::NOT_FOUND};
  }

  VariableIdx PushVar(StrId name) {
    VariableIdx idx = FindVariableIdx(name, LocalScope());
    if (idx != -1) {
      return idx;
    }

    idx = LocalScope().size();
    LocalScope().push_back(name);
    return idx;
  }

  Scope& LocalScope() {
    return scopes_.back();
  }

  Scope& GlobalScope() {
    return scopes_[0];
  }

  void PushScope() {
    scopes_.emplace_back();
  }

  void PopScope() {
    if (scopes_.size() < 2) {
      throw std::runtime_error("Can not pop scope");
    }
    scopes_.pop_back();
  }
};

}  // namespace slip
