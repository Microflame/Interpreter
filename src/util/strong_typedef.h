#pragma once

#ifdef NDEBUG

#define DEFINE_INDEX(name, type) using name = type

#else

#include <functional>

#define DEFINE_INDEX(name, type)                                     \
  struct name {                                                      \
    name() = default;                                                \
    name(type idx) : idx_(idx) {}                                    \
    inline operator type() const { return idx_; }                    \
    inline name& operator++() {                                      \
      ++idx_;                                                        \
      return *this;                                                  \
    }                                                                \
    inline name operator++(int) {                                    \
      ++idx_;                                                        \
      return idx_ - 1;                                               \
    }                                                                \
    type idx_;                                                       \
  };                                                                 \
                                                                     \
  } /* namespace ilang */                                            \
  namespace std {                                                    \
  template <>                                                        \
  struct hash<ilang::name> {                                         \
    size_t operator()(const ilang::name& x) const { return x.idx_; } \
  };                                                                 \
  } /* namespace std */                                              \
  namespace ilang {
#endif
