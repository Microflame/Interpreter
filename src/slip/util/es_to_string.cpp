#include "slip/util/es_to_string.hpp"

#include <sstream>
#include <vector>

#include "slip/context.hpp"
#include "slip/token.hpp"

namespace slip {

std::string ExprBlockToString(ExprBlockId id, const Context& ctx) {
  if (id < -1) return {};
  std::stringstream ss;
  const std::vector<Expr>& exprs = ctx.expr_blocks_[id];
  for (Expr e : exprs) {
    ss << ExprToString(e, ctx) << ", ";
  }
  return ss.str();
}

std::string ExprToString(Expr expr, const Context& ctx) {
  std::stringstream ss;
  switch (expr.type_) {
    case (Expr::THIS): {
      break;
    }
    case (Expr::SUPER): {
      break;
    }
    case (Expr::GET): {
      GetExpr e = expr.get_;
      ss << ExprToString(e.object_, ctx) << "."
         << TokenStrToString(e.name_, ctx);
      break;
    }
    case (Expr::SET): {
      SetExpr e = expr.set_;
      ss << ExprToString(e.object_, ctx) << "."
         << TokenStrToString(e.name_, ctx);
      ss << " = " << ExprToString(e.value_, ctx);
      break;
    }
    case (Expr::ASSIGN): {
      AssignExpr e = expr.assign_;
      ss << TokenStrToString(e.name_, ctx) << " = "
         << ExprToString(e.value_, ctx);
      break;
    }
    case (Expr::BINARY): {
      BinaryExpr e = expr.binary_;
      ss << "(";
      ss << ExprToString(e.left_, ctx);
      ss << " " << GetTokenTypeName(e.op_) << " ";
      ss << ExprToString(e.right_, ctx);
      ss << ")";
      break;
    }
    case (Expr::COMPARISON): {
      ComparisonExpr e = expr.comparison_;
      const std::vector<Expr>& comps = ctx.expr_blocks_[e.comparables_];
      const std::vector<TokenType>& ops = ctx.token_type_blocks_[e.ops_];
      for (int i = 0; i < (int)comps.size() - 1; ++i) {
        ss << "(" << ExprToString(comps[i], ctx);
        ss << " " << GetTokenTypeName(ops[i]) << " ";
        ss << ExprToString(comps[i + 1], ctx) << ") && ";
      }
      break;
    }
    case (Expr::LOGICAL): {
      LogicalExpr e = expr.logical_;
      ss << ExprToString(e.left_, ctx);
      ss << " " << GetTokenTypeName(e.op_) << " ";
      ss << ExprToString(e.right_, ctx);
      break;
    }
    case (Expr::GROUPING): {
      break;
    }
    case (Expr::LITERAL): {
      LiteralExpr e = expr.literal_;
      switch (e.val_.type_) {
        case (Object::INT): {
          ss << e.val_.int_;
          break;
        }
        case (Object::FLOAT): {
          ss << e.val_.fp_;
          break;
        }
        case (Object::STRING):
        case (Object::IDENTIFIER): {
          ss << TokenStrToString(e.val_.str_id_, ctx);
          break;
        }
        case (Object::BOOLEAN): {
          ss << (e.val_.int_ ? "true" : "false");
          break;
        }
        case (Object::CALLABLE): {
          ss << "<CALLABLE>";
          break;
        }
        case (Object::CLASS): {
          ss << "<CLASS>";
          break;
        }
        case (Object::INSTANCE): {
          ss << "<INSTANCE>";
          break;
        }
        case (Object::NONE): {
          ss << "NULL";
          break;
        }
        case (Object::BUILTIN_FUNCTION): {
          ss << "<BUILTIN_FUNCTION>";
          break;
        }
        case (Object::USER_FUNCTION): {
          ss << "<USER_FUNCTION>";
          break;
        }
      }
      break;
    }
    case (Expr::UNARY): {
      UnaryExpr e = expr.unary_;
      ss << GetTokenTypeName(e.op_);
      ss << ExprToString(e.right_, ctx);
      break;
    }
    case (Expr::VARIABLE): {
      VariableExpr e = expr.variable_;
      ss << TokenStrToString(e.name_, ctx);
      break;
    }
    case (Expr::CALL): {
      CallExpr e = expr.call_;
      ss << ExprToString(e.callee_, ctx);
      ss << "(";
      ss << ExprBlockToString(e.args_, ctx);
      ss << ")";
      break;
    }
  }
  return ss.str();
}

std::string ExprToString(ExprId id, const Context& ctx) {
  if (id < 0) return {};
  Expr expr = ctx.exprs_[id];
  return ExprToString(expr, ctx);
}

std::string TokenStrToString(StrId id, const Context& ctx) {
  if (id < 0) return {};
  return ctx.GetStr(id);
}

std::string StrBlockToString(StrBlockId str_block, const Context& ctx) {
  if (str_block < 0) return {};
  std::stringstream ss;
  for (StrId token_str : ctx.str_blocks_[str_block]) {
    ss << TokenStrToString(token_str, ctx);
  }
  return ss.str();
}

std::string StmtBlockToString(StmtBlockId id, const Context& ctx) {
  if (id < 0) return {};
  std::stringstream ss;
  for (Stmt stmt : ctx.stmt_blocks_[id]) {
    ss << StmtToString(stmt, ctx);
  }
  return ss.str();
}

std::string StmtToString(Stmt stmt, const Context& ctx) {
  std::stringstream ss;
  switch (stmt.type_) {
    case Stmt::RETURN: {
      ss << "return " << ExprToString(stmt.return_.value_, ctx) << ";\n";
      break;
    }
    case Stmt::DEF: {
      DefStmt s = stmt.def_;
      ss << "def ";
      ss << TokenStrToString(s.name_, ctx);
      ss << "(";
      ss << StrBlockToString(s.params_, ctx);
      ss << ") {\n";
      ss << StmtBlockToString(s.body_, ctx);
      ss << "}\n";
      break;
    }
    case Stmt::CLASS: {
      break;
    }
    case Stmt::IF: {
      IfStmt s = stmt.if_;
      ss << "if (";
      ss << ExprToString(s.condition_, ctx);
      ss << ") {\n";
      ss << StmtToString(s.true_branch_, ctx);
      ss << "} else {\n";
      ss << StmtToString(s.false_branch_, ctx);
      ss << "}\n";
      break;
    }
    case Stmt::BLOCK: {
      BlockStmt s = stmt.block_;
      ss << "{\n";
      ss << StmtBlockToString(s.statements_, ctx);
      ss << "}\n";
      break;
    }
    case Stmt::EXPRESSION: {
      ExpressionStmt s = stmt.expression_;
      ss << ExprToString(s.expr_, ctx) << ";\n";
      break;
    }
    case Stmt::WHILE: {
      WhileStmt s = stmt.while_;
      ss << "while (";
      ss << ExprToString(s.condition_, ctx);
      ss << ") {\n";
      ss << StmtToString(s.body_, ctx);
      ss << "}\n";
      break;
    }
  }
  return ss.str();
}

std::string StmtToString(StmtId id, const Context& ctx) {
  if (id < 0) return {};
  return StmtToString(ctx.stmts_[id], ctx);
}

}  // namespace slip
