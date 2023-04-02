#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "slip/expr.hpp"
#include "slip/context.hpp"
#include "slip/stmt.hpp"

namespace slip {

struct FrameInfo {
  VariableIdx frame_size;
};

struct VariableLocation {
  enum Location: uint8_t {
    NOT_FOUND,
    LOCAL,
    GLOBAL,
  };

  VariableIdx idx;
  Location location;

  static const char* LocationToString(Location loc) {
    switch (loc) {
      case NOT_FOUND: return "NOT_FOUND";
      case LOCAL: return "LOCAL";
      case GLOBAL: return "GLOBAL";
    }
    return "BAD_LOCATION";
  }
};

class Resolver {
 public:

  Resolver(const Context& ctx) : ctx_(ctx) {}

  FrameInfo GetFrameInfo(FrameInfoId id) const { return frame_infos_[id]; }
  VariableLocation GetVariableLocation(ResolveId id) const { return resolve_[id]; }

  void ResolveStmts(const std::vector<StmtId>& stmts) {
    PushCtx();
    StrId id = ctx_.FindStrId("print");
    if (id == -1) {
      throw "print id not found";
    }
    PushVariable(id);

    for (StmtId id : stmts) {
      ResolveStmt(id);
    }

    VariableIdx frame_size = contexts_.back().size();
    SetFrameInfo(0, frame_size);
  }

  void ResolveStmt(StmtId id) {
    if (id < 0) return;
    ResolveStmt(ctx_.stmts_[id]);
  }

  void ResolveStmtBlock(StmtBlockId id) {
    if (id < 0) return;
    const StmtBlock& block = ctx_.stmt_blocks_[id];
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
    VariableIdx idx = PushVariable(s.name_);
    Resolve(s.id_, VariableLocation{.idx = idx, .location = VariableLocation::LOCAL});
    // std::cerr << "RES/Def: " << ctx_.strs_[s.name_] << ", " << VariableLocation::LocationToString(VariableLocation::LOCAL) << ", idx: " << idx << "\n";
    PushCtx();
    if (s.params_ != -1) {
      const StrBlock& block = ctx_.str_blocks_[s.params_];
      for (StrId id : block) {
        PushVariable(id);
      }
    }
    ResolveStmtBlock(s.body_);

    VariableIdx frame_size = contexts_.back().size();
    SetFrameInfo(s.frame_info_, frame_size);
    PopCtx();
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
        VariableLocation loc = FindVariableLocation(e.name_);
        if (loc.location == VariableLocation::NOT_FOUND) {
          loc.location = VariableLocation::LOCAL;
          loc.idx = PushVariable(e.name_);
        }
        Resolve(e.id_, loc);
        // std::cerr << "RES/Assign: " << ctx_.strs_[e.name_] << ", " << VariableLocation::LocationToString(loc.location) << ", idx: " << loc.idx << "\n";
        
        ResolveExpr(e.value_);
        break;
      }
      case Expr::VARIABLE: {
        VariableExpr e = expr.variable_;
        VariableLocation loc = FindVariableLocation(e.name_);
        if (loc.location == VariableLocation::NOT_FOUND) {
          throw std::runtime_error("Undefined variable " + ctx_.strs_[e.name_]);
        }
        Resolve(expr.variable_.id_, loc);
        // std::cerr << "RES/Variable: " << ctx_.strs_[e.name_] << ", " << VariableLocation::LocationToString(loc.location) << ", idx: " << loc.idx << "\n";
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

  void Resolve(ResolveId id, VariableLocation::Location location, VariableIdx idx) {
    Resolve(id, VariableLocation{.idx = idx, .location = location});
  }

  void Resolve(ResolveId id, VariableLocation vl) {
    size_t required_size = id + 1;
    if (resolve_.size() < required_size) {
      resolve_.resize(required_size);
    }
    resolve_[id] = vl;
  }

  void SetFrameInfo(FrameInfoId id, VariableIdx size) {
    size_t required_size = id + 1;
    if (frame_infos_.size() < required_size) {
      frame_infos_.resize(required_size);
    }
    frame_infos_[id] = {size};
  }

  VariableIdx FindVariableIdx(StrId name, const std::vector<StrId>& ctx) {
    for (size_t i = 0; i < ctx.size(); i++)
    {
      if (name == ctx[i]) {
        return i;
      }
    }
    return -1;
  }

  VariableLocation FindVariableLocation(StrId name) {
    VariableIdx idx; 
    
    const Context& local_context = contexts_.back();
    idx = FindVariableIdx(name, local_context);
    if (idx != -1) {
      return VariableLocation{.idx = idx, .location = VariableLocation::LOCAL};
    }

    const Context& global_context = contexts_[0];
    idx = FindVariableIdx(name, global_context);
    if (idx != -1) {
      return VariableLocation{.idx = idx, .location = VariableLocation::GLOBAL};
    }
    
    return VariableLocation{.idx = -1, .location = VariableLocation::NOT_FOUND};
  }

  VariableIdx PushVariable(StrId name) {
    Context& context = contexts_.back();
    VariableIdx idx = FindVariableIdx(name, context);
    if (idx != -1) {
      return idx;
    }
    idx = context.size();
    context.push_back(name);
    return idx;
  }

  void PushCtx() {
    contexts_.emplace_back();
  }
  void PopCtx() {
    contexts_.pop_back();
  }

 private:
  using Context = std::vector<StrId>;

  std::vector<Context> contexts_;
  std::vector<VariableLocation> resolve_;
  std::vector<FrameInfo> frame_infos_;
  const Context& ctx_;
};

}  // namespace slip
