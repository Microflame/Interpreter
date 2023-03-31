#pragma once

namespace slip {

const char* GetBuildType() {
#ifdef NDEBUG
  return "Release";
#else
  return "Debug";
#endif
}

}  // namespace slip
