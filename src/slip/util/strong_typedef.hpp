#pragma once

#ifdef NDEBUG

#define SLIP_DEFINE_INDEX(name, type) using name = type

#else

#include <functional>

#define SLIP_DEFINE_INDEX(name, type)                                 \
  struct name {                                                       \
    name() = default;                                                 \
    name(type idx) : idx_(idx) {}                                     \
    inline operator type() const { return idx_; }                     \
    inline name& operator++() {                                       \
      ++idx_;                                                         \
      return *this;                                                   \
    }                                                                 \
    inline name operator++(int) {                                     \
      ++idx_;                                                         \
      return idx_ - 1;                                                \
    }                                                                 \
    type idx_;                                                        \
  };                                                                  \
                                                                      \
  } /* namespace slip */                                              \
  namespace std {                                                     \
  template <>                                                         \
  struct hash<slip::name> {                                           \
    size_t operator()(const slip::name& x) const { return x.idx_; }   \
  };                                                                  \
  } /* namespace std */                                               \
  namespace slip {
#endif
