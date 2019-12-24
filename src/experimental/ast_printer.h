#pragma once

#include <string>
#include <sstream>

#include "parser/expr.h"
#include "util/visitor_getter.h"

class AstPrinter: public util::VisitorGetter<AstPrinter, parser::Expr, std::string>, public parser::IVisitor
{
public:
  void Visit(const parser::Binary& expr) override
  {
    std::stringstream ss;
    ss << "(" << expr.op_->ToRawString() << " ";
    ss << GetValue(*expr.left_) << " " << GetValue(*expr.right_) << ")";
    Return(ss.str());
  }

  void Visit(const parser::Grouping& expr) override
  {
    std::stringstream ss;
    ss << "(" << GetValue(*expr.expr_) << ")";
    Return(ss.str());
  }

  void Visit(const parser::Literal& expr) override
  {
    Return(expr.val_.ToString());
  }

  void Visit(const parser::Unary& expr) override
  {
    std::stringstream ss;
    ss << "(" << expr.op_->ToRawString() << " " << GetValue(*expr.right_) << ")";
    Return(ss.str());
  }
};