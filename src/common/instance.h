#pragma once 

#include <string>

#include "object.h"

namespace common
{

class IInstance
{
public:
  virtual ~IInstance() {};

  virtual std::string GetTypeName() const = 0;

  virtual Object& Get(const std::string&, bool) = 0;
};

} // namespace common
