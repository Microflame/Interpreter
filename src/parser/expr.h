#pragma once

#include <memory>

#include "scanner/scanner.h"

class IVisitor
{
public:
  virtual void Visit(const Binary&) const = 0;
  virtual void Visit(const Grouping&) const = 0;
  virtual void Visit(const Literal&) const = 0;
  virtual void Visit(const Unary&) const = 0;

  virtual ~IVisitor() {}
};

template <typename VisitorType, typename VisitableType>
class VisitorGetter
{
public:
  static VisitorType::ValueType GetValue(VisitableType& visitable)
  {
    VisitorType vis;
    visitable.Accept(vis);
    return vis.value;
  }

  void Return(const VisitorType::ValueType& val)
  {
    val_ = val
  }

private:
  VisitorType::ValueType val_;
};

class Expr
{
public:
  virtual void Accept(const IVisitor& visitor) const = 0;

  virtual ~Expr() {}
};


class Binary: public Expr
{
public:
  Binary(Expr& left, Token& op, Expr& right)
    : left_(&left), op_(&op), right_(&right)
  {}

  void Accept(const IVisitor & visitor) const override { visitor.Visit(*this); }

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

  void Accept(const IVisitor & visitor) const override { visitor.Visit(*this); }

  Expr* expr_;
};

class Literal: public Expr
{
public:
  Literal(Token& val)
    : val_(&val)
  {}

  void Accept(const IVisitor & visitor) const override { visitor.Visit(*this); }

  Token* val_;
};

class Unary: public Expr
{
public:
  Unary(Token& op, Expr& right)
    : op_(&op), right_(&right)
  {}

  void Accept(const IVisitor & visitor) const override { visitor.Visit(*this); }

  Token* op_;
  Expr* right_;
};