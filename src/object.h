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
    StrId str_id_;
  };

  static const char* GetTypeName(Type type);

  const char* GetTypeName() const;
  Type GetType() const;

  void AsssertType(Type type) const;
  bool IsNumber() const;
  bool AsBool() const;

  Object Mult(Object other) const;
  Object MultInt(int64_t other) const;
  Object MultFp(double other) const;
  Object Div(Object other) const;
  Object DivInt(int64_t other) const;
  Object DivFp(double other) const;
  Object Add(Object other) const;
  Object AddInt(int64_t other) const;
  Object AddFp(double other) const;
  Object Sub(Object other) const;
  Object SubInt(int64_t other) const;
  Object SubFp(double other) const;
};

Object MakeInt(int64_t val);
Object MakeFloat(double val);
Object MakeString(StrId val);
Object MakeBool(bool val);
Object MakeNone();

}  // namespace ilang
