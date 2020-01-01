#pragma once

#include <unordered_map>

#include "common/class.h"
#include "common/instance.h"

namespace interpreter
{
  
class InstanceImpl: public common::IInstance
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

    auto it = properties_.find(name);
    if (it == properties_.end())
    {
      throw std::runtime_error(GetTypeName() + " has no " + name + " property.");
    }
    
    return it->second;
  }

private:
  std::shared_ptr<common::IClass> class_type_;
  std::unordered_map<std::string, common::Object> properties_;
};

} // namespace interpreter
