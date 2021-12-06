#include "object.h"

#include <iostream>
#include <stdexcept>

#include "expr_stmt_pool.h"

namespace ilang {

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

std::string Object::ToString(const ExprStmtPool& pool) const {
  switch (type_) {
    case INT:
      return std::to_string(int_);
    case FLOAT:
      return std::to_string(fp_);
    case STRING:
    case IDENTIFIER:
      return pool.strs_[str_id_];
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

Object MakeInt(int64_t val) {
  // std::cerr << "MakeInt: " << val << '\n';
  return {.type_ = Object::INT, .int_ = val};
}

Object MakeFloat(double val) {
  // std::cerr << "MakeFloat: " << val << '\n';
  return {.type_ = Object::FLOAT, .fp_ = val};
}

Object MakeString(StrId val) {
  // std::cerr << "MakeString\n";
  return {.type_ = Object::STRING, .str_id_ = val};
}

Object MakeBool(bool val) {
  // std::cerr << "MakeBool: " << val << '\n';
  return {.type_ = Object::BOOLEAN, .int_ = val};
}

Object MakeNone() {
  // std::cerr << "MakeNone\n";
  return {Object::NONE};
}

Object MakeBuiltin(BuiltinFn fn) {
  // std::cerr << "MakeBuiltin\n";
  return {.type_ = Object::BUILTIN_FUNCTION, .builtin_fn_ = fn};
}

Object MakeUserFn(StackFrameId prev_frame, StrBlockId args, StmtBlockId stmts) {
  // std::cerr << "MakeUserFn\n";
  Object res = {.type_ = Object::USER_FUNCTION, .stack_frame_ = prev_frame};
  res.user_fn_.args_block_ = args;
  res.user_fn_.stmt_block_ = stmts;
  return res;
}

}  // namespace ilang
