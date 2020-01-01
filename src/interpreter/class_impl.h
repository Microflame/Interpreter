#pragma once

#include <string>
#include <vector>

#include "common/object.h"
#include "common/class.h"
#include "instance_impl.h"

namespace interpreter
{

class Interpreter;

class ClassImpl: public common::IClass
{
public:
  ClassImpl(const std::string& name)
    : kName(name)
  {}

  std::string GetName() const override
  {
    return kName;
  }

  common::Object Call(interpreter::Interpreter& interpreter, std::vector<common::Object>& args) const override
  {
    auto ptr = std::make_shared<InstanceImpl>(self_.lock());
    common::Object obj = common::MakeInstance(ptr);
    return obj;
  }

  size_t GetArity() const override
  {
    return 0;
  }

  void SetSelf(std::weak_ptr<ClassImpl> self)
  {
    self_ = self;
  }

private:
  const std::string kName;
  std::weak_ptr<ClassImpl> self_;
};

} // namespace interpreter
