#include "expr.h"

namespace ilang
{

const char* ExprTypeToString(Expr::Type type)
{
  switch (type)
  {
    case Expr::THIS:        return "THIS";
    case Expr::SUPER:       return "SUPER";
    case Expr::GET:         return "GET";
    case Expr::SET:         return "SET";
    case Expr::ASSIGN:      return "ASSIGN";
    case Expr::BINARY:      return "BINARY";
    case Expr::COMPARISON:  return "COMPARISON";
    case Expr::LOGICAL:     return "LOGICAL";
    case Expr::GROUPING:    return "GROUPING";
    case Expr::LITERAL:     return "LITERAL";
    case Expr::UNARY:       return "UNARY";
    case Expr::VARIABLE:    return "VARIABLE";
    case Expr::CALL:        return "CALL";
  }
  throw std::runtime_error("[ExprTypeToString] Bad type");
}

} // namespace ilang
