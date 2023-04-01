#pragma once

#include <string>

namespace slip
{

struct SourceRange {
  size_t offset = 0;
  size_t length = 0;
};

struct SourceIntersection {
  std::string_view line;
  size_t row = 0;
  size_t column = 0;
};

static SourceIntersection FindLine(std::string_view source, size_t offset);

class Source {
public:
  static constexpr size_t NUM_PADDING_ZEROS = 16;

  static Source LoadFromFile(std::string file_name);
  static Source LoadFromString(const std::string& content);

  const std::string& GetFileName() const;
  std::string_view GetContent() const;
  const char* Data() const;
  size_t Size() const;
  SourceIntersection FindLine(size_t offset) const;

private:
  std::string content_;
  std::string file_name_;

  Source() = default;
  Source(const Source& other) = default;
  Source& operator=(const Source& other) = default;

  void LoadFromFileImpl(std::string file_name);
  void LoadFromStringImpl(const std::string& content);
  void SetContent(const std::string& content);
};

class InSourceError: public std::exception {
public:
  InSourceError(SourceRange range, std::string message) :
    range_(range),
    message_(std::move(message))
  {}

  SourceRange range_;
  std::string message_;
};

} // namespace slip
