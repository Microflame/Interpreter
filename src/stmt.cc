#include "stmt.h"

namespace ilang
{

const char* StmtTypeToString(Stmt::Type type)
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
