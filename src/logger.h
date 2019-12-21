#pragma once

#include <iostream>

class Logger
{
public:
  enum LogLevel
  {
    kDebug,
    kInfo,
    kWarning,
    kError,
    kFatal
  };

  Logger(LogLevel log_level)
    : kBufferSize(1024), log_level_(log_level) {}

  template <size_t N, typename ... Args>
  void operator()(LogLevel log_level, const char (&message)[N], Args ... args)
  {
    if (log_level < log_level_)
    {
      return;
    }

    char buffer[kBufferSize];
    snprintf(buffer, kBufferSize, message, args...);

    std::cerr << buffer << "\n";
  }

  void operator()(LogLevel log_level, const char* message)
  {
    if (log_level < log_level_)
    {
      return;
    }

    std::cerr << message << "\n";
  }

private:
  const size_t kBufferSize;
  LogLevel log_level_;
};