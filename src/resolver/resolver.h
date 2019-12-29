#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "util/visitor_getter.h"
#include "interpreter/interpreter.h"
#include "parser/expr.h"
#include "parser/stmt.h"

namespace resolver
{

class Resolver: public parser::IVisitor,
                public parser::stmt::IStmtVisitor
{
public:
  Resolver(interpreter::Interpreter& interpreter)
    : interpreter_(interpreter),
      scopes_(1)
  {
    scopes_.back()["print"] = true;
    scopes_.back()["clock"] = true;
    context_stack_.push_back(ContextType::GLOBAL);
  }

  void Resolve(const std::vector<std::shared_ptr<parser::stmt::Stmt>>& stmts)
  {
    for (const auto& s: stmts)
    {
      Resolve(*s);
    }
  }

private:
  enum class ContextType
  {
    GLOBAL,
    FUNCTION,
    METHOD
  };

  interpreter::Interpreter& interpreter_;
  std::vector<std::unordered_map<std::string, bool>> scopes_;
  std::vector<ContextType> context_stack_;

  void Visit(const parser::stmt::Return& stmt)
  {
    if (context_stack_.back() == ContextType::GLOBAL)
    {
      throw std::runtime_error("The \"return\" keyword is not allowed in the global context.");
    }
    if (stmt.value_)
    {
      Resolve(*stmt.value_);
    }
  }
  
  void Visit(const parser::stmt::Block& stmt)
  {
    BeginScope();
    Resolve(*stmt.statements_);
    EndScope();
  }
  
  void Visit(const parser::stmt::Func& stmt)
  {
    Declare(*stmt.name_);
    Define(*stmt.name_);

    ResolveFunction(stmt);
  }
  
  void Visit(const parser::stmt::If& stmt)
  {
    Resolve(*stmt.condition_);
    Resolve(*stmt.stmt_true_);
    if (stmt.stmt_false_)
    {
      Resolve(*stmt.stmt_false_);
    }
  }
  
  void Visit(const parser::stmt::Expression& stmt)
  {
    Resolve(*stmt.expr_);
  }
  
  void Visit(const parser::stmt::Print& stmt)
  {
    Resolve(*stmt.expr_);
  }
  
  void Visit(const parser::stmt::While& stmt)
  {
    Resolve(*stmt.condition_);
    Resolve(*stmt.body_);
  }
  
  void Visit(const parser::stmt::Var& stmt)
  {
    Declare(*stmt.name_);
    if (stmt.expr_)
    {
      Resolve(*stmt.expr_);
    }
    Define(*stmt.name_);
  }
  

  void Visit(const parser::Assign& expr)
  {
    Resolve(*expr.value_);
    ResolveLocal(expr, *expr.name_);
  }

  void Visit(const parser::Binary& expr)
  {
    Resolve(*expr.left_);
    Resolve(*expr.right_);
  }

  void Visit(const parser::Logical& expr)
  {
    Resolve(*expr.left_);
    Resolve(*expr.right_);
  }

  void Visit(const parser::Grouping& expr)
  {
    Resolve(*expr.expr_);
  }

  void Visit(const parser::Literal& expr)
  {

  }

  void Visit(const parser::Unary& expr)
  {
    Resolve(*expr.right_);
  }

  void Visit(const parser::Variable& expr)
  {
    auto it = scopes_.back().find(expr.name_->ToRawString());
    if (it != scopes_.back().end() && it->second == false)
    {
      throw std::runtime_error("Can not access uninitialized variable.");
    }
    ResolveLocal(expr, *expr.name_);
  }

  void Visit(const parser::Call& expr)
  {
    Resolve(*expr.callee_);
    for (const auto& arg: *expr.args_)
    {
      Resolve(*arg);
    }
  }


  void BeginScope()
  {
    scopes_.emplace_back();
  }

  void EndScope()
  {
    scopes_.pop_back();
  }

  void Resolve(const parser::stmt::Stmt& stmt)
  {
    stmt.Accept(*this);
  }

  void Resolve(const parser::Expr& expr)
  {
    expr.Accept(*this);
  }

  void Declare(const scanner::Token& name)
  {
    auto it = scopes_.back().find(name.ToRawString());
    if (it != scopes_.back().end())
    {
      throw std::runtime_error("Variable \"" + name.ToRawString() + "\" already defined in this scope.");
    }
    scopes_.back()[name.ToRawString()] = false;
  }

  void Define(const scanner::Token& name)
  {
    scopes_.back()[name.ToRawString()] = true;
  }

  void ResolveLocal(const parser::Expr& expr, const scanner::Token& name)
  {
    size_t depth = 0;
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
      if (it->find(name.ToRawString()) != it->end())
      {
        interpreter_.Resolve(expr, depth);
        return;
      }
      ++depth;
    }
  }

  void ResolveFunction(const parser::stmt::Func& func)
  {
    context_stack_.push_back(ContextType::FUNCTION);
    BeginScope();
    for (const auto& t: *func.params_)
    {
      Declare(*t);
      Define(*t);
    }
    Resolve(*func.body_);
    EndScope();
    context_stack_.pop_back();
  }
};

} // namespace resolver
