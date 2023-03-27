#pragma once

#include <cstring>
#include <sstream>
#include <vector>

#include "token.h"
#include "util/logger.h"
#include "util/string_tools.h"

namespace ilang {

static bool IsInRange(char chr, char lo, char hi) {
  return (chr >= lo) && (chr <= hi);
}

static bool IsDigit(char chr) { return IsInRange(chr, '0', '9'); }

static bool IsAlpha(char chr) {
  return IsInRange(chr, 'a', 'z') || IsInRange(chr, 'A', 'Z') || (chr == '_');
}

static bool IsAlphanum(char chr) { return IsAlpha(chr) || IsDigit(chr); }

static constexpr uint16_t JoinChars(uint16_t first, uint16_t second) {
  return (first << 8) | second;
}

class Scanner {
 public:
  Scanner() :
    keywords_({
      {"and", TokenType::AND},
      {"or", TokenType::OR},
      {"not", TokenType::NOT},
      {"if", TokenType::IF},
      {"else", TokenType::ELSE},
      {"True", TokenType::TRUE},
      {"False", TokenType::FALSE},
      {"class", TokenType::CLASS},
      {"def", TokenType::DEF},
      {"return", TokenType::RETURN},
      {"for", TokenType::FOR},
      {"in", TokenType::IN},
      {"while", TokenType::WHILE},
      {"none", TokenType::NONE},
      {"super", TokenType::SUPER},
      {"self", TokenType::THIS},
    }),
    log_(Logger::kWarning) {}

  std::vector<Token> GetTokens(std::string source,
                               TokenSpawner* token_spawner) {
    token_spawner_ = token_spawner;
    source.append(16, '\0');
    cur_ = source.data();
    end_ = source.data() + source.size() - 16;
    error_ = false;
    col_idx_ = 0;
    current_indent_level_ = 0;
    single_indent_length_ = 0;
    result_ = {};

    log_(Logger::kDebug, "Scanner started.");

    while (!IsAtEOF()) {
      Parse();
    }
    FinishFile();

    log_(Logger::kDebug, "Scanner finished with %d non empty tokens.",
         result_.size());
    return std::move(result_);
  }

  bool HasError() { return error_; }

 private:
  const std::unordered_map<std::string, TokenType> keywords_;

  std::vector<Token> result_;

  TokenSpawner* token_spawner_;

  const char* cur_;
  const char* end_;

  Logger log_;
  bool error_;
  int col_idx_;

  int single_indent_length_;
  int current_indent_level_;

  void FinishFile() {
    PushAnyToken(TokenType::NEWLINE, 0);
    while (current_indent_level_) {
      current_indent_level_ -= 1;
      PushAnyToken(TokenType::UNINDENT, 0);
    }
    PushAnyToken(TokenType::END_OF_FILE, 0);
  }

  bool IsAtEOF() { return cur_ >= end_; }

