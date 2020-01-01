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
};

} // namespace common
