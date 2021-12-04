#pragma once

#include "types.h"

namespace ilang {

struct Object {
  enum Type : int8_t {
    INT,
    FLOAT,
    STRING,
    IDENTIFIER,
    BOOLEAN,
    CALLABLE,
    CLASS,
    INSTANCE,
    NONE
  } type_;

  union {
    int64_t int_;
    double fp_;
    TokenStrId str_id_;
  };

  static const char* GetTypeName(Type type);

  const char* GetTypeName() const;
  Type GetType() const;

  void AsssertType(Type type) const;
  bool IsNumber() const;
};

Object MakeInt(int64_t val);
Object MakeFloat(double val);
Object MakeString(TokenStrId val);
Object MakeBool(bool val);
Object MakeNone();

}  // namespace ilang
