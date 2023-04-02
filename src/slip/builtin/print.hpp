#pragma once

#include <iostream>

#include "slip/object.hpp"

namespace slip {

Object PrintBuiltin(std::span<Object> args, const Context& ctx) {
  const char* sep = "";
  for (const Object& obj : args) {
    std::cout << sep << obj.ToString(ctx);
    sep = " ";
  }
  std::cout << std::endl;
  return MakeNone();
}

}  // namespace slip
