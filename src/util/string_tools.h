#pragma once

#include <string>

namespace util {

struct Position
{
  size_t line;
  size_t column;
};

namespace string_tools {

Position GetPosition(const char* s, size_t idx);

} // string_tools
} // util