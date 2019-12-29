#pragma once

#include <memory>
#include <vector>

#include "object.h"

namespace interpreter
{
class Interpreter;
} // namespace interpreter

namespace common
{

template <typename T>
using Ptr = std::shared_ptr<T>;

class ICallable
{
public:
  virtual ~ICallable() {}

  virtual Object Call(interpreter::Interpreter& interpreter, std::vector<Object>& args) const
  {
    throw std::logic_error("Call() not implemented.");
  }

  virtual size_t GetArity() const
  {
    throw std::logic_error("GetArity() not implemented.");
  }
};

template <typename T, size_t A>
class BuiltinCallable: public ICallable
{
public:
  Object Call(interpreter::Interpreter& interpreter, std::vector<Object>& args) const override
  {
    return T(interpreter, args);
  }

  size_t GetArity() const override
  {
    return A;
  }
};

} // namespace common
