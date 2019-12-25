#pragma once

#include <unordered_map>
#include <string>

#include "common/object.h"
#include "interpret_error.h"

namespace interpreter
{
  
class Environment
{
public:
  void Define(const std::string& name, common::Object obj)
  {
    if (env_.find(name) != env_.end())
    {
      throw std::runtime_error(name + " is already defined.");
    }
    env_[name] = obj;
  }

  common::Object& Get(const std::string& name)
  {
    auto it = env_.find(name);
    if (it == env_.end())
    {
      throw std::runtime_error(name + " is not defined.");
    }
    return it->second;
  }

private:
  std::unordered_map<std::string, common::Object> env_;
};

} // namespace interpreter
