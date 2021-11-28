#pragma once

#include <vector>

// #include "scanner/scanner.h"
// #include "common/object.h"

namespace ilang
{

using ExprId = int32_t;

struct BaseExpr
{
  ExprId id_;
};

struct ThisExpr : BaseExpr
{
  // scanner::Token name_;
};

struct SuperExpr : BaseExpr
{
  // scanner::Token name_;
  // scanner::Token method_;
};

struct GetExpr : BaseExpr
{
  // const Expr* object_;
  // scanner::Token name_;
};

struct SetExpr : BaseExpr
{
  // const Expr* object_;
  // const Expr* value_;
  // scanner::Token name_;
};

struct AssignExpr : BaseExpr
{
  // const Expr* left_;
  // const Expr* value_;
  // scanner::Token name_;
};

struct BinaryExpr : BaseExpr
{
  // const Expr* left_;
  // const Expr* right_;
  // scanner::Token op_;
};

struct LogicalExpr : BaseExpr
{
  // const Expr* left_;
  // const Expr* right_;
  // scanner::Token op_;
};

struct GroupingExpr : BaseExpr
{
  // const Expr* expr_;
};

struct LiteralExpr : BaseExpr
{
  // const Object* val_;
};

struct UnaryExpr : BaseExpr
{
  // const Expr* right_;
  // scanner::Token op_;
};

struct VariableExpr : BaseExpr
{
  // scanner::Token name_;
};

struct CallExpr : BaseExpr
{
  // const Expr* callee_;
  // scanner::Token paren_;
  // const std::vector<const Expr*>* args_;
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

  union
  {
    BaseExpr base_;
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
  };
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