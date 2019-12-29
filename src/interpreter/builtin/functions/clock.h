#pragma once

#include <chrono>

#include "common/object.h"
#include "common/callable.h"

namespace interpreter
{
namespace builtin
{
namespace functions
{

class ClockBuiltin: public common::ICallable
{
public:
  using Clock = std::chrono::system_clock;
  using Units = std::chrono::milliseconds;

  common::Object Call(interpreter::Interpreter& interpreter, std::vector<common::Object>& args) const override
  {
    int64_t num_millis = std::chrono::duration_cast<Units>(Clock::now().time_since_epoch()).count();
    return common::MakeInt(num_millis);
  }

  size_t GetArity() const override
  {
    return 0;
  }
};

} // namespace functions
} // namespace builtin
} // namespace interpreter
