#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "expr.h"
#include "expr_stmt_pool.h"
#include "stmt.h"

namespace ilang {

class Resolver {
 public:
  Resolver(const ExprStmtPool& pool) : pool_(pool) {
    PushCtx();
    Define("print");
  }

  int32_t GetDepth(ResolveId id) const { return resolve_[id]; }

  void ResolveStmts(const std::vector<StmtId>& stmts) {
    for (StmtId id : stmts) {
      ResolveStmt(id);
    }
  }

  void ResolveStmt(StmtId id) {
    if (id < 0) return;
    ResolveStmt(pool_.stmts_[id]);
  }

  void ResolveStmtBlock(StmtBlockId id) {
    if (id < 0) return;
    const StmtBlock& block = pool_.stmt_blocks_[id];
    for (Stmt s : block) {
      ResolveStmt(s);
    }
  }

  void ResolveStmt(Stmt stmt) {
    switch (stmt.type_) {
      case Stmt::RETURN: {
        ReturnStmt s = stmt.return_;
        ResolveExpr(s.value_);
        break;
      }
      case Stmt::DEF: {
        DefStmt s = stmt.def_;
        Define(s.name_);
        ResolveFunction(s);
        break;
      }
      case Stmt::CLASS: {
        // ClassStmt s = stmt.class_;
        // TODO
        break;
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
        // PushCtx();
        ResolveStmtBlock(s.statements_);
        // PopCtx();
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

  void ResolveFunction(DefStmt s) {
    PushCtx();
    if (s.params_ != -1) {
      const StrBlock& block = pool_.str_blocks_[s.params_];
      for (StrId id : block) {
        Define(id);
      }
    }
    ResolveStmtBlock(s.body_);
    PopCtx();
  }

  void ResolveExpr(ExprId id) {
    if (id < 0) return;
    ResolveExpr(pool_.exprs_[id]);
  }

  void ResolveExprBlock(ExprBlockId id) {
    if (id < 0) return;
    const ExprBlock& block = pool_.expr_blocks_[id];
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
        ResolveExpr(e.value_);
        const std::string& name = pool_.strs_[e.name_];
        // int32_t depth = FindDepth(name);
        // if (depth < 0) {
        //   Define(name);
        //   depth = 0;
        // }
        // Resolve(expr.assign_.id_, depth, name);
        Define(name);
        Resolve(expr.assign_.id_, 0, name);
        break;
      }
      case Expr::VARIABLE: {
        VariableExpr e = expr.variable_;
        const std::string& name = pool_.strs_[e.name_];
        int32_t depth = FindDepth(name);
        if (depth < 0) {
          throw std::runtime_error("Undefined variable " + name);
        }
        Resolve(expr.variable_.id_, depth, name);
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

  void Resolve(ResolveId id, int32_t depth, const std::string& name) {
    // std::cerr << "Resolved " << name << " at " << -depth << '\n';
    if (resolve_.size() < id + 1) {
      resolve_.resize(id + 1);
    }
    resolve_[id] = depth;
  }

  int32_t FindDepth(StrId id) { return FindDepth(pool_.strs_[id]); }
  int32_t FindDepth(const std::string& str) {
    int32_t depth = 0;
    for (auto it = contexts_.rbegin(); it < contexts_.rend(); ++it) {
      if (it->contains(str)) {
        return depth;
      }
      ++depth;
    }
    return -1;
  }

  void Define(StrId id) { Define(pool_.strs_[id]); }
  void Define(const std::string& name) {
    // std::cerr << "Defined " << name << '\n';
    contexts_.back().insert(name);
  }

  void PushCtx() {
    // std::cerr << "PushCtx\n";
    contexts_.emplace_back();
  }
  void PopCtx() {
    // std::cerr << "PopCtx\n";
    contexts_.pop_back();
  }

 private:
  std::vector<std::unordered_set<std::string>> contexts_;
  std::vector<int32_t> resolve_;
  const ExprStmtPool& pool_;
};

}  // namespace ilang
