#pragma once

#include <unordered_map>

#include "expr.h"
#include "expr_stmt_pool.h"
#include "resolver.h"
#include "stmt.h"

namespace ilang {

class StackFrame {
 public:
  StackFrame(StackFrameId prev) : kPrevious(prev) {}

  void Set(const std::string& name, Object obj) { variables_[name] = obj; }

  Object Get(const std::string& name) {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
      throw std::runtime_error("[StackFrame::Get] Undefined variable " + name);
    }
    return it->second;
  }

 public:
  const StackFrameId kPrevious;

 private:
  std::unordered_map<std::string, Object> variables_;
};

class Interpreter {
 public:
  Interpreter(const ExprStmtPool& pool, const Resolver& resolver)
      : pool_(pool), resolver_(resolver) {}

  void Interpret(const std::vector<StmtId>& stmts) {
    PushStackFrame();
    for (StmtId id : stmts) {
      InterpretStmt(id);
    }
    PopStackFrame();
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
        InterpretBlock(s);
        break;
      }
      case Stmt::EXPRESSION: {
        ExpressionStmt s = stmt.expression_;
        InterpretExpr(s.expr_);
        break;
      }
      case Stmt::WHILE: {
        WhileStmt s = stmt.while_;
        InterpretWhile(s);
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

  void InterpretBlock(BlockStmt stmt) {
    const StmtBlock& stmts = pool_.stmt_blocks_[stmt.statements_];
    // TODO: push frame
    ExecuteStmts(stmts);
    // TODO: pop frame
  }

  void ExecuteStmts(const StmtBlock& stmts) {
    for (Stmt stmt : stmts) {
      InterpretStmt(stmt);
      if (has_return_) {
        return;
      }
    }
  }

  void InterpretWhile(WhileStmt stmt) {
    while (true) {
      Object cond = InterpretExpr(stmt.condition_);
      bool pred = cond.AsBool();
      if (!pred) {
        break;
      }
      InterpretStmt(stmt.body_);
      if (has_return_) {
        break;
      }
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
        // GroupingExpr e = expr.grouping_;
        break;
      }
      case Expr::LITERAL:
        return expr.literal_.val_;
      case Expr::UNARY:
        return EvalUnary(expr.unary_);
      case Expr::ASSIGN:
        return EvalAssign(expr.assign_);
      case Expr::VARIABLE:
        return EvalVariable(expr.variable_);
      case Expr::CALL: {
        CallExpr e = expr.call_;
        break;
      }
    }
    throw std::runtime_error("[InterpretExpr] Bad expr opcode!");
  }

  Object EvalVariable(VariableExpr expr) {
    const std::string& name = pool_.strs_[expr.name_];
    return GetCurrentStackFrame().Get(name);
  }

  Object EvalAssign(AssignExpr expr) {
    const std::string& name = pool_.strs_[expr.name_];
    Object val = InterpretExpr(expr.value_);
    GetCurrentStackFrame().Set(name, val);
    return val;
  }

  Object EvalUnary(UnaryExpr expr) {
    Object obj = InterpretExpr(expr.right_);
    if (expr.op_ == TokenType::NOT) {
      return MakeBool(!obj.AsBool());
    }
    int64_t sign = 0;
    if (expr.op_ == TokenType::PLUS) {
      sign = 1;
    } else if (expr.op_ == TokenType::MINUS) {
      sign = -1;
    } else {
      throw std::runtime_error("[EvalUnary] Bad op");
    }
    return obj.MultInt(sign);
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
    switch (expr.op_) {
      case TokenType::STAR:
        return left.Mult(right);
      case TokenType::SLASH:
        return left.Div(right);
      case TokenType::PLUS:
        return left.Add(right);
      case TokenType::MINUS:
        return left.Sub(right);
      default:
        throw std::runtime_error("[EvalBinary] Bad op!");
    }
  }

  Object EvalComparison(ComparisonExpr expr) { return MakeNone(); }

  StackFrameId GetCurrentStackFrameId() {
    return (StackFrameId)stack_.size() - 1;
  }
  void PushStackFrame() { PushStackFrame(GetCurrentStackFrameId()); }
  void PushStackFrame(StackFrameId prev) { stack_.emplace_back(prev); }
  void PopStackFrame() { stack_.pop_back(); }
  StackFrame& GetCurrentStackFrame() { return stack_.back(); }

 private:
  const ExprStmtPool& pool_;
  const Resolver& resolver_;
  std::vector<StackFrame> stack_;

  bool has_return_ = false;
  Object retval_ = MakeNone();
};

}  // namespace ilang
