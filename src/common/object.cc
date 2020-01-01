#include "object.h"

#include "callable.h"
#include "class.h"
#include "instance.h"

namespace common
{

std::string ToStringImpl(const std::string& val)
{
  return val;
}

std::string ToStringImpl(const std::shared_ptr<ICallable>& val)
{
  return "<callable: " + val->GetName() + ">";
}

std::string ToStringImpl(const std::shared_ptr<IClass>& val)
{
  return "<class: " + val->GetName() + ">";
}

std::string ToStringImpl(const std::shared_ptr<IInstance>& val)
{
  return "<instance of: " + val->GetTypeName() + ">";
}

ICallable& Object::AsCallable() const
  {
    if (type_ == CLASS)
    {
      return *held_->As<std::shared_ptr<IClass>>();
    }
    AssumeType(CALLABLE);
    return *held_->As<std::shared_ptr<ICallable>>();
  }

Object MakeInt(int64_t val)
{
  return Object(Object::INT, val);
}

Object MakeFloat(double val)
{
  return Object(Object::FLOAT, val);
}

Object MakeString(const std::string& val)
{
  return Object(Object::STRING, val);
}

Object MakeBool(bool val)
{
  return Object(Object::BOOLEAN, val);
}

Object MakeNone()
{
  return Object();
}

Object MakeCallable(std::shared_ptr<common::ICallable> val)
{
  return Object(Object::CALLABLE, val);
}

Object MakeClass(std::shared_ptr<common::IClass> val)
{
  return Object(Object::CLASS, val);
}

Object MakeInstance(std::shared_ptr<common::IInstance> val)
{
  return Object(Object::INSTANCE, val);
}


} // namespace common
