#include "string_tools.h"

#include <cstring>

namespace ilang {

Position GetPosition(std::string_view view) {
  size_t line = 1;
  size_t line_start_idx = 0;

  for (size_t i = 0; i < view.size(); ++i) {
    if (view[i] == '\n') {
      line += 1;
      line_start_idx = i + 1;
    }
  }

  return {line, view.size() - line_start_idx + 1};
}

}  // namespace ilang