#pragma once

#include <utility>
#include <cstddef>
#include <string>

namespace util{
namespace string_tools
{

std::pair<size_t, size_t> GetPosition(const std::string& source, size_t idx)
{
  size_t line = 1;
  size_t column = idx + 1;
  size_t nl_idx = source.find('\n');
  while ((nl_idx != std::string::npos) && ((nl_idx + 1) < source.size()) && (nl_idx < idx))
  {
    ++line;
    column = idx - nl_idx;
    nl_idx = source.find('\n', nl_idx + 1);
  }

  return {line, column};
}

} // string_tools
} // util