#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "common/object.h"
#include "common/class.h"
#include "instance_impl.h"

namespace interpreter
{

class Interpreter;

class ClassImpl: public common::IClass
{
public:
  using Methods = std::unordered_map<std::string, std::shared_ptr<common::Object>>;

  ClassImpl(const std::string& name, common::Object super, Methods& methods)
    : kName(name),
      methods_(std::move(methods)),
      super_(super)
  {}

  std::shared_ptr<common::Object> FindMethod(const std::string& name) const override
  {
    auto it = methods_.find(name);
    if (it != methods_.end())
    {
      return it->second;
    }
    if (super_.GetType() == common::Object::CLASS)
    {
      return super_.AsClass().FindMethod(name);
    }
    return nullptr;
  }

  std::string GetName() const override
  {
    return kName;
  }

  common::Object Call(interpreter::Interpreter& interpreter, std::vector<common::Object>& args) const override
  {
    auto ptr = std::make_shared<InstanceImpl>(self_.lock());
    common::Object obj = common::MakeInstance(ptr);
    auto init = FindMethod("__init");
    if (init)
    {
      init->AsCallable().Bind("this", obj)->Call(interpreter, args);
    }
    return obj;
  }

  size_t GetArity() const override
  {
    auto init = FindMethod("__init");
    if (init)
    {
      return init->AsCallable().GetArity();
    }
    return 0;
  }

  void SetSelf(std::weak_ptr<ClassImpl> self)
  {
    self_ = self;
  }

private:
  const std::string kName;
  Methods methods_;
  std::weak_ptr<ClassImpl> self_;
  common::Object super_;
};

} // namespace interpreter
