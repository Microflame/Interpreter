#include "object.h"

#include <stdexcept>

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

Object MakeInt(int64_t val) { return {.type_ = Object::INT, .int_ = val}; }

Object MakeFloat(double val) { return {.type_ = Object::FLOAT, .fp_ = val}; }

Object MakeString(TokenStrId val) {
  return {.type_ = Object::STRING, .str_id_ = val};
}

Object MakeBool(bool val) { return {.type_ = Object::BOOLEAN, .int_ = val}; }

Object MakeNone() { return {Object::NONE}; }

}  // namespace ilang
