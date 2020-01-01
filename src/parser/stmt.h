#pragma once

#include <vector>

#include "expr.h"

namespace parser
{
namespace stmt
{

template <typename T>
using Ptr = std::shared_ptr<T>;

class Return;
class Block;
class Func;
class Class;
class If;
class Expression;
class Print;
class While;
class Var;

class IStmtVisitor
{
public:
  virtual void Visit(const Return&) = 0;
  virtual void Visit(const Block&) = 0;
  virtual void Visit(const Func&) = 0;
  virtual void Visit(const Class&) = 0;
  virtual void Visit(const If&) = 0;
  virtual void Visit(const Expression&) = 0;
  virtual void Visit(const Print&) = 0;
  virtual void Visit(const While&) = 0;
  virtual void Visit(const Var&) = 0;

  ~IStmtVisitor() {}
};
  
class Stmt
{
public:
  virtual void Accept(IStmtVisitor& vis) const = 0;

  ~Stmt() {}
};

class Return: public Stmt
{
public:
  Return(Ptr<scanner::Token> tok, Ptr<Expr> value)
    : tok_(tok),
      value_(value)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<scanner::Token> tok_;
  Ptr<Expr> value_;
};

class Func: public Stmt
{
public:
  Func(Ptr<scanner::Token> name,
       Ptr<std::vector<Ptr<scanner::Token>>> params,
       Ptr<std::vector<Ptr<Stmt>>> body)
  : name_(name),
    params_(params),
    body_(body)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<scanner::Token> name_;
  Ptr<std::vector<Ptr<scanner::Token>>> params_;
  Ptr<std::vector<Ptr<Stmt>>> body_;
};

class Class: public Stmt
{
public:
  Class(Ptr<scanner::Token> name,
        Ptr<std::vector<Ptr<Func>>> methods)
  : name_(name),
    methods_(methods)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<scanner::Token> name_;
  Ptr<std::vector<Ptr<Func>>> methods_;
};

class If: public Stmt
{
public:
  If(Ptr<Expr> condition, Ptr<Stmt> stmt_true, Ptr<Stmt> stmt_false)
  : condition_(condition),
    stmt_true_(stmt_true),
    stmt_false_(stmt_false)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<Expr> condition_;
  Ptr<Stmt> stmt_true_;
  Ptr<Stmt> stmt_false_;
};

class Block: public Stmt
{
public:
  Block(Ptr<std::vector<Ptr<Stmt>>> statements)
  : statements_(statements)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<std::vector<Ptr<Stmt>>> statements_;
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

class While: public Stmt
{
public:
  While(Ptr<Expr> condition, Ptr<stmt::Stmt> body)
  : condition_(condition),
    body_(body)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<Expr> condition_;
  Ptr<stmt::Stmt> body_;
};

class Var: public Stmt
{
public:
  Var(Ptr<scanner::Token> name, Ptr<Expr> expr)
  : name_(name),
    expr_(expr)
  {}

  void Accept(IStmtVisitor& vis) const { vis.Visit(*this); }

  Ptr<scanner::Token> name_;
  Ptr<Expr> expr_;
};

} // namespace stmt
} // namespace parser
