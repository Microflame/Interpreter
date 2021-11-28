#pragma once

#include <vector>

#include "expr.h"

namespace parser
{

struct Stmt
{
  enum : uint8_t
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

} // namespace parser
