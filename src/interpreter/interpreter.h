#pragma once

#include "scanner/token.h"
#include "parser/expr.h"
#include "common/object.h"
#include "util/visitor_getter.h"
#include "interpret_error.h"
#include "environment.h"

namespace interpreter
{

class Interpreter: public util::VisitorGetter<Interpreter, parser::Expr, common::Object>,
                   public parser::IVisitor,
                   public parser::stmt::IStmtVisitor
{
public:
  void Interpret(const std::vector<std::shared_ptr<parser::stmt::Stmt>>& statements)
  {
    try
    {
      for (const auto& stmt_ptr: statements)
      {
        Execute(*stmt_ptr);
      }
    }
    catch (const InterpretError& e)
    {
      std::cerr << e.what() << '\n';
    }
    
  }

  void Execute(const parser::stmt::Stmt& stmt)
  {
    stmt.Accept(*this);
  }

  void Visit(const parser::stmt::If& stmt)
  {
    common::Object obj = Evaluate(*stmt.condition_);
    if (IsTruthy(obj))
    {
      Execute(*stmt.stmt_true_);
    }
    else if (stmt.stmt_false_)
    {
      Execute(*stmt.stmt_false_);
    }
  }

  void Visit(const parser::stmt::Block& stmt)
  {
    ExecuteBlock(stmt);
  }

  void Visit(const parser::stmt::Expression& stmt)
  {
    Evaluate(*stmt.expr_);
  }

  void Visit(const parser::stmt::Print& stmt)
  {
    common::Object obj = Evaluate(*stmt.expr_);
    std::cout << obj.ToString() << "\n";
  }

  void Visit(const parser::stmt::While& stmt)
  {
    while (IsTruthy(Evaluate(*stmt.condition_)))
    {
      Execute(*stmt.body_);
    }
  }

  void Visit(const parser::stmt::Var& stmt)
  {
    common::Object init;
    if (stmt.expr_)
    {
      init = Evaluate(*stmt.expr_);
    }

    GetCurrentEnv().Define(stmt.name_->ToRawString(), init);
  }

  void Visit(const parser::Assign& expr) override
  {
    common::Object obj = Evaluate(*expr.value_);
    GetCurrentEnv().Assign(expr.name_->ToRawString(), obj);
    Return(obj);
  }

  void Visit(const parser::Literal& expr) override
  {
    common::Object obj = expr.val_;
    Return(obj);
  }

  void Visit(const parser::Grouping& expr) override
  {
    Return(Evaluate(*expr.expr_));
  }

  void Visit(const parser::Unary& expr) override
  {
    common::Object obj = Evaluate(*expr.right_);

    switch (expr.op_->GetType())
    {
      case (scanner::Token::MINUS):
      {
        switch (obj.GetType())
        {
          case common::Object::INT:
            Return(common::MakeInt(-obj.AsInt()));
            return;
          case common::Object::FLOAT:
            Return(common::MakeFloat(-obj.AsFloat()));
            return;
          default:
            throw InterpretError(*expr.op_, "Int or Float expected before " + expr.op_->ToString());
        }
      }
      case (scanner::Token::BANG):
      {
        Return(common::MakeBool(!IsTruthy(obj)));
        return;
      }
      default:
        throw std::logic_error("Bad unary type.");
    }
  }

  void Visit(const parser::Logical& expr) override
  {
    common::Object left = Evaluate(*expr.left_);

    if (expr.op_->GetType() == scanner::Token::OR && IsTruthy(left))
    {
      Return(left);
    }
    else if (!IsTruthy(left))
    {
      Return(left);
    }
    else
    {
      Return(Evaluate(*expr.right_));
    }
  }

  void Visit(const parser::Binary& expr) override
  {
    common::Object left = Evaluate(*expr.left_);
    common::Object right = Evaluate(*expr.right_);
    scanner::Token::Type op_type = expr.op_->GetType();

    if (expr.op_->GetType() == scanner::Token::EQUAL_EQUAL)
    {
      Return(common::MakeBool(left.IsEqual(right)));
      return;
    }
    if (expr.op_->GetType() == scanner::Token::BANG_EQUAL)
    {
      Return(common::MakeBool(!left.IsEqual(right)));
      return;
    }

    {
      bool left_str = left.GetType() == common::Object::STRING;
      bool right_str = right.GetType() == common::Object::STRING;
      if (left_str || right_str)
      {
        if ((left_str && right_str) && (op_type == scanner::Token::PLUS))
        {
          Return(common::MakeString(left.AsString() + right.AsString()));
          return;
        }
        throw InterpretError(*expr.op_, left.GetTypeName() + " and " + right.GetTypeName() + " are not valid for +.");
      }
    }

    std::vector<scanner::Token::Type> arithmetic_op_types = {scanner::Token::MINUS,
                                                             scanner::Token::PLUS,
                                                             scanner::Token::STAR,
                                                             scanner::Token::SLASH};

    std::vector<scanner::Token::Type> comparison_op_types = {scanner::Token::GREATER,
                                                             scanner::Token::GREATER_EQUAL,
                                                             scanner::Token::LESS,
                                                             scanner::Token::LESS_EQUAL};

    std::vector<scanner::Token::Type> equality_op_types = {scanner::Token::EQUAL_EQUAL,
                                                           scanner::Token::BANG_EQUAL};

    if (!expr.op_->OneOf(arithmetic_op_types) &&
        !expr.op_->OneOf(comparison_op_types))
    {
      throw InterpretError(*expr.op_, "Expected arithmetic or comparison operator.");
    }

    if (!left.IsNumber() || !right.IsNumber())
    {
      throw InterpretError(*expr.op_, left.GetTypeName() + " and " +
                           right.GetTypeName() + " are not valid for " +
                           expr.op_->ToRawString());
    }

    if (left.GetType() == common::Object::FLOAT || right.GetType() == common::Object::FLOAT)
    {
      double l = AsNumber<double>(left);
      double r = AsNumber<double>(right);

      if (expr.op_->OneOf(arithmetic_op_types))
      {
        Return(common::MakeFloat(DispatchBinary(l, op_type, r)));
      }
      else
      {
        Return(common::MakeBool(DispatchBinary<double, bool>(l, op_type, r)));
      }
      return;
    }
    else
    {
      int64_t l = AsNumber<int64_t>(left);
      int64_t r = AsNumber<int64_t>(right);

      if (expr.op_->OneOf(arithmetic_op_types))
      {
        Return(common::MakeInt(DispatchBinary(l, op_type, r)));
      }
      else
      {
        Return(common::MakeBool(DispatchBinary<int64_t, bool>(l, op_type, r)));
      }
      return;
    }
  }

  void Visit(const parser::Variable& expr) override
  {
    Return(GetCurrentEnv().Get(expr.name_->ToRawString()));
  }


private:
  EnvironmentStack environment_stack;

  Environment& GetCurrentEnv()
  {
    return environment_stack.GetCurrent();
  }

  std::unique_ptr<EnvironmentStack::EnvironmentGuard> GetEnvGuard()
  {
    return environment_stack.GetGuard();
  }

  void ExecuteBlock(const parser::stmt::Block& stmt)
  {
    auto g = GetEnvGuard();

    for (const auto& s: *stmt.statements_)
    {
      Execute(*s);
    }
  }

  common::Object Evaluate(const parser::Expr& expr)
  {
    return GetValue(expr);
  }

  bool IsTruthy(const common::Object& obj)
  {
    switch (obj.GetType())
    {
      case (common::Object::NONE):
        return false;
      case (common::Object::BOOLEAN):
        return obj.AsBool();
      default:
        return true;
    }
  }

  template <typename T, typename U = T>
  T DispatchBinary(T l, scanner::Token::Type op_type, T r)
  {
    switch (op_type)
    {
      // comparison
      case (scanner::Token::GREATER_EQUAL):
      {
        return l >= r;
      }
      case (scanner::Token::GREATER):
      {
        return l > r;
      }
      case (scanner::Token::LESS_EQUAL):
      {
        return l <= r;
      }
      case (scanner::Token::LESS):
      {
        return l < r;
      }
      // Arithmetics
      case (scanner::Token::MINUS):
      {
        return l - r;
      }
      case (scanner::Token::SLASH):
      {
        return l / r;
      }
      case (scanner::Token::STAR):
      {
        return l * r;
      }
      case (scanner::Token::PLUS):
      {
        return l + r;
      }
      default:
        throw std::logic_error("DispatchBinary called with bad op_type");
    }
  }

  template <typename T>
  T AsNumber(const common::Object& obj)
  {
    if (obj.GetType() == common::Object::INT)
    {
      return obj.AsInt();
    }
    if (obj.GetType() == common::Object::FLOAT)
    {
      return obj.AsFloat();
    }
    throw std::logic_error("Not a number");
  }
};

} // interpreter