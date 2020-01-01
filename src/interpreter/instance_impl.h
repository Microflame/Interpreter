#pragma once

#include <unordered_map>
#include <memory>

#include "common/class.h"
#include "common/instance.h"

namespace interpreter
{
  
class InstanceImpl: public common::IInstance, public std::enable_shared_from_this<InstanceImpl>
{
public:
  InstanceImpl(std::shared_ptr<common::IClass> class_type)
    : class_type_(class_type)
  {}

  std::string GetTypeName() const override
  {
    return class_type_->GetName();
  }

  common::Object& Get(const std::string& name, bool create_if_not_exist) override
  {
    if (create_if_not_exist)
    {
      return properties_[name];
    }

    auto propit = properties_.find(name);
    if (propit != properties_.end())
    {
      return propit->second;
    }

    auto methit = methods_.find(name);
    if (methit != methods_.end())
    {
      return methit->second;
    }

    auto method = class_type_->FindMethod(name);
    if (method)
    {
      auto callable_ptr = method->AsCallable().Bind("this", common::MakeInstance(shared_from_this()));
      common::Object obj = common::MakeCallable(callable_ptr);
      return methods_[name] = obj;
    }
    
    throw std::runtime_error(GetTypeName() + " has no " + name + " property.");
  }

private:
  std::shared_ptr<common::IClass> class_type_;
  std::unordered_map<std::string, common::Object> properties_;
  std::unordered_map<std::string, common::Object> methods_;
};

} // namespace interpreter
