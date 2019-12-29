#pragma once

#include <utility>
#include <cstddef>
#include <string>

namespace util {
namespace string_tools {

std::pair<size_t, size_t> GetPosition(const std::string& source, size_t idx);

} // string_tools
} // util