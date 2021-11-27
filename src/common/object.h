#pragma once

#include <memory>
#include <stdexcept>

namespace common
{

class ICallable;
class IClass;
class IInstance;

template <typename T>
decltype(std::to_string(T())) ToStringImpl(const T& val)
{
  return std::to_string(val);
}

std::string ToStringImpl(const std::string& val);

std::string ToStringImpl(const std::shared_ptr<ICallable>& val);

std::string ToStringImpl(const std::shared_ptr<IClass>& val);

std::string ToStringImpl(const std::shared_ptr<IInstance>& val);

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
    CALLABLE,
    CLASS,
    INSTANCE,
    NONE
  };

  Object() : type_(NONE), held_(nullptr) {}

  template <typename T>
  Object(Type type, T val) : type_(type), held_(std::make_shared<Holder<T>>(val))
  {}

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

  ICallable& AsCallable() const;

  IClass& AsClass() const
  {
    AssumeType(CLASS);
    return *held_->As<std::shared_ptr<IClass>>();
  }

  IInstance& AsInstance() const
  {
    AssumeType(INSTANCE);
    return *held_->As<std::shared_ptr<IInstance>>();
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
      case CALLABLE: return "CALLABLE";
      case CLASS: return "CLASS";
      case INSTANCE: return "INSTANCE";
      case NONE: return "NONE";
    }
    return "Bad type: " + std::to_string(type);
  }

  std::string ToString() const
  {
    if (type_ == NONE)
    {
      return "None";
    }
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
      return ToStringImpl(held_);
    }

  private:
    T held_;
  };

  Type type_;
  std::shared_ptr<AbstractHolder> held_;
};


Object MakeInt(int64_t val);

Object MakeFloat(double val);

Object MakeString(const std::string& val);

Object MakeBool(bool val);

Object MakeNone();

Object MakeCallable(std::shared_ptr<common::ICallable> val);

Object MakeClass(std::shared_ptr<common::IClass> val);

Object MakeInstance(std::shared_ptr<common::IInstance> val);


} // namespace common
