#pragma once

#include <memory>

#include "scanner/scanner.h"

namespace parser
{

template <typename T>
using Ptr = std::shared_ptr<T>;

class Binary;
class Grouping;
class Literal;
class Unary;

class IVisitor
{
public:
  virtual void Visit(const Binary&) = 0;
  virtual void Visit(const Grouping&) = 0;
  virtual void Visit(const Literal&) = 0;
  virtual void Visit(const Unary&) = 0;

  virtual ~IVisitor() {}
};

class Expr
{
public:
  virtual void Accept(IVisitor& visitor) const = 0;

  virtual ~Expr() {}
};


class Binary: public Expr
{
public:
  Binary(Ptr<Expr> left, Ptr<scanner::Token> op, Ptr<Expr> right)
    : left_(left), op_(op), right_(right)
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
  Literal(Ptr<scanner::Token> val)
    : val_(val)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<scanner::Token> val_;
};

class Unary: public Expr
{
public:
  Unary(Ptr<scanner::Token> op, Ptr<Expr> right)
    : op_(op), right_(right)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Ptr<scanner::Token> op_;
  Ptr<Expr> right_;
};

} // parser