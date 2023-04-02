#include "slip/object.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#include "slip/context.hpp"

namespace slip {

const char* Object::GetTypeName(Type type) {
  switch (type) {
    case INT:
      return "INT";
    case FLOAT:
      return "FLOAT";
    case STRING:
      return "STRING";
    case IDENTIFIER:
      return "IDENTIFIER";
    case BOOLEAN:
      return "BOOLEAN";
    case CALLABLE:
      return "CALLABLE";
    case CLASS:
      return "CLASS";
    case INSTANCE:
      return "INSTANCE";
    case BUILTIN_FUNCTION:
      return "BUILTIN_FUNCTION";
    case USER_FUNCTION:
      return "USER_FUNCTION";
    case NONE:
      return "NONE";
  }
  throw std::runtime_error("[Object::GetTypeName] Bad type");
}

void Object::AsssertType(Type type) const {
  if (type_ != type) {
    std::string exp_type = GetTypeName(type);
    throw std::logic_error("Assumed type " + exp_type + " for object of type " +
                           GetTypeName(type_));
  }
}

const char* Object::GetTypeName() const { return GetTypeName(type_); }

Object::Type Object::GetType() const { return type_; }

bool Object::IsNumber() const { return (type_ == INT) || (type_ == FLOAT); }

bool Object::AsBool() const {
  if (type_ == NONE) return false;
  if (type_ == INT) return int_;
  if (type_ == BOOLEAN) return int_;
  throw std::runtime_error("[Object::AsBool] Bad type");
}

double Object::AsFloat() const {
  if (type_ == Type::INT) return int_;
  return fp_;
}

std::string Object::ToString(const Context& ctx) const {
  switch (type_) {
    case INT:
      return std::to_string(int_);
    case FLOAT:
      return std::to_string(fp_);
    case STRING:
    case IDENTIFIER:
      return ctx.GetStr(str_id_);
    case BOOLEAN:
      return int_ ? "True" : "False";
    case NONE:
      return "None";
    case CALLABLE:
    case CLASS:
    case INSTANCE:
    case BUILTIN_FUNCTION:
    case USER_FUNCTION:
      return GetTypeName();
  }
  return "<INVALID_TYPE>";
}

Object Object::Mult(Object other) const {
  if (other.type_ == Type::FLOAT) return MultFp(other.fp_);
  if (other.type_ == Type::INT) return MultInt(other.int_);
  throw std::runtime_error("[Mult] bad right value type.");
}

Object Object::MultInt(int64_t other) const {
  if (type_ == Type::INT) return MakeInt(int_ * other);
  if (type_ == Type::FLOAT) return MakeFloat(fp_ * other);
  throw std::runtime_error("[MultInt] bad left value type.");
}

Object Object::MultFp(double other) const {
  if (type_ == Type::FLOAT) return MakeFloat(fp_ * other);
  if (type_ == Type::INT) return MakeFloat(int_ * other);
  throw std::runtime_error("[MultFp] bad left value type.");
}

Object Object::Div(Object other) const {
  if (other.type_ == Type::FLOAT) return DivFp(other.fp_);
  if (other.type_ == Type::INT) return DivInt(other.int_);
  throw std::runtime_error("[Div] bad right value type.");
}

Object Object::DivInt(int64_t other) const {
  if (type_ == Type::INT)
    return int_ % other ? MakeFloat((double)int_ / other)
                        : MakeInt(int_ / other);
  if (type_ == Type::FLOAT) return MakeFloat(fp_ / other);
  throw std::runtime_error("[DivInt] bad left value type.");
}

Object Object::DivFp(double other) const {
  if (type_ == Type::FLOAT) return MakeFloat(fp_ / other);
  if (type_ == Type::INT) return MakeFloat(int_ / other);
  throw std::runtime_error("[DivFp] bad left value type.");
}

Object Object::Add(Object other) const {
  if (other.type_ == Type::FLOAT) return AddFp(other.fp_);
  if (other.type_ == Type::INT) return AddInt(other.int_);
  throw std::runtime_error("[Add] bad right value type.");
}

Object Object::AddInt(int64_t other) const {
  if (type_ == Type::INT) return MakeInt(int_ + other);
  if (type_ == Type::FLOAT) return MakeFloat(fp_ + other);
  throw std::runtime_error("[AddInt] bad left value type.");
}

Object Object::AddFp(double other) const {
  if (type_ == Type::FLOAT) return MakeFloat(fp_ + other);
  if (type_ == Type::INT) return MakeFloat(int_ + other);
  throw std::runtime_error("[AddFp] bad left value type.");
}

Object Object::Sub(Object other) const {
  if (other.type_ == Type::FLOAT) return SubFp(other.fp_);
  if (other.type_ == Type::INT) return SubInt(other.int_);
  throw std::runtime_error("[Sub] bad right value type.");
}

Object Object::SubInt(int64_t other) const {
  if (type_ == Type::INT) return MakeInt(int_ - other);
  if (type_ == Type::FLOAT) return MakeFloat(fp_ - other);
  throw std::runtime_error("[SubInt] bad left value type.");
}

Object Object::SubFp(double other) const {
  if (type_ == Type::FLOAT) return MakeFloat(fp_ - other);
  if (type_ == Type::INT) return MakeFloat(int_ - other);
  throw std::runtime_error("[SubFp] bad left value type.");
}

template <typename T, typename U>
bool DoCompare(T lvalue, U rvalue, TokenType op) {
  switch (op) {
    case TokenType::EQUAL_EQUAL:
      return lvalue == rvalue;
    case TokenType::BANG_EQUAL:
      return lvalue != rvalue;
    case TokenType::LESS:
      return lvalue < rvalue;
    case TokenType::LESS_EQUAL:
      return lvalue <= rvalue;
    case TokenType::GREATER:
      return lvalue > rvalue;
    case TokenType::GREATER_EQUAL:
      return lvalue >= rvalue;
    default:
      throw std::runtime_error("[DoCompare] Bad op!");
  }
}

bool Object::Compare(Object other, TokenType op,
                     const Context& ctx) const {
  if (type_ == Type::STRING && other.type_ == Type::STRING) {
    const std::string& lvalue = ctx.GetStr(str_id_);
    const std::string& rvalue = ctx.GetStr(other.str_id_);
    return DoCompare(lvalue, rvalue, op);
  }
  if (IsNumber() && other.IsNumber()) {
    if (type_ == Type::FLOAT || other.type_ == Type::FLOAT) {
      return DoCompare(AsFloat(), other.AsFloat(), op);
    } else {
      return DoCompare(int_, other.int_, op);
    }
  }
  throw std::runtime_error("[IsEqual] invalid values.");
}

Object MakeInt(int64_t val) {
  return {.type_ = Object::INT, .int_ = val};
}

Object MakeFloat(double val) {
  return {.type_ = Object::FLOAT, .fp_ = val};
}

Object MakeString(StrId val) {
  return {.type_ = Object::STRING, .str_id_ = val};
}

Object MakeBool(bool val) {
  return {.type_ = Object::BOOLEAN, .int_ = val};
}

Object MakeNone() {
  return {Object::NONE};
}

Object MakeBuiltin(BuiltinFn fn) {
  return Object{.type_ = Object::BUILTIN_FUNCTION, .builtin_fn_ = fn};
}

Object MakeUserFn(VariableIdx frame_size, StrBlockId args, StmtBlockId stmts) {
  Object res = {.type_ = Object::USER_FUNCTION, .frame_size_ = frame_size};
  res.user_fn_.args_block_ = args;
  res.user_fn_.stmt_block_ = stmts;
  return res;
}

}  // namespace slip
