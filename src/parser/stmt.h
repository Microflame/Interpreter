#pragma once

#include <vector>

#include "expr.h"

namespace ilang
{

struct ReturnStmt
{
  // scanner::Token tok_;
  // const Expr* value_;
};

struct DefStmt
{
  // scanner::Token name_;
  // const std::vector<scanner::Token>* value_;
  // const std::vector<const Stmt*>* value_;
};

struct ClassStmt
{
  // scanner::Token name_;
  // const Variable* super_;
  // const std::vector<const Func*>* methods_;
};

struct IfStmt
{
  // const Expr* condition_;
  // const Stmt* branch_true_;
  // const Stmt* branch_false_;
};

struct BlockStmt
{
  // const std::vector<const Stmt*>* statements_;
};

struct ExpressionStmt
{
  // const Expr* expr_;
};

struct WhileStmt
{
  // const Expr* condition_;
  // const Stmt* body_;
};

struct Stmt
{
  enum Type : uint8_t
  {
    RETURN,
    DEF,
    CLASS,
    IF,
    BLOCK,
    EXPRESSION,
    WHILE
  } type_;

  union
  {
    ReturnStmt return_;
    DefStmt def_;
    ClassStmt class_;
    IfStmt if_;
    BlockStmt block_;
    ExpressionStmt expression_;
    WhileStmt while_;
  };
};

static const char* StmtTypeToString(Stmt::Type type)
{
  switch (type)
  {
    case Stmt::RETURN:      return "RETURN";
    case Stmt::DEF:         return "DEF";
    case Stmt::CLASS:       return "CLASS";
    case Stmt::IF:          return "IF";
    case Stmt::BLOCK:       return "BLOCK";
    case Stmt::EXPRESSION:  return "EXPRESSION";
    case Stmt::WHILE:       return "WHILE";
  }
  throw std::runtime_error("[StmtTypeToString] Bad type");
}

} // namespace ilang
