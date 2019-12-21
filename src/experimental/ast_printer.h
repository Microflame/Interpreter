#pragma once

#include <string>
#include <sstream>

#include "parser/expr.h"
#include "util/visitor_getter.h"

class AstPrinter: public VisitorGetter<AstPrinter, Expr, std::string>, public IVisitor
{
public:
  void Visit(const Binary& expr) override
  {
    std::stringstream ss;
    ss << "(" << expr.op_->ToString() << " ";
    ss << GetValue(*expr.left_) << " " << GetValue(*expr.right_) << ")";
    Return(ss.str());
  }

  void Visit(const Grouping& expr) override
  {
    std::stringstream ss;
    ss << "(" << GetValue(*expr.expr_) << ")";
    Return(ss.str());
  }

  void Visit(const Literal& expr) override
  {
    Return(expr.val_->ToString());
  }

  void Visit(const Unary& expr) override
  {
    std::stringstream ss;
    ss << "(" << expr.op_->ToString() << " " << GetValue(*expr.right_) << ")";
    Return(ss.str());
  }
};