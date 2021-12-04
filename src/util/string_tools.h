#pragma once

#include <string_view>

namespace ilang {

struct Position {
  size_t line;
  size_t column;
};

Position GetPosition(std::string_view view);

}  // namespace ilang