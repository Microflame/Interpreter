#pragma once

#include <memory>

#include "scanner/scanner.h"
#include "common/object.h"

namespace stmt
{

class Stmt;

} // namespace stmt


namespace parser
{
  
template <typename T>
using Ptr = std::shared_ptr<T>;

class Assign;
class Get;
class Set;
class Binary;
class Logical;
class Grouping;
class Literal;
class Unary;
class Variable;
class Call;

class IVisitor
{
public:
  virtual void Visit(const Assign&) = 0;
  virtual void Visit(const Get&) = 0;
  virtual void Visit(const Set&) = 0;
  virtual void Visit(const Binary&) = 0;
  virtual void Visit(const Logical&) = 0;
  virtual void Visit(const Grouping&) = 0;
  virtual void Visit(const Literal&) = 0;
  virtual void Visit(const Unary&) = 0;
  virtual void Visit(const Variable&) = 0;
  virtual void Visit(const Call&) = 0;

  virtual ~IVisitor() {}
};

class Expr
{
public:
  const size_t kId;

  Expr()
    : kId(-1)
  {}

  Expr(size_t id)
    : kId(id)
  {}

  virtual void Accept(IVisitor& visitor) const = 0;

  virtual ~Expr() {}
};


class Get: public Expr
{
public:
  Get(Ptr<Expr> object, Ptr<scanner::Token> name)
    : object_(object),
      name_(name)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<Expr> object_;
  Ptr<scanner::Token> name_;
};

class Set: public Expr
{
public:
  Set(Ptr<Expr> object, Ptr<scanner::Token> name, Ptr<Expr> value)
    : object_(object),
      name_(name),
      value_(value)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<Expr> object_;
  Ptr<scanner::Token> name_;
  Ptr<Expr> value_;
};

class Assign: public Expr
{
public:
  Assign(Ptr<scanner::Token> name, Ptr<Expr> value, size_t id)
    : Expr(id),
      name_(name),
      value_(value)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<Expr> left_;
  Ptr<scanner::Token> name_;
  Ptr<Expr> value_;
};

class Binary: public Expr
{
public:
  Binary(Ptr<Expr> left, Ptr<scanner::Token> op, Ptr<Expr> right)
    : left_(left),
      op_(op),
      right_(right)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<Expr> left_;
  Ptr<scanner::Token> op_;
  Ptr<Expr> right_;
};

class Logical: public Expr
{
public:
  Logical(Ptr<Expr> left, Ptr<scanner::Token> op, Ptr<Expr> right)
    : left_(left),
      op_(op),
      right_(right)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<Expr> left_;
  Ptr<scanner::Token> op_;
  Ptr<Expr> right_;
};

class Grouping: public Expr
{
public:
  Grouping(Ptr<Expr> expr)
    : expr_(expr)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<Expr> expr_;
};

class Literal: public Expr
{
public:
  Literal(Ptr<scanner::Token> tok)
    : val_(tok->GetObject())
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  common::Object val_;
};

class Unary: public Expr
{
public:
  Unary(Ptr<scanner::Token> op, Ptr<Expr> right)
    : op_(op),
      right_(right)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<scanner::Token> op_;
  Ptr<Expr> right_;
};

class Variable: public Expr
{
public:
  Variable(Ptr<scanner::Token> name, size_t id)
    : Expr(id),
      name_(name)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<scanner::Token> name_;
};

class Call: public Expr
{
public:
  Call(Ptr<Expr> callee, Ptr<scanner::Token> paren, Ptr<std::vector<Ptr<Expr>>> args)
    : callee_(callee),
      paren_(paren),
      args_(args)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<Expr> callee_;
  Ptr<scanner::Token> paren_;
  Ptr<std::vector<Ptr<Expr>>> args_;
};

} // parser