  void Parse() {
    if (TryGetIndentation()) return;
    if (TryGetIntToken()) return;
    if (TryGetFloatToken()) return;
    if (TryGetStringToken()) return;

    std::string identifier = GetIdentifier();
    if (identifier.size()) {
      auto it = keywords_.find(identifier);
      if (it != keywords_.end()) {
        PushAnyToken(it->second, identifier.size());
      } else {
        PushIdentifierToken(std::move(identifier), identifier.size());
      }
    }

    bool next_is_eq = cur_[1] == '=';

    switch (*cur_) {
      case ' ':
      case '\r':
        PushNothing(1); break;
      case '\n':
        PushAnyToken(TokenType::NEWLINE, 1);
        col_idx_ = 0;
        break;
      case '#':
        FinishLine(); break;
      case '(':
        PushAnyToken(TokenType::LEFT_PAREN, 1); break;
      case ')':
        PushAnyToken(TokenType::RIGHT_PAREN, 1); break;
      case '{':
        PushAnyToken(TokenType::LEFT_BRACE, 1); break;
      case '}':
        PushAnyToken(TokenType::RIGHT_BRACE, 1); break;
      case '[':
        PushAnyToken(TokenType::LEFT_BRAKET, 1); break;
      case ']':
        PushAnyToken(TokenType::RIGHT_BRAKET, 1); break;
      case ',':
        PushAnyToken(TokenType::COMMA, 1); break;
      case '.':
        PushAnyToken(TokenType::DOT, 1); break;
      case '+':
        PushAnyToken(TokenType::PLUS, 1); break;
      case '-':
        PushAnyToken(TokenType::MINUS, 1); break;
      case ':':
        PushAnyToken(TokenType::COLON, 1); break;
      case ';':
        PushAnyToken(TokenType::SEMICOLON, 1); break;
      case '*':
        PushAnyToken(TokenType::STAR, 1); break;
      case '/':
        PushAnyToken(TokenType::SLASH, 1); break;
      // Double char op
      case '!':
        next_is_eq ? PushAnyToken(TokenType::BANG_EQUAL, 2) : PushAnyToken(TokenType::BANG, 1); break;
      case '=':
        next_is_eq ? PushAnyToken(TokenType::EQUAL_EQUAL, 2) : PushAnyToken(TokenType::EQUAL, 1); break;
      case '>':
        next_is_eq ? PushAnyToken(TokenType::GREATER_EQUAL, 2) : PushAnyToken(TokenType::GREATER, 1); break;
      case '<':
        next_is_eq ? PushAnyToken(TokenType::LESS_EQUAL, 2) : PushAnyToken(TokenType::LESS, 1); break;
      default:
        auto pos = GetPosition(GetRemaining());
        ReportError("[SCANNER]:%d:%d: bad token.", pos.line, pos.column);
        FinishLine();
    }
  }

  bool TryGetIndentation() {
    if (col_idx_ != 0) {
      return false;
    }

    size_t num_indent_symbols = 0;
    while (num_indent_symbols < Remaining() && cur_[num_indent_symbols] == ' ') {
      num_indent_symbols += 1;
    }
    
    char cur_chr = cur_[num_indent_symbols];
    if (cur_chr == '#' || cur_chr == '\r' || cur_chr == '\n') {
      return false;
    }

    if (single_indent_length_ == 0) {
      single_indent_length_ = num_indent_symbols;
    }

    if (!single_indent_length_) {
      return false;
    }

    if (num_indent_symbols % single_indent_length_) {
      auto pos = GetPosition(GetRemaining());
      ReportError("[SCANNER]:%d:%d: bad token.", pos.line, pos.column);
      FinishLine();
      return true;
    }

    int level = num_indent_symbols / single_indent_length_;

    if (level == current_indent_level_) {
      return false;
    }

    if (level < current_indent_level_) {
      PushNothing(num_indent_symbols);
    }

    while (level != current_indent_level_) {
      if (level < current_indent_level_) {
        PushAnyToken(TokenType::UNINDENT, 0);
        current_indent_level_ -= 1;
      } else {
        PushAnyToken(TokenType::INDENT, single_indent_length_);
        current_indent_level_ += 1;
      }
    }
    return true;
  }

  bool TryGetIntToken() {
    size_t i = 0;
    int64_t value = 0;
    while (IsDigit(cur_[i])) {
      value = value * 10 + (cur_[i] - '0');
      ++i;
    }

    if (!i) {
      return false;
    }

    PushIntToken(value, i);
    return true;
  }

  bool TryGetFloatToken() {
    if (!IsDigit(cur_[0]) && cur_[0] != '.') {
      return false;
    }
    size_t i = 0;
    size_t whole_part_size = 0;
    size_t fractional_part_size = 0;

    // whole part
    while (IsDigit(cur_[whole_part_size])) {
      ++whole_part_size;
    }
    i += whole_part_size;

    if (cur_[i] == '.') {
      ++i;
    }

    // fractional part
    while (IsDigit(cur_[i + fractional_part_size])) {
      ++fractional_part_size;
    }
    i += fractional_part_size;

    if (!whole_part_size && !fractional_part_size) {
      return false;
    }

    size_t until_exp = i;

    if (cur_[i] == 'e' || cur_[i] == 'E') {
      i += 1;
      if (cur_[i] == '-' || cur_[i] == '+') {
        i += 1;
      }
      if (!IsDigit(cur_[i])) {
        i = until_exp;
      } else {
        while (IsDigit(cur_[i])) {
          ++i;
        }
      }
    }

    double value = std::stod(std::string(cur_, cur_ + i));
    PushFloatToken(value, i);
    return true;
  }

