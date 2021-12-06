#pragma once

#include <unordered_map>

#include "builtin/print.h"
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
    AddBuiltins();
    for (StmtId id : stmts) {
      InterpretStmt(id);
    }
    PopStackFrame();
  }

  void AddBuiltins() {
    StackFrame& frame = GetCurrentStackFrame();
    frame.Set("print", MakeBuiltin(PrintBuiltin));
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
        InterpretDef(stmt.def_);
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

  void InterpretDef(DefStmt stmt) {
    Object fn = MakeUserFn(GetCurrentStackFrameId(), stmt.params_, stmt.body_);
    const std::string& name = pool_.strs_[stmt.name_];
    GetCurrentStackFrame().Set(name, fn);
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
    // TODO: push frame
    ExecuteStmts(stmt.statements_);
    // TODO: pop frame
  }

  void ExecuteStmts(StmtBlockId id) {
    if (id == -1) return;
    ExecuteStmts(pool_.stmt_blocks_[id]);
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
      case Expr::CALL:
        return EvalCall(expr.call_);
    }
    throw std::runtime_error("[InterpretExpr] Bad expr opcode!");
  }

  Object EvalCall(CallExpr expr) {
    Object callee = InterpretExpr(expr.callee_);
    if (callee.type_ != Object::BUILTIN_FUNCTION &&
        callee.type_ != Object::USER_FUNCTION) {
      throw std::runtime_error("[EvalCall] Bad callee!");
    }

    std::vector<Object> args;
    if (expr.args_ != -1) {
      args = EvalExprBlock(expr.args_);
    }
    if (callee.type_ == Object::BUILTIN_FUNCTION) {
      return callee.builtin_fn_(args, pool_);
    } else {
      return EvalUserFn(callee, args);
    }
  }

  Object EvalUserFn(Object callee, const std::vector<Object>& args) {
    PushStackFrame(callee.stack_frame_);

    if (callee.user_fn_.args_block_ != -1) {
      const StrBlock& param_names =
          pool_.str_blocks_[callee.user_fn_.args_block_];
      if (args.size() != param_names.size()) {
        throw std::runtime_error("[EvalUserFn] Wrong number of arguments");
      }

      StackFrame& sf = GetCurrentStackFrame();
      for (size_t i = 0; i < args.size(); i++) {
        const std::string& name = pool_.strs_[param_names[i]];
        sf.Set(name, args[i]);
      }
    }

    ExecuteStmts(callee.user_fn_.stmt_block_);

    PopStackFrame();
    has_return_ = false;
    return retval_;
  }

  std::vector<Object> EvalExprBlock(ExprBlockId id) {
    if (id == -1) return {};
    const ExprBlock& block = pool_.expr_blocks_[id];
    return EvalExprBlock(block);
  }
  std::vector<Object> EvalExprBlock(const ExprBlock& block) {
    std::vector<Object> res;
    res.reserve(block.size());
    for (Expr e : block) {
      res.push_back(InterpretExpr(e));
    }
    return res;
  }

  Object EvalVariable(VariableExpr expr) {
    const std::string& name = pool_.strs_[expr.name_];
    int32_t depth = resolver_.GetDepth(expr.id_);
    return GetStackFrame(depth).Get(name);
  }

  Object EvalAssign(AssignExpr expr) {
    const std::string& name = pool_.strs_[expr.name_];
    Object val = InterpretExpr(expr.value_);
    int32_t depth = resolver_.GetDepth(expr.id_);
    GetStackFrame(depth).Set(name, val);
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
  StackFrame& GetStackFrame(int32_t depth) {
    StackFrame* sf = &GetCurrentStackFrame();
    while (depth) {
      sf = &(stack_[sf->kPrevious]);
    }
    return *sf;
  }

 private:
  const ExprStmtPool& pool_;
  const Resolver& resolver_;
  std::vector<StackFrame> stack_;

  bool has_return_ = false;
  Object retval_ = MakeNone();
};

}  // namespace ilang
