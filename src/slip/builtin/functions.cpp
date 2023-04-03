#include "slip/builtin/functions.hpp"

#include <iostream>
#include <chrono>

#include "slip/context.hpp"
#include "slip/object.hpp"

namespace slip
{

namespace builtin
{

static Object Print(std::span<Object> args, const Context& ctx) {
  const char* sep = "";
  for (const Object& obj : args) {
    std::cout << sep << obj.ToString(ctx);
    sep = " ";
  }
  std::cout << std::endl;
  return MakeNone();
}

static Object Time(std::span<Object> args, const Context& ctx) {
  auto now = std::chrono::high_resolution_clock::now();
  auto since_epoch = now.time_since_epoch();
  double micros = std::chrono::duration_cast<std::chrono::microseconds>(since_epoch).count();
  return MakeFloat(micros / 1'000'000);
}

} // namespace builtin

static void AddBuiltinFn(Context* context, slip::BuiltinFnPtr ptr, std::string name) {
  context->builtin_functions_.push_back({.ptr = ptr, .name = context->RegisterStr(std::move(name))});
}

void AddBuiltins(Context* context) {
  AddBuiltinFn(context, builtin::Print, "print");
  AddBuiltinFn(context, builtin::Time, "time");
}

} // namespace slip
