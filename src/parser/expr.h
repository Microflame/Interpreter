#pragma once

#include <vector>

#include "scanner/token.h"
#include "common/object.h"

namespace ilang
{

using ExprId = int32_t;
using ExprBlockId = int32_t;

struct ThisExpr
{
  // scanner::Token name_;
};

struct SuperExpr
{
  // scanner::Token name_;
  // scanner::Token method_;
};

struct GetExpr
{
  ExprId object_;
  TokenStrId name_;
};

struct SetExpr
{
  ExprId object_;
  ExprId value_;
  TokenStrId name_;
};

struct AssignExpr
{
  ExprId left_;
  ExprId value_;
  TokenStrId name_;
};

struct BinaryExpr
{
  ExprId left_;
  ExprId right_;
  TokenType op_;
};

struct LogicalExpr
{
  ExprId left_;
  ExprId right_;
  TokenType op_;
};

struct GroupingExpr
{
  ExprId expr_;
};

struct LiteralExpr
{
  Object val_;
};

struct UnaryExpr
{
  ExprId right_;
  TokenType op_;
};

struct VariableExpr
{
  TokenStrId name_;
};

struct CallExpr
{
  ExprId callee_;
  // scanner::Token paren_;
  ExprBlockId args_;
};

struct Expr
{
  enum Type : uint8_t
  {
    THIS,
    SUPER,
    GET,
    SET,
    ASSIGN,
    BINARY,
    LOGICAL,
    GROUPING,
    LITERAL,
    UNARY,
    VARIABLE,
    CALL
  } type_;

  ExprId id_; //TODO: Do we need this field in every Expr?

  union Data
  {
    ThisExpr this_;
    SuperExpr super_;
    GetExpr get_;
    SetExpr set_;
    AssignExpr assign_;
    BinaryExpr binary_;
    LogicalExpr logical_;
    GroupingExpr grouping_;
    LiteralExpr literal_;
    UnaryExpr unary_;
    VariableExpr variable_;
    CallExpr call_;
  } daa;
};

static const char* ExprTypeToString(Expr::Type type)
{
  switch (type)
  {
    case Expr::THIS:      return "THIS";
    case Expr::SUPER:     return "SUPER";
    case Expr::GET:       return "GET";
    case Expr::SET:       return "SET";
    case Expr::ASSIGN:    return "ASSIGN";
    case Expr::BINARY:    return "BINARY";
    case Expr::LOGICAL:   return "LOGICAL";
    case Expr::GROUPING:  return "GROUPING";
    case Expr::LITERAL:   return "LITERAL";
    case Expr::UNARY:     return "UNARY";
    case Expr::VARIABLE:  return "VARIABLE";
    case Expr::CALL:      return "CALL";
  }
  throw std::runtime_error("[ExprTypeToString] Bad type");
}

} // ilang