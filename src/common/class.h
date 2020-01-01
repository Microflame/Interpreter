#pragma once

#include <string>

#include "callable.h"

namespace common
{

class ICallable;

class IClass: public ICallable
{
public:
  virtual ~IClass() {}

  virtual std::shared_ptr<common::Object> FindMethod(const std::string&) const = 0;
  
};

} // namespace common
