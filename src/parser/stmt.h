#pragma once

#include "expr.h"

namespace parser
{
namespace stmt
{

template <typename T>
using Ptr = std::shared_ptr<T>;

class Expression;
class Print;
class Var;

class IStmtVisitor
{
public:
  virtual void Visit(const Expression&) = 0;
  virtual void Visit(const Print&) = 0;
  virtual void Visit(const Var&) = 0;

  ~IStmtVisitor() {}
};
  
class Stmt
{
public:
  virtual void Accept(IStmtVisitor& vis) const = 0;

  ~Stmt() {}
};

class Expression: public Stmt
{
public:
  Expression(Ptr<Expr> expr)
  : expr_(expr)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<Expr> expr_;
};

class Print: public Stmt
{
public:
  Print(Ptr<Expr> expr)
  : expr_(expr)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<Expr> expr_;
};

class Var: public Stmt
{
public:
  Var(Ptr<scanner::Token> name, Ptr<Expr> expr)
  : name_(name), expr_(expr)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<scanner::Token> name_;
  Ptr<Expr> expr_;
};

} // namespace stmt
} // namespace parser
