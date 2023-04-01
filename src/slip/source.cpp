#include "slip/source.hpp"

#include <fstream>

namespace slip
{

SourceIntersection FindLine(std::string_view source, size_t offset) {
  if (offset >= source.size()) {
    return {};
  }

  size_t row = 0;
  size_t line_start = 0;
  for (size_t i = 0; i < offset; i++) {
    if (source[i] == '\n') {
      row += 1;
      line_start = i + 1;
    }
  }

  size_t line_end = offset;
  for (; line_end < source.size(); line_end++) {
    if (source[line_end] == '\n') {
      break;
    }
  }

  SourceIntersection res{
    .line = std::string_view(source.data() + line_start, source.data() + line_end),
    .row = row,
    .column = offset - line_start
  };
  return res;
}

Source Source::LoadFromFile(std::string file_name) {
  Source src;
  src.LoadFromFileImpl(std::move(file_name));
  return src;
}

Source Source::LoadFromString(const std::string& content) {
  Source src;
  src.LoadFromStringImpl(content);
  return src;
}

const std::string& Source::GetFileName() const {
  return file_name_;
}

std::string_view Source::GetContent() const {
  return {Data(), Size()};
}

const char* Source::Data() const {
  return content_.data();
}

size_t Source::Size() const {
  return content_.size() - NUM_PADDING_ZEROS;
}

SourceIntersection Source::FindLine(size_t offset) const {
  return ::slip::FindLine(content_, offset);
}

void Source::LoadFromFileImpl(std::string file_name) {
  file_name_ = std::move(file_name);
  std::ifstream fin(file_name_);
  if (!fin) {
    throw std::invalid_argument("Can not open file");
  }

  SetContent({(std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>()});
}

void Source::LoadFromStringImpl(const std::string& content) {
  SetContent(content);
}

void Source::SetContent(const std::string& content) {
  content_.reserve(content.size() + NUM_PADDING_ZEROS);
  content_.assign(content);
  content_.append(NUM_PADDING_ZEROS, '\0');
}

} // namespace slip
