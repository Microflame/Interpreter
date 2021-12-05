#pragma once

#include "expr.h"
#include "expr_stmt_pool.h"
#include "resolver.h"
#include "stmt.h"

namespace ilang {

class Interpreter {
 public:
  Interpreter(const ExprStmtPool& pool, const Resolver& resolver)
      : pool_(pool), resolver_(resolver) {}

  void Interpret(const std::vector<StmtId>& stmts) {
    for (StmtId id : stmts) {
      InterpretStmt(id);
    }
  }

  void InterpretStmt(StmtId id) {
    if (id == -1) return;
    Stmt stmt = pool_.stmts_[id];
    InterpretStmt(stmt);
  }

  void InterpretStmt(Stmt stmt) {
    switch (stmt.type_) {
      case Stmt::RETURN: {
        ReturnStmt s = stmt.return_;
        has_return_ = true;
        retval_ = InterpretExpr(s.value_);
        break;
      }
      case Stmt::DEF: {
        DefStmt s = stmt.def_;
        break;
      }
      case Stmt::CLASS: {
        // ClassStmt s = stmt.class_;
        break;
      }
      case Stmt::IF: {
        InterpretIf(stmt.if_);
        break;
      }
      case Stmt::BLOCK: {
        BlockStmt s = stmt.block_;
        break;
      }
      case Stmt::EXPRESSION: {
        ExpressionStmt s = stmt.expression_;
        break;
      }
      case Stmt::WHILE: {
        WhileStmt s = stmt.while_;
        break;
      }
    }
  }

  void InterpretIf(IfStmt stmt) {
    bool cond = InterpretExpr(stmt.condition_).AsBool();
    if (cond) {
      InterpretStmt(stmt.true_branch_);
    } else {
      InterpretStmt(stmt.false_branch_);
    }
  }

  Object InterpretExpr(ExprId id) {
    if (id == -1) return MakeNone();
    Expr expr = pool_.exprs_[id];
    return InterpretExpr(expr);
  }

  Object InterpretExpr(Expr expr) {
    switch (expr.type_) {
      case Expr::THIS: {
        // ThisExpr e = expr.this_;
        break;
      }
      case Expr::SUPER: {
        // SuperExpr e = expr.super_;
        break;
      }
      case Expr::GET: {
        // GetExpr e = expr.get_;
        break;
      }
      case Expr::SET: {
        // SetExpr e = expr.set_;
        break;
      }
      case Expr::BINARY:
        return EvalBinary(expr.binary_);
      case Expr::COMPARISON:
        return EvalComparison(expr.comparison_);
      case Expr::LOGICAL:
        return EvalLogical(expr.logical_);
      case Expr::GROUPING: {
        GroupingExpr e = expr.grouping_;
        break;
      }
      case Expr::LITERAL: {
        LiteralExpr e = expr.literal_;
        break;
      }
      case Expr::UNARY: {
        UnaryExpr e = expr.unary_;
        break;
      }
      case Expr::ASSIGN: {
        AssignExpr e = expr.assign_;
        break;
      }
      case Expr::VARIABLE: {
        VariableExpr e = expr.variable_;
        break;
      }
      case Expr::CALL: {
        CallExpr e = expr.call_;
        break;
      }
    }
  }

  Object EvalLogical(LogicalExpr expr) {
    if (expr.op_ != TokenType::OR && expr.op_ != TokenType::AND) {
      throw std::runtime_error("[Expr::LOGICAL] Bad op!");
    }
    bool is_or = expr.op_ == TokenType::OR;

    bool left = InterpretExpr(expr.left_).AsBool();
    if (left && is_or) {
      return MakeBool(true);
    }
    if (!left && !is_or) {
      return MakeBool(false);
    }

    bool right = InterpretExpr(expr.right_).AsBool();
    bool res = false;
    if (is_or) {
      res = left || right;
    }
    res = left && right;

    return MakeBool(res);
  }

  Object EvalBinary(BinaryExpr expr) {
    Object left = InterpretExpr(expr.left_);
    Object right = InterpretExpr(expr.right_);
    return {};
  }

  Object EvalComparison(ComparisonExpr expr) {}

 private:
  const ExprStmtPool& pool_;
  const Resolver& resolver_;

  bool has_return_ = false;
  Object retval_ = MakeNone();
};

}  // namespace ilang
