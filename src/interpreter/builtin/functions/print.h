#pragma once

#include <iostream>

#include "common/object.h"
#include "common/callable.h"

namespace interpreter
{
namespace builtin
{
namespace functions
{

class PrintBuiltin: public common::ICallable
{
public:
  common::Object Call(interpreter::Interpreter& interpreter, std::vector<common::Object>& args) const override
  {
    std::cout << args[0].ToString() << "\n";
    return common::MakeNone();
  }

  std::string GetName() const override
  {
    return "PrintBuiltin";
  }

  size_t GetArity() const override
  {
    return 1;
  }
};

} // namespace functions
} // namespace builtin
} // namespace interpreter