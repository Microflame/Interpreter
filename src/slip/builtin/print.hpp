#pragma once

#include <iostream>

#include "slip/object.hpp"

namespace slip {

Object PrintBuiltin(std::span<Object> args, const ExprStmtPool& pool) {
  const char* sep = "";
  for (const Object& obj : args) {
    std::cout << sep << obj.ToString(pool);
    sep = " ";
  }
  std::cout << std::endl;
  return MakeNone();
}

}  // namespace slip
