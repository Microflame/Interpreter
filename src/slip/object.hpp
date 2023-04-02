#pragma once

#include <span>
#include <string>

#include "slip/indexing.hpp"
#include "slip/token_type.hpp"

namespace slip {

struct Context;

struct Object;
using BuiltinFn = Object (*)(std::span<Object>, const Context&);

struct Object {
  struct UserFn {
    StrBlockId args_block_;
    StmtBlockId stmt_block_;
  };

  enum Type : int8_t {
    INT,
    FLOAT,
    STRING,
    IDENTIFIER,
    BOOLEAN,
    CALLABLE,
    CLASS,
    INSTANCE,
    BUILTIN_FUNCTION,
    USER_FUNCTION,
    NONE
  } type_;


  union {
    VariableIdx frame_size_;
  };
  
  union {
    int64_t int_;
    double fp_;
    StrId str_id_;
    BuiltinFn builtin_fn_;
    UserFn user_fn_;
  };

  static const char* GetTypeName(Type type);

  const char* GetTypeName() const;
  Type GetType() const;
  void AsssertType(Type type) const;
  bool IsNumber() const;
  bool AsBool() const;
  double AsFloat() const;
  std::string ToString(const Context& ctx) const;

  bool Compare(Object other, TokenType op, const Context& ctx) const;
};

Object Add(const Object& first, const Object& second, const Context& ctx);
Object Sub(const Object& first, const Object& second, const Context& ctx);
Object Mul(const Object& first, const Object& second, const Context& ctx);
Object Div(const Object& first, const Object& second, const Context& ctx);

Object MakeInt(int64_t val);
Object MakeFloat(double val);
Object MakeString(StrId val);
Object MakeBool(bool val);
Object MakeNone();
Object MakeBuiltin(BuiltinFn fn);
Object MakeUserFn(VariableIdx frame_size, StrBlockId args, StmtBlockId stmts);

}  // namespace slip