  bool TryGetStringToken() {
    char quote = '\0';
    if (*cur_ == '"' || *cur_ == '\'') {
      quote = *cur_;
    } else {
      return false;
    }

    std::string result;

    bool escape = false;
    for (size_t i = 1; i < Remaining(); ++i) {
      if (cur_[i] == '\n') {
        auto pos = GetPosition(GetRemaining());
        ReportError("[SCANNER]:%d:%d: unexpected end of line inside of string.",
                    pos.line, pos.column);
        FinishLine();
        return true;
      }
      if (escape) {
        escape = false;
        switch (cur_[i]) {
          case 'n':
            result += '\n';
            break;
          case 't':
            result += '\t';
            break;
          case '\\':
            result += '\\';
            break;
          case '\"':
            result += '\"';
            break;
          case '\'':
            result += '\'';
            break;
          default:
            result += '\\';
            result += cur_[i];
        }
        continue;
      }
      if (cur_[i] == '\\') {
        escape = true;
        continue;
      }
      if (cur_[i] == quote) {
        PushStringToken(std::move(result), i);
        return true;
      }
      result += cur_[i];
    }
    auto pos = GetPosition(GetRemaining());
    ReportError("[SCANNER]:%d:%d: Expected closing quote.", pos.line, pos.column);
    FinishLine();
    return true;
  }

  std::string GetIdentifier() {
    std::string res;
    size_t i = 0;
    if (!IsAlpha(*cur_)) {
      return {};
    }
    ++i;

    while (IsAlphanum(cur_[i])) {
      ++i;
    }

    return std::string(cur_, cur_ + i);
  }

  bool MatchChar(size_t offset, char chr) {
    const char* s = cur_ + offset;
    return (s < end_) && (*s == chr);
  }

  std::string_view GetRemaining() {
    return std::string_view(cur_, end_ - cur_);
  }

  size_t Remaining() { return end_ - cur_; }

  void FinishLine() {
    while (Remaining() && *cur_ != '\n')
    {
      cur_ += 1;
      col_idx_ += 1;
    }
  }

  void PushNothing(size_t advance) {
    cur_ += advance;
    col_idx_ += advance;
  }

  void PushToken(Token t, size_t advance) {
    PushNothing(advance);
    if (t.GetType() == TokenType::NEWLINE && (!result_.size() || result_.back().GetType() == TokenType::NEWLINE)) {
      return;
    }
    result_.emplace_back(t);
  }

  void PushAnyToken(TokenType type, size_t advance) {
    PushToken(token_spawner_->Spawn(type), advance);
  }

  void PushStringToken(std::string str, size_t advance) {
    PushToken(token_spawner_->Spawn(TokenType::STRING, std::move(str)), advance);
  }

  void PushIdentifierToken(std::string name, size_t advance) {
    PushToken(token_spawner_->Spawn(TokenType::IDENTIFIER, std::move(name)), advance);
  }

  void PushIntToken(int64_t val, size_t advance) {
    PushToken(token_spawner_->Spawn(TokenType::INT_LITERAL, val), advance);
  }

  void PushFloatToken(double val, size_t advance) {
    PushToken(token_spawner_->Spawn(TokenType::FLOAT_LITERAL, val), advance);
  }

  template <size_t N, typename... Args>
  void ReportError(const char (&message)[N], Args... args) {
    error_ = true;
    log_(Logger::kError, message, args...);
  }

  void ReportError(const char* message) {
    error_ = true;
    log_(Logger::kError, message);
  }
};

}  // namespace ilang