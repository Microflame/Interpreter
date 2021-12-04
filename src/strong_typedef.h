#pragma once

#ifdef NDEBUG

#define DEFINE_INDEX(name, type) using name = type

#else

#define DEFINE_INDEX(name, type)                    \
struct name                                         \
{                                                   \
  name() = default;                                 \
  name(type idx) : idx_(idx) {}                     \
  inline operator type() const { return idx_; }     \
  inline name& operator ++()                        \
  { ++idx_; return *this; }                         \
  inline name operator ++(int)                      \
  { ++idx_; return idx_ - 1; }                      \
  type idx_;                                        \
}

#endif
