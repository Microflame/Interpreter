#pragma once

#include <type_traits>

namespace util
{

template <typename VisitorType, typename VisitableType, typename ReturnType>
class VisitorGetter
{
public:
  ~VisitorGetter()
  {
    static_assert(std::is_base_of<VisitorGetter, VisitorType>::value,
                  "VisitorGetter should be the base for VisitableType");
  }

  ReturnType GetValue(const VisitableType& visitable)
  {
    visitable.Accept(*static_cast<VisitorType*>(this));
    return val_;
  }

  void Return(const ReturnType& val)
  {
    val_ = val;
  }

private:
  ReturnType val_;
};

} // util