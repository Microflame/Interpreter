#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include "common/object.h"
#include "interpret_error.h"

namespace interpreter
{
  
class Environment
{
public:
  Environment()
    : parent_env_(nullptr)
  {}

  Environment(std::shared_ptr<Environment> parent_env)
    : parent_env_(parent_env)
  {}

  ~Environment() {}

  void Define(const std::string& name, common::Object obj)
  {
    if (env_.find(name) != env_.end())
    {
      throw std::runtime_error(name + " is already defined.");
    }
    env_[name] = obj;
  }

  common::Object& GetAt(const std::string& name, size_t depth)
  {
    if (depth)
    {
      return GetParentEnvironment()->GetAt(name, depth - 1);
    }
    auto it = env_.find(name);
    if (it != env_.end())
    {
      return it->second;
    }
    throw std::runtime_error(name + " is not defined.");
  }

  std::shared_ptr<Environment> GetParentEnvironment()
  {
    return parent_env_;
  }

private:
  std::unordered_map<std::string, common::Object> env_;
  std::shared_ptr<Environment> parent_env_;
};

class EnvironmentStack
{
public:
  class Guard
  {
  public:
    Guard() = delete;

    Guard(EnvironmentStack& stack, std::shared_ptr<Environment> other)
      : stack_(stack),
        old_(stack.GetCurrent())
    {
      stack_.SetCurrent(other);
    }

    Guard(EnvironmentStack& stack)
      : Guard(stack, std::make_shared<Environment>(stack.GetCurrent()))
    {}

    ~Guard()
    {
      stack_.SetCurrent(old_);
    }

  private:
    EnvironmentStack& stack_;
    std::shared_ptr<Environment> old_;
  };

  EnvironmentStack()
    : top_(std::make_shared<Environment>())
  {}

  std::shared_ptr<Environment> GetCurrent()
  {
    return top_;
  }

  void SetCurrent(std::shared_ptr<Environment> env)
  {
    top_ = env;
  }

  std::unique_ptr<Guard> GetGuard()
  {
    return std::make_unique<Guard>(*this);
  }

  std::unique_ptr<Guard> GetGuard(std::shared_ptr<Environment> other)
  {
    return std::make_unique<Guard>(*this, other);
  }

  std::shared_ptr<Environment> GetRoot()
  {
    std::shared_ptr<Environment> root = top_;
    while (root->GetParentEnvironment())
    {
      root = root->GetParentEnvironment();
    }
    return root;
  }

private:
  std::shared_ptr<Environment> top_;
};

} // namespace interpreter
