#pragma once

namespace ilang
{

const char* GetBuildType()
{
#ifdef NDEBUG
  return "Release";
#else
  return "Debug";
#endif
}

} // namespace ilang
