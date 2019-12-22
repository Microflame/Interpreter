#pragma once

namespace util
{

template <typename VisitorType, typename VisitableType, typename ReturnType>
class VisitorGetter
{
public:
  static ReturnType GetValue(VisitableType& visitable)
  {
    VisitorType vis;
    visitable.Accept(vis);
    return vis.val_;
  }

  void Return(const ReturnType& val)
  {
    val_ = val;
  }

private:
  ReturnType val_;
};

} // util