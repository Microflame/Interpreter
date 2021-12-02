#pragma once

#include <vector>

#include "expr.h"

namespace ilang
{

using StmtId = int32_t;
using StmtBlockId = int32_t;

struct ReturnStmt
{
  // scanner::Token tok_;
  ExprId value_;
};

struct DefStmt
{
  TokenStrId name_;
  StrBlockId params_;
  StmtBlockId body_;
};

struct ClassStmt
{
  // scanner::Token name_;
  // const Variable* super_;
  // const std::vector<const Func*>* methods_;
};

struct IfStmt
{
  ExprId condition_;
  StmtId true_branch_;
  StmtId false_branch_;
};

struct BlockStmt
{
  StmtBlockId statements_;
};

struct ExpressionStmt
{
  ExprId expr_;
};

struct WhileStmt
{
  ExprId condition_;
  StmtId body_;
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
