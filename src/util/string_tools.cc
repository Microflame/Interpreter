#include "string_tools.h"

#include <cstring>

namespace util {
namespace string_tools {

Position GetPosition(const char* s, size_t idx)
{
  size_t line = 1;
  size_t line_start_idx = 0;

  for (size_t i = 0; i < idx; ++i)
  {
    if (s[i] == '\n')
    {
      line += 1;
      line_start_idx = i + 1;
    }
  }

  return {line, idx - line_start_idx + 1};
}

} // string_tools
} // util