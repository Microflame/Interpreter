#pragma once

#include <stdexcept>

#include "types.h"
#include "scanner/token.h"

namespace ilang
{

// class ICallable;
// class IClass;
// class IInstance;

// template <typename T>
// decltype(std::to_string(T())) ToStringImpl(const T& val)
// {
//   return std::to_string(val);
// }

// std::string ToStringImpl(const std::string& val);

// std::string ToStringImpl(const std::shared_ptr<ICallable>& val);

// std::string ToStringImpl(const std::shared_ptr<IClass>& val);

// std::string ToStringImpl(const std::shared_ptr<IInstance>& val);

struct Object
{
  enum Type : int8_t
  {
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

  union
  {
    int64_t int_;
    double fp_;
    TokenStrId str_id_;
  };

  static const char* GetTypeName(Type type)
  {
    switch (type)
    {
      case INT: return "INT";
      case FLOAT: return "FLOAT";
      case STRING: return "STRING";
      case IDENTIFIER: return "IDENTIFIER";
      case BOOLEAN: return "BOOLEAN";
      case CALLABLE: return "CALLABLE";
      case CLASS: return "CLASS";
      case INSTANCE: return "INSTANCE";
      case NONE: return "NONE";
    }
    throw std::runtime_error("[Object::GetTypeName] Bad type");
  }

  void AsssertType(Type type) const
  {
    if (type_ != type)
    {
      std::string exp_type = GetTypeName(type);
      throw std::logic_error("Assumed type " + exp_type + " for object of type " + GetTypeName(type_));
    }
  }

  const char* GetTypeName() const
  {
    return GetTypeName(type_);
  }

  // std::string ToString() const
  // {
  //   switch (type_)
  //   {
  //     case INT: return std::to_string(int_);
  //     case FLOAT: return std::to_string(fp_);
  //     case STRING:
  //     case IDENTIFIER: return GlobalPool::token_spawner->GetString(str_id_);
  //     case BOOLEAN: return int_ ? "True" : "False";
  //     case CALLABLE: return "<CALLABLE>";
  //     case CLASS: return "<CLASS>";
  //     case INSTANCE: return "<INSTANCE>";
  //     case NONE: return "None";
  //   }
  //   throw std::runtime_error("[Object::ToString] Bad type");
  // }

  Type GetType() const
  {
    return type_;
  }

  // bool IsEqual(const Object& other)
  // {
  //   if (type_ == NONE && other.type_ == NONE)
  //   {
  //     return true;
  //   }
  //   if (type_ == NONE)
  //   {
  //     return false;
  //   }
  //   if (type_ != other.type_)
  //   {
  //     return false;
  //   }
  //   switch (type_)
  //   {
  //     case (INT):
  //       return int_ == other.int_;
  //     case (FLOAT):
  //       return fp_ == other.fp_;
  //     case (STRING):
  //     case (IDENTIFIER):
  //       return GlobalPool::token_spawner->GetString(str_id_) == GlobalPool::token_spawner->GetString(other.str_id_);
  //     default:
  //       throw std::logic_error("Objext::IsEqual() Bad type");
  //   }
  // }

  bool IsNumber() const
  {
    return (type_ == INT) || (type_ == FLOAT);
  }
};


Object MakeInt(int64_t val)
{
  return {.type_ = Object::INT, .int_=val};
}

Object MakeFloat(double val)
{
  return {.type_ = Object::FLOAT, .fp_=val};
}

Object MakeString(TokenStrId val)
{
  return {.type_ = Object::STRING, .str_id_=val};
}

Object MakeBool(bool val)
{
  return {.type_ = Object::BOOLEAN, .int_=val};
}

Object MakeNone()
{
  return {Object::NONE};
}

// Object MakeCallable(std::shared_ptr<common::ICallable> val);

// Object MakeClass(std::shared_ptr<common::IClass> val);

// Object MakeInstance(std::shared_ptr<common::IInstance> val);


} // namespace ilang
