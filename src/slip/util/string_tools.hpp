#pragma once

#include <string_view>

namespace slip {

struct Position {
  size_t line;
  size_t column;
};

Position GetPosition(std::string_view view);

}  // namespace slip