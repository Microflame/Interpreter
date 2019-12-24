#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace common
{

class Object
{
public:
  enum Type
  {
    INT,
    FLOAT,
    STRING,
    IDENTIFIER,
    BOOLEAN,
    NONE
  };

  Object() : type_(NONE), held_(nullptr) {}

  template <typename T>
  Object(Type type, T val) : type_(type), held_(std::make_shared<Holder<T>>(val))
  {
    // TODO: Add static assert to ensure that `type` and `val` are a match
  }

  virtual ~Object() {}

  void AssumeType(Type type) const
  {
    if (type_ != type)
    {
      throw std::logic_error("Assumed type " + GetTypeName(type) + " for object of type " + GetTypeName(type_));
    }
  }

  int64_t& AsInt() const
  {
    AssumeType(INT);
    return held_->As<int64_t>();
  }

  double& AsFloat() const
  {
    AssumeType(FLOAT);
    return held_->As<double>();
  }

  std::string& AsString() const
  {
    AssumeType(STRING);
    return held_->As<std::string>();
  }

  bool& AsBool() const
  {
    AssumeType(BOOLEAN);
    return held_->As<bool>();
  }

  std::string GetTypeName()
  {
    return GetTypeName(type_);
  }

  static std::string GetTypeName(Type type)
  {
    switch (type)
    {
      case INT: return "INT";
      case FLOAT: return "FLOAT";
      case STRING: return "STRING";
      case IDENTIFIER: return "IDENTIFIER";
      case BOOLEAN: return "BOOLEAN";
      case NONE: return "NONE";
    }
    return "Bad type: " + std::to_string(type);
  }

  std::string ToString() const
  {
    return held_->ToString();
  }

  Type GetType() const
  {
    return type_;
  }

  bool IsEqual(const Object& other)
  {
    if (type_ == NONE && other.type_ == NONE)
    {
      return true;
    }
    if (type_ == NONE)
    {
      return false;
    }
    if (type_ != other.type_)
    {
      return false;
    }
    switch (type_)
    {
      case (INT):
        return AsInt() == other.AsInt();
      case (FLOAT):
        return AsFloat() == other.AsFloat();
      case (STRING):
      case (IDENTIFIER):
        return AsString() == other.AsString();
      default:
        throw std::logic_error("Objext::IsEqual() Bad type");
    }
    
  }

  bool IsNumber() const
  {
    return (type_ == INT) || (type_ == FLOAT);
  }

private:
  class AbstractHolder
  {
  public:
    virtual ~AbstractHolder() {}

    template <typename T>
    T& As()
    {
      Holder<T>* ptr = dynamic_cast<Holder<T>*>(this);
      if (ptr == nullptr)
      {
        throw std::logic_error("Bad object cast");
      }
      return ptr->GetValue();
    }

    virtual std::string ToString() const = 0;
  };

  template <typename T>
  class Holder: public AbstractHolder
  {
  public:
    Holder() = delete;
    Holder(T held) : held_(held)
    {}

    T& GetValue() { return held_; }

    std::string ToString() const override
    {
      return ToStringImpl();
    }

  private:
    T held_;


    //TODO: c'mon just use static polymorphism
    template <typename U = T>
    typename std::enable_if<!std::is_same<std::string, U>::value, std::string>::type
    ToStringImpl() const
    {
      return std::to_string(held_);
    }

    template <typename U = T>
    typename std::enable_if<std::is_same<std::string, U>::value, std::string>::type
    ToStringImpl() const
    {
      return held_;
    }
  };

  Type type_;
  std::shared_ptr<AbstractHolder> held_;
};


Object MakeInt(int64_t val)
{
  return Object(Object::INT, val);
}

Object MakeFloat(double val)
{
  return Object(Object::FLOAT, val);
}

Object MakeString(std::string val)
{
  return Object(Object::STRING, val);
}

Object MakeBool(bool val)
{
  return Object(Object::BOOLEAN, val);
}

Object MakeNone()
{
  return Object();
}


} // namespace common
