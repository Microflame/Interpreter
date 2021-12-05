#pragma once

#include <iostream>

#include "object.h"

namespace ilang {

Object PrintBuiltin(const std::vector<Object>& args, const ExprStmtPool& pool) {
  const char* sep = "";
  for (const Object& obj : args) {
    std::cout << sep << obj.ToString(pool);
    sep = " ";
  }
  std::cout << std::endl;
  return MakeNone();
}

}  // namespace ilang
