#include "es_to_string.h"

#include <sstream>
#include <vector>

#include "expr_stmt_pool.h"
#include "token.h"

namespace ilang
{

std::string ExprBlockToString(ExprBlockId id, Pools pools)
{
  if (id < -1) return {};
  std::stringstream ss;
  const std::vector<Expr>& exprs = pools.es.expr_blocks_[id];
  for (Expr e: exprs)
  {
    ss << ExprToString(e, pools) << ", ";
  }
  return ss.str();
}

std::string ExprToString(Expr expr, Pools pools)
{
  std::stringstream ss;
  switch (expr.type_)
  {
    case (Expr::THIS):
    {

      break;
    }
    case (Expr::SUPER):
    {

      break;
    }
    case (Expr::GET):
    {
      GetExpr e = expr.get_;
      ss << ExprToString(e.object_, pools) << "." << TokenStrToString(e.name_, pools);
      break;
    }
    case (Expr::SET):
    {
      SetExpr e = expr.set_;
      ss << ExprToString(e.object_, pools) << "." << TokenStrToString(e.name_, pools);
      ss << " = " << ExprToString(e.value_, pools);
      break;
    }
    case (Expr::ASSIGN):
    {
      AssignExpr e = expr.assign_;
      ss << TokenStrToString(e.name_, pools) << " = " << ExprToString(e.value_, pools);
      break;
    }
    case (Expr::BINARY):
    {
      BinaryExpr e = expr.binary_;
      ss << ExprToString(e.left_, pools);
      ss << " " << GetTokenTypeName(e.op_) << " ";
      ss << ExprToString(e.right_, pools);
      break;
    }
    case (Expr::COMPARISON):
    {
      ComparisonExpr e = expr.comparison_;
      const std::vector<Expr>& comps = pools.es.expr_blocks_[e.comparables_];
      const std::vector<TokenType>& ops = pools.es.token_type_blocks_[e.ops_];
      for (int i = 0; i < (int) comps.size() - 1; ++i)
      {
        ss << "(" << ExprToString(comps[i], pools);
        ss << " " << GetTokenTypeName(ops[i]) << " ";
        ss << ExprToString(comps[i + 1], pools) << ") &&";
      }
      break;
    }
    case (Expr::LOGICAL):
    {
      LogicalExpr e = expr.logical_;
      ss << ExprToString(e.left_, pools);
      ss << " " << GetTokenTypeName(e.op_) << " ";
      ss << ExprToString(e.right_, pools);
      break;
    }
    case (Expr::GROUPING):
    {

      break;
    }
    case (Expr::LITERAL):
    {
      LiteralExpr e = expr.literal_;
      switch (e.val_.type_)
      {
        case (Object::INT):
        {
          ss << e.val_.int_;
          break;
        }
        case (Object::FLOAT):
        {
          ss << e.val_.fp_;
          break;
        }
        case (Object::STRING):
        case (Object::IDENTIFIER):
        {
          ss << TokenStrToString(e.val_.str_id_, pools);
          break;
        }
        case (Object::BOOLEAN):
        {
          ss << (e.val_.int_ ? "true" : "false");
          break;
        }
        case (Object::CALLABLE):
        {
          ss << "<CALLABLE>";
          break;
        }
        case (Object::CLASS):
        {
          ss << "<CLASS>";
          break;
        }
        case (Object::INSTANCE):
        {
          ss << "<INSTANCE>";
          break;
        }
        case (Object::NONE):
        {
          ss << "NULL";
          break;
        }
      }
      break;
    }
    case (Expr::UNARY):
    {
      UnaryExpr e = expr.unary_;
      ss << GetTokenTypeName(e.op_);
      ss << ExprToString(e.right_, pools);
      break;
    }
    case (Expr::VARIABLE):
    {
      VariableExpr e = expr.variable_;
      ss << TokenStrToString(e.name_, pools);
      break;
    }
    case (Expr::CALL):
    {
      CallExpr e = expr.call_;
      ss << ExprToString(e.callee_, pools);
      ss << "(";
      ss << ExprBlockToString(e.args_, pools);
      ss << ")";
      break;
    }
  }
  return ss.str();
}

std::string ExprToString(ExprId id, Pools pools)
{
  if (id < 0) return {};
  Expr expr = pools.es.expressions_[id];
  return ExprToString(expr, pools);
}

std::string TokenStrToString(TokenStrId id, Pools pools)
{
  if (id < 0) return {};
  return pools.ts.GetString(id);
}

std::string StrBlockToString(StrBlockId str_block, Pools pools)
{
  if (str_block < 0) return {};
  std::stringstream ss;
  for (TokenStrId token_str: pools.es.str_blocks_[str_block])
  {
    ss << TokenStrToString(token_str, pools);
  }
  return ss.str();
}

std::string StmtBlockToString(StmtBlockId id, Pools pools)
{
  if (id < 0) return {};
  std::stringstream ss;
  for (Stmt stmt: pools.es.stmt_blocks_[id])
  {
    ss << StmtToString(stmt, pools);
  }
  return ss.str();
}

std::string StmtToString(Stmt stmt, Pools pools)
{
  std::stringstream ss;
  switch (stmt.type_)
  {
    case Stmt::RETURN:
    {
      ss << "return " << ExprToString(stmt.return_.value_, pools) << ";\n";
      break;
    }
    case Stmt::DEF:
    {
      DefStmt s = stmt.def_;
      ss << "Object ";
      ss << TokenStrToString(s.name_, pools);
      ss << "(";
      ss << StrBlockToString(s.params_, pools);
      ss << ") {\n";
      ss << StmtBlockToString(s.body_, pools);
      ss << "}\n";
      break;
    }
    case Stmt::CLASS:
    {
      break;
    }
    case Stmt::IF:
    {
      IfStmt s = stmt.if_;
      ss << "if (";
      ss << ExprToString(s.condition_, pools);
      ss << ") {\n";
      ss << StmtToString(s.true_branch_, pools);
      ss << "} else {\n";
      ss << StmtToString(s.false_branch_, pools);
      ss << "}\n";
      break;
    }
    case Stmt::BLOCK:
    {
      BlockStmt s = stmt.block_;
      ss << "{\n";
      ss << StmtBlockToString(s.statements_, pools);
      ss << "}\n";
      break;
    }
    case Stmt::EXPRESSION:
    {
      ExpressionStmt s = stmt.expression_;
      ss << ExprToString(s.expr_, pools) << ";\n";
      break;
    }
    case Stmt::WHILE:
    {
      WhileStmt s = stmt.while_;
      ss << "while (";
      ss << ExprToString(s.condition_, pools);
      ss << ") {\n";
      ss << StmtToString(s.body_, pools);
      ss << "}\n";
      break;
    }
  }
  return ss.str();
}

std::string StmtToString(StmtId id, Pools pools)
{
  if (id < 0) return {};
  return StmtToString(pools.es.statements_[id], pools);
}


} // namespace ilang
