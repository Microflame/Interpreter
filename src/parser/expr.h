#pragma once

#include <memory>

#include "scanner/scanner.h"

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
  Binary(Expr& left, Token& op, Expr& right)
    : left_(&left), op_(&op), right_(&right)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Expr* left_;
  Token* op_;
  Expr* right_;
};

class Grouping: public Expr
{
public:
  Grouping(Expr& expr)
    : expr_(&expr)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Expr* expr_;
};

class Literal: public Expr
{
public:
  Literal(Token& val)
    : val_(&val)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Token* val_;
};

class Unary: public Expr
{
public:
  Unary(Token& op, Expr& right)
    : op_(&op), right_(&right)
  {}

  void Accept(IVisitor& visitor) const override { visitor.Visit(*this); }

  Token* op_;
  Expr* right_;
};