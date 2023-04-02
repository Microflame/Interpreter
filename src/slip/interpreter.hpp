#pragma once

#include <unordered_map>

#include "slip/builtin/print.hpp"
#include "slip/expr.hpp"
#include "slip/context.hpp"
#include "slip/resolver.hpp"
#include "slip/stmt.hpp"

namespace slip {

class Interpreter {
 public:
  Interpreter(Context& ctx, const Resolver& resolver)
      : ctx_(ctx), resolver_(resolver) {}

  void Interpret(const std::vector<StmtId>& stmts) {
    PushStackFrame(resolver_.GetFrameInfo(0).frame_size);
    AddBuiltins();
    for (StmtId id : stmts) {
      InterpretStmt(id);
    }
    PopStackFrame();
  }

  void AddBuiltins() {
    StrId id = ctx_.FindStrId("print");
    if (id == -1) {
      throw "print id not found";
    }
    GetStackVar(0) = MakeBuiltin(PrintBuiltin); //TODO: Get this index from resolver
  }

  void InterpretStmt(StmtId id) {
    if (id == -1) return;
    Stmt stmt = ctx_.stmts_[id];
    InterpretStmt(stmt);
  }

  void InterpretStmt(Stmt stmt) {
    switch (stmt.type_) {
      case Stmt::RETURN: {
        ReturnStmt s = stmt.return_;
        retval_ = InterpretExpr(s.value_);
        has_return_ = true;
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
    VariableIdx frame_size = resolver_.GetFrameInfo(stmt.frame_info_).frame_size;
    Object fn = MakeUserFn(frame_size, stmt.params_, stmt.body_);

    VariableIdx idx = resolver_.GetVariableLocation(stmt.id_).idx;
    GetStackVar(idx) = fn;
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
    ExecuteStmts(ctx_.stmt_blocks_[id]);
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
    if (id == -1) [[unlikely]] return MakeNone();
    return InterpretExpr(ctx_.exprs_[id]);
  }

  Object InterpretExpr(const Expr& expr) {
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

    size_t num_args = GetExprBlockSize(expr.args_);
    Object* args_dest = (Object*) alloca(num_args * sizeof(Object));
    std::span<Object> args(args_dest, num_args);

    if (expr.args_ != -1) {
      EvalExprBlock(expr.args_, args);
    }
    if (callee.type_ == Object::BUILTIN_FUNCTION) {
      return callee.builtin_fn_(args, ctx_);
    } else {
      return EvalUserFn(callee.user_fn_, args, callee.frame_size_);
    }
  }

  Object EvalUserFn(Object::UserFn callee, std::span<Object> args, VariableIdx frame_size) {
    PushStackFrame(frame_size);

    if (callee.args_block_ != -1) {
      const StrBlock& param_names =
          ctx_.str_blocks_[callee.args_block_];
      if (args.size() != param_names.size()) {
        throw std::runtime_error("[EvalUserFn] Wrong number of arguments");
      }

      for (size_t i = 0; i < args.size(); i++) {
        GetStackVar(i) = args[i];
      }
    }

    ExecuteStmts(callee.stmt_block_);

    PopStackFrame();
    has_return_ = false;
    return retval_;
  }

  size_t GetExprBlockSize(ExprBlockId id)
  {
    if (id == -1) return 0;
    return ctx_.expr_blocks_[id].size();
  }

  void EvalExprBlock(ExprBlockId id, std::span<Object> args_dest) {
    if (id == -1) return;
    const ExprBlock& block = ctx_.expr_blocks_[id];
    return EvalExprBlock(block, args_dest);
  }

  void EvalExprBlock(const ExprBlock& block, std::span<Object> args_dest) {
    if (block.size() != args_dest.size()) {
      throw std::runtime_error("[EvalExprBlock] args/block size mismatch");
    }
    size_t i = 0;
    for (const Expr& e : block) {
      args_dest[i] = InterpretExpr(e);
      i++;
    }
    return;
  }

  Object EvalVariable(VariableExpr expr) {
    VariableLocation loc = resolver_.GetVariableLocation(expr.id_);
    if (loc.location == VariableLocation::LOCAL) {
      return GetStackVar(loc.idx);
    }
    return GetGlobal(loc.idx);
  }

  Object EvalAssign(AssignExpr expr) {
    Object val = InterpretExpr(expr.value_);
    VariableLocation loc = resolver_.GetVariableLocation(expr.id_);
    Object* target = {};
    if (loc.location == VariableLocation::LOCAL) {
      target = &GetStackVar(loc.idx);
    }
    else
    {
      target = &GetGlobal(loc.idx);
    }
    *target = val;
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

  Object EvalComparison(ComparisonExpr expr) {
    size_t num_args = GetExprBlockSize(expr.comparables_);
    Object* args_dest = (Object*) alloca(num_args * sizeof(Object));
    std::span<Object> comps(args_dest, num_args);

    EvalExprBlock(expr.comparables_, comps);

    const std::vector<TokenType>& ops = ctx_.token_type_blocks_[expr.ops_];
    if (comps.size() < 2) {
      throw std::runtime_error("[EvalComparison] Bad number of comparables.");
    }
    if (ops.size() != comps.size() - 1) {
      throw std::runtime_error("[EvalComparison] Bad number of ops.");
    }
    for (size_t i = 0; i < ops.size(); i++) {
      if (!DoCompare(comps[i], comps[i + 1], ops[i])) {
        return MakeBool(false);
      }
    }
    return MakeBool(true);
  }

  bool DoCompare(Object left, Object right, TokenType op) {
    return left.Compare(right, op, ctx_);
  }

  void PushStackFrame(size_t size) {
    size_t old_size = stack_sizes_.back();
    size_t new_size = old_size + size;
    while (stack_.size() < new_size) {
      stack_.resize(stack_.size() ? stack_.size() * 2 : new_size);
    }

    frame_ = std::span<Object>(&stack_[old_size], size);

    stack_sizes_.push_back(old_size + size);
  }

  void PopStackFrame() {
    if (stack_sizes_.size() <= 1) {
      throw std::runtime_error("Stack is already empty");
    }

    stack_sizes_.pop_back();
    size_t cur_size = stack_sizes_.back();
    size_t prev_size = stack_sizes_[stack_sizes_.size() - 2];

    frame_ = std::span<Object>(&stack_[prev_size], cur_size - prev_size);
  }

  Object& GetStackVar(VariableIdx idx) {
    return frame_[idx];
  }

  Object& GetGlobal(VariableIdx idx) {
    return stack_[idx];
  }

 private:
  const Context& ctx_;
  const Resolver& resolver_;
  std::vector<Object> stack_ = {};
  std::vector<size_t> stack_sizes_ = {0, 0};
  std::span<Object> frame_ = {};

  bool has_return_ = false;
  Object retval_ = MakeNone();
};

}  // namespace slip
