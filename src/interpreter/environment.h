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

  common::Object& Get(const std::string& name)
  {
    auto it = env_.find(name);
    if (it != env_.end())
    {
      return it->second;
    }
    if (parent_env_)
    {
      return parent_env_->Get(name);
    }
    throw std::runtime_error(name + " is not defined.");
  }

  void Assign(const std::string& name, common::Object obj)
  {
    auto it = env_.find(name);
    if (it != env_.end())
    {
      it->second = obj;
      return;
    }
    if (parent_env_)
    {
      parent_env_->Assign(name, obj);
      return;
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
  class EnvironmentGuard
  {
  public:
    EnvironmentGuard() = delete;

    EnvironmentGuard(EnvironmentStack& stack)
      : stack_(stack)
    {
      stack_.PushEnvironment();
    }

    ~EnvironmentGuard()
    {
      stack_.PopEnvironment();
    }

  private:
    EnvironmentStack& stack_;
  };

  EnvironmentStack()
    : top_(std::make_shared<Environment>())
  {}

  Environment& GetCurrent()
  {
    return *top_;
  }

  void PushEnvironment()
  {
    top_ = std::make_shared<Environment>(top_);
  }

  void PopEnvironment()
  {
    top_ = top_->GetParentEnvironment();
  }

  std::unique_ptr<EnvironmentGuard> GetGuard()
  {
    return std::make_unique<EnvironmentGuard>(*this);
  }

private:
  std::shared_ptr<Environment> top_;
};

} // namespace interpreter
