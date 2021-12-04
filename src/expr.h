#pragma once

#include <variant>

#include "object.h"
#include "types.h"

namespace ilang {

struct ThisExpr {
  // scanner::Token name_;
};

struct SuperExpr {
  // scanner::Token name_;
  // scanner::Token method_;
};

struct GetExpr {
  ExprId object_;
  TokenStrId name_;
};

struct SetExpr {
  ExprId object_;
  ExprId value_;
  TokenStrId name_;
};

struct AssignExpr {
  ExprId value_;
  TokenStrId name_;
};

struct BinaryExpr {
  ExprId left_;
  ExprId right_;
  TokenType op_;
};

struct ComparisonExpr {
  ExprBlockId comparables_;
  TokenTypeBlockId ops_;
};

struct LogicalExpr {
  ExprId left_;
  ExprId right_;
  TokenType op_;
};

struct GroupingExpr {
  ExprId expr_;
};

struct LiteralExpr {
  Object val_;
};

struct UnaryExpr {
  ExprId right_;
  TokenType op_;
};

struct VariableExpr {
  TokenStrId name_;
};

struct CallExpr {
  ExprId callee_;
  ExprBlockId args_;
};

struct Expr {
  enum Type : uint8_t {
    THIS,
    SUPER,
    GET,
    SET,
    ASSIGN,
    BINARY,
    COMPARISON,
    LOGICAL,
    GROUPING,
    LITERAL,
    UNARY,
    VARIABLE,
    CALL
  } type_;

  ExprId id_;  // TODO: Do we need this field in every Expr?

  union {
    ThisExpr this_;
    SuperExpr super_;
    GetExpr get_;
    SetExpr set_;
    AssignExpr assign_;
    BinaryExpr binary_;
    ComparisonExpr comparison_;
    LogicalExpr logical_;
    GroupingExpr grouping_;
    LiteralExpr literal_;
    UnaryExpr unary_;
    VariableExpr variable_;
    CallExpr call_;
  };
};

const char* ExprTypeToString(Expr::Type type);

}  // namespace ilang