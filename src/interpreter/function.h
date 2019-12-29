#pragma once

#include "common/object.h"
#include "common/callable.h"
#include "parser/stmt.h"

namespace interpreter
{

class Interpreter;

class UserDefinedFunction: public common::ICallable
{
public:
  UserDefinedFunction() = delete;

  UserDefinedFunction(std::shared_ptr<parser::stmt::Func> func);

  common::Object Call(interpreter::Interpreter& interpreter, std::vector<common::Object>& args) const override;

  size_t GetArity() const override;

private:
  std::shared_ptr<parser::stmt::Func> func_;
};

} // namespace interpreter
