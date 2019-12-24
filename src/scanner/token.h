#pragma once

#include <memory>
#include <string>
#include <sstream>

#include "util/string_tools.h"
#include "common/object.h"


#define INTERP_FORALL_TOKEN_TYPES(_) \
  /* Single-character tokens. */ \
  _(LEFT_PAREN) \
  _(RIGHT_PAREN) \
  _(LEFT_BRACE) \
  _(RIGHT_BRACE) \
  _(COMMA) \
  _(DOT) \
  _(MINUS) \
  _(PLUS) \
  _(COLON) \
  _(SEMICOLON) \
  _(SLASH) \
  _(STAR) \
  /* One or two character tokens. */ \
  _(BANG) \
  _(BANG_EQUAL) \
  _(EQUAL) \
  _(EQUAL_EQUAL) \
  _(GREATER) \
  _(GREATER_EQUAL) \
  _(LESS) \
  _(LESS_EQUAL) \
  /* Literals. */ \
  _(IDENTIFIER) \
  _(STRING) \
  _(INT_LITERAL) \
  _(FLOAT_LITERAL) \
  /* Types. */ \
  _(INT_TYPE) \
  _(FLOAT_TYPE) \
  /* Keywords. */ \
  _(AND) \
  _(CLASS) \
  _(ELSE) \
  _(FALSE) \
  _(FUNC) \
  _(FOR) \
  _(IF) \
  _(NONE) \
  _(OR) \
  _(PRINT) \
  _(RETURN) \
  _(SUPER) \
  _(THIS) \
  _(TRUE) \
  _(VAR) \
  _(WHILE) \
  _(END_OF_FILE) \
  /* Non lang. */ \
  _(EMPTY_TOKEN) \
  _(COMMENT) \
  _(BAD_TOKEN)

namespace scanner
{

class Token
{
public:
  enum Type
  {
#define INTERP_PUT_WITH_COMMA(_) _,
    INTERP_FORALL_TOKEN_TYPES(INTERP_PUT_WITH_COMMA)
#undef INTERP_PUT_WITH_COMMA
  };

  explicit Token(Type type, const char* begin, size_t size, int64_t content)
    : object_(common::MakeInt(content)),
      type_(type),
      begin_(begin),
      size_(size)
  {}

  explicit Token(Type type, const char* begin, size_t size, double content)
    : object_(common::MakeFloat(content)),
      type_(type),
      begin_(begin),
      size_(size)
  {}

  explicit Token(Type type, const char* begin, size_t size, std::string content)
    : object_(common::MakeString(content)),
      type_(type),
      begin_(begin),
      size_(size)
  {}

  template <typename T>
  Token(Type type, const char* begin, size_t size, T content) = delete;

  Token(Type type, const char* begin, size_t size)
    : object_(common::MakeNone()),
      type_(type),
      begin_(begin),
      size_(size)
  {}

  Token() : Token(EMPTY_TOKEN, nullptr, 0) {}

  std::string ToRawString() const
  {
    return std::string(begin_, begin_ + size_);
  }

  std::string ToString() const
  {
    std::stringstream ss;

    switch (type_)
    {
#define INTERP_PUT_TOKEN_NAME(_) case _: { ss << #_; break; }
      INTERP_FORALL_TOKEN_TYPES(INTERP_PUT_TOKEN_NAME)
#undef INTERP_PUT_TOKEN_NAME
      default: ss << "BAD TOKEN!";
    }

    ss << "(\"";

    switch (type_)
    {
      case INT_LITERAL:
        ss << std::to_string(object_.AsInt());
        break;
      case FLOAT_LITERAL:
        ss << std::to_string(object_.AsFloat());
        break;
      case IDENTIFIER:
      case STRING:
        ss  << object_.AsString();
        break;
      default:
        ss << std::string(begin_, begin_ + size_);
    }
    
    ss << "\")";
    return ss.str();
  }

  size_t Length() const { return size_; }

  Type GetType() const { return type_; }

  bool OneOf(const std::vector<Type>& types)
  {
    for (Type t: types)
    {
      if (type_ == t)
      {
        return true;
      }
    }
    return false;
  }

  template <typename ... Args>
  bool OneOf(Type first, Args ... other) const
  {
    return (type_ == first) || OneOf(other...);
  }

  bool OneOf(Type first) const
  {
    return type_ == first;
  }

  std::pair<size_t, size_t> GetPosition(const std::string& source) const
  {
    return util::string_tools::GetPosition(source, begin_ - source.c_str());
  }

  common::Object& GetObject()
  {
    return object_;
  }

  const common::Object& GetObject() const
  {
    return object_;
  }

private:
  common::Object object_;
  Type type_;
  const char* begin_;
  size_t size_;
};

  
} // scanner