#include "es_to_string.h"

#include <sstream>
#include <vector>

#include "expr_stmt_pool.h"
#include "token.h"

namespace ilang {

std::string ExprBlockToString(ExprBlockId id, const ExprStmtPool& pool) {
  if (id < -1) return {};
  std::stringstream ss;
  const std::vector<Expr>& exprs = pool.expr_blocks_[id];
  for (Expr e : exprs) {
    ss << ExprToString(e, pool) << ", ";
  }
  return ss.str();
}

std::string ExprToString(Expr expr, const ExprStmtPool& pool) {
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
      ss << ExprToString(e.object_, pool) << "."
         << TokenStrToString(e.name_, pool);
      break;
    }
    case (Expr::SET): {
      SetExpr e = expr.set_;
      ss << ExprToString(e.object_, pool) << "."
         << TokenStrToString(e.name_, pool);
      ss << " = " << ExprToString(e.value_, pool);
      break;
    }
    case (Expr::ASSIGN): {
      AssignExpr e = expr.assign_;
      ss << TokenStrToString(e.name_, pool) << " = "
         << ExprToString(e.value_, pool);
      break;
    }
    case (Expr::BINARY): {
      BinaryExpr e = expr.binary_;
      ss << "(";
      ss << ExprToString(e.left_, pool);
      ss << " " << GetTokenTypeName(e.op_) << " ";
      ss << ExprToString(e.right_, pool);
      ss << ")";
      break;
    }
    case (Expr::COMPARISON): {
      ComparisonExpr e = expr.comparison_;
      const std::vector<Expr>& comps = pool.expr_blocks_[e.comparables_];
      const std::vector<TokenType>& ops = pool.token_type_blocks_[e.ops_];
      for (int i = 0; i < (int)comps.size() - 1; ++i) {
        ss << "(" << ExprToString(comps[i], pool);
        ss << " " << GetTokenTypeName(ops[i]) << " ";
        ss << ExprToString(comps[i + 1], pool) << ") && ";
      }
      break;
    }
    case (Expr::LOGICAL): {
      LogicalExpr e = expr.logical_;
      ss << ExprToString(e.left_, pool);
      ss << " " << GetTokenTypeName(e.op_) << " ";
      ss << ExprToString(e.right_, pool);
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
          ss << TokenStrToString(e.val_.str_id_, pool);
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
      }
      break;
    }
    case (Expr::UNARY): {
      UnaryExpr e = expr.unary_;
      ss << GetTokenTypeName(e.op_);
      ss << ExprToString(e.right_, pool);
      break;
    }
    case (Expr::VARIABLE): {
      VariableExpr e = expr.variable_;
      ss << TokenStrToString(e.name_, pool);
      break;
    }
    case (Expr::CALL): {
      CallExpr e = expr.call_;
      ss << ExprToString(e.callee_, pool);
      ss << "(";
      ss << ExprBlockToString(e.args_, pool);
      ss << ")";
      break;
    }
  }
  return ss.str();
}

std::string ExprToString(ExprId id, const ExprStmtPool& pool) {
  if (id < 0) return {};
  Expr expr = pool.exprs_[id];
  return ExprToString(expr, pool);
}

std::string TokenStrToString(StrId id, const ExprStmtPool& pool) {
  if (id < 0) return {};
  return pool.strs_[id];
}

std::string StrBlockToString(StrBlockId str_block, const ExprStmtPool& pool) {
  if (str_block < 0) return {};
  std::stringstream ss;
  for (StrId token_str : pool.str_blocks_[str_block]) {
    ss << TokenStrToString(token_str, pool);
  }
  return ss.str();
}

std::string StmtBlockToString(StmtBlockId id, const ExprStmtPool& pool) {
  if (id < 0) return {};
  std::stringstream ss;
  for (Stmt stmt : pool.stmt_blocks_[id]) {
    ss << StmtToString(stmt, pool);
  }
  return ss.str();
}

std::string StmtToString(Stmt stmt, const ExprStmtPool& pool) {
  std::stringstream ss;
  switch (stmt.type_) {
    case Stmt::RETURN: {
      ss << "return " << ExprToString(stmt.return_.value_, pool) << ";\n";
      break;
    }
    case Stmt::DEF: {
      DefStmt s = stmt.def_;
      ss << "def ";
      ss << TokenStrToString(s.name_, pool);
      ss << "(";
      ss << StrBlockToString(s.params_, pool);
      ss << ") {\n";
      ss << StmtBlockToString(s.body_, pool);
      ss << "}\n";
      break;
    }
    case Stmt::CLASS: {
      break;
    }
    case Stmt::IF: {
      IfStmt s = stmt.if_;
      ss << "if (";
      ss << ExprToString(s.condition_, pool);
      ss << ") {\n";
      ss << StmtToString(s.true_branch_, pool);
      ss << "} else {\n";
      ss << StmtToString(s.false_branch_, pool);
      ss << "}\n";
      break;
    }
    case Stmt::BLOCK: {
      BlockStmt s = stmt.block_;
      ss << "{\n";
      ss << StmtBlockToString(s.statements_, pool);
      ss << "}\n";
      break;
    }
    case Stmt::EXPRESSION: {
      ExpressionStmt s = stmt.expression_;
      ss << ExprToString(s.expr_, pool) << ";\n";
      break;
    }
    case Stmt::WHILE: {
      WhileStmt s = stmt.while_;
      ss << "while (";
      ss << ExprToString(s.condition_, pool);
      ss << ") {\n";
      ss << StmtToString(s.body_, pool);
      ss << "}\n";
      break;
    }
  }
  return ss.str();
}

std::string StmtToString(StmtId id, const ExprStmtPool& pool) {
  if (id < 0) return {};
  return StmtToString(pool.stmts_[id], pool);
}

}  // namespace ilang
