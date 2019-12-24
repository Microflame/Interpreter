#pragma once

#include <stdexcept>

#include "scanner/token.h"

namespace interpreter
{

class InterpretError: public std::exception
{
public:
  InterpretError() = delete;

  InterpretError(const scanner::Token& token, const std::string& message)
    : token_(token),
      message_(message)
  {
  }

  const char* what() const noexcept override
  {
    return message_.c_str();
  }

private:
  const scanner::Token& token_;
  std::string message_;
};

} // namespace interpreter
