#include "object.h"

namespace common
{

std::string ToStringImpl(const std::string& val)
{
  return val;
}

std::string ToStringImpl(const std::shared_ptr<ICallable>& val)
{
  return "<callable>";
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


} // namespace common
