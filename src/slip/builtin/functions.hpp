#pragma once

#include <span>

#include "slip/indexing.hpp"

namespace slip
{

class Context;
class Object;

struct Object;
using BuiltinFnPtr = Object (*)(std::span<Object>, const Context&);

struct BuiltinFunction {
  BuiltinFnPtr ptr;
  StrId name;
};

void AddBuiltins(Context* context);

} // namespace slip
