#define EXPAND_ARG_LIST_ITER(ArgType, ArgName) \
  ArgType ArgName,
#define EXPAND_ARG_LIST_LAST(ArgType, ArgName) \
  ArgType ArgName

#define EXPAND_INITIALIZER_LIST_ITER(ArgType, ArgName) \
  ArgName##_(ArgName),
#define EXPAND_INITIALIZER_LIST_LAST(ArgType, ArgName) \
  ArgName##_(ArgName)

#define EXPAND_MEMBER_LIST_ITER(ArgType, ArgName) \
  ArgType ArgName##_;
#define EXPAND_MEMBER_LIST_LAST(ArgType, ArgName) \
  ArgType ArgName##_

#define DECLARE_CLASS(Name, AllArgs) \
  class Name : public Expr \
  { \
  public: \
    Name(AllArgs(EXPAND_ARG_LIST_ITER, EXPAND_ARG_LIST_LAST)) \
      : AllArgs(EXPAND_INITIALIZER_LIST_ITER, EXPAND_INITIALIZER_LIST_LAST) \
    {} \
  private: \
    AllArgs(EXPAND_MEMBER_LIST_ITER, EXPAND_MEMBER_LIST_LAST) \
  };


#define FORALL_ARGS(Iter, Last) \
  Iter(Expr, left) \
  Iter(Token, op) \
  Last(Expr, right)
DECLARE_CLASS(Binary, FORALL_ARGS)

#define FORALL_ARGS(Iter, Last) \
  Last(Expr, expression) 
DECLARE_CLASS(Grouping, FORALL_ARGS)