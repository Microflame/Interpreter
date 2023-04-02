#include "slip/tokenizer.hpp"

#include "slip/source.hpp"
#include "slip/expr_stmt_pool.hpp"

namespace slip
{

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


Tokenizer::Tokenizer() :
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

std::vector<Token> Tokenizer::Run(const Source& source, ExprStmtPool* pool) {
  result_ = {};

  pool_ = pool;

  begin_ = source.Data();
  cur_ = source.Data();
  end_ = source.Data() + source.Size();

  col_idx_ = 0;
  single_indent_length_ = 0;
  current_indent_level_ = 0;
  cur_token_id_ = 0;

  log_(Logger::kDebug, "Scanner started.");

  while (!IsAtEOF()) {
    Parse();
  }
  FinishFile();

  log_(Logger::kDebug, "Scanner finished with %d non empty tokens.",
        result_.size());
  return std::move(result_);
}

void Tokenizer::FinishFile() {
  PushAnyToken(TokenType::NEWLINE, 0);
  while (current_indent_level_) {
    current_indent_level_ -= 1;
    PushAnyToken(TokenType::UNINDENT, 0);
  }
  PushAnyToken(TokenType::END_OF_FILE, 0);
}

void Tokenizer::Parse() {
  if (TryGetIndentation()) return;
  if (TryGetIntToken()) return;
  if (TryGetFloatToken()) return;
  if (TryGetStringToken()) return;

  std::string identifier(GetIdentifier());
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
      ThrowSourceError("Bad token");
  }
}

bool Tokenizer::TryGetIndentation() {
  if (col_idx_ != 0) {
    return false;
  }

  size_t num_indent_symbols = 0;
  while (cur_[num_indent_symbols] == ' ') {
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
    ThrowSourceError("Expected indentation to be multiple of TODO");
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

bool Tokenizer::TryGetIntToken() {
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

bool Tokenizer::TryGetFloatToken() {
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

bool Tokenizer::TryGetStringToken() {
  char quote = '\0';
  if (*cur_ == '"' || *cur_ == '\'') {
    quote = *cur_;
  } else {
    return false;
  }

  std::string result;

  bool escape = false;
  size_t num_remaining = GetNumRemainingChars();
  for (size_t i = 1; i < num_remaining; ++i) {
    if (cur_[i] == '\n') {
      ThrowSourceError("Unexpected end of line inside of string");
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
  ThrowSourceError("Expected closing quote");
  return true;
}

bool Tokenizer::IsAtEOF() const {
  return cur_ >= end_;
}

std::string_view Tokenizer::GetIdentifier() const {
  size_t i = 0;
  if (!IsAlpha(*cur_)) {
    return {};
  }
  ++i;

  while (IsAlphanum(cur_[i])) {
    ++i;
  }

  return std::string_view(cur_, cur_ + i);
}

size_t Tokenizer::GetNumRemainingChars() const {
  return cur_ <= end_ ? end_ - cur_ : 0;
}

void Tokenizer::FinishLine() {
  while (!IsAtEOF() && *cur_ != '\n')
  {
    cur_ += 1;
    col_idx_ += 1;
  }
}

void Tokenizer::PushNothing(size_t advance) {
  cur_ += advance;
  col_idx_ += advance;
}

void Tokenizer::PushToken(Token t, size_t advance) {
  PushNothing(advance);
  if (t.GetType() == TokenType::NEWLINE && (!result_.size() || result_.back().GetType() == TokenType::NEWLINE)) {
    return;
  }
  result_.emplace_back(t);
}

void Tokenizer::PushAnyToken(TokenType type, size_t advance) {
  Token t = {
    .meta_ = {
      .type_ = type,
      .id_ = cur_token_id_++
    }
  };
  t.data_.int_ = 0;
  PushToken(t, advance);
}

void Tokenizer::PushStringToken(std::string str, size_t advance) {
  Token t = {
    .meta_ = {
      .type_ = TokenType::STRING,
      .id_ = cur_token_id_++
    }
  };
  t.data_.str_idx_ = pool_->PushStr(std::move(str));
  PushToken(t, advance);
}

void Tokenizer::PushIdentifierToken(std::string name, size_t advance) {
  Token t = {
    .meta_ = {
      .type_ = TokenType::IDENTIFIER,
      .id_ = cur_token_id_++
    }
  };
  t.data_.str_idx_ = pool_->PushStr(std::move(name));
  PushToken(t, advance);
}

void Tokenizer::PushIntToken(int64_t val, size_t advance) {
  Token t = {
    .meta_ = {
      .type_ = TokenType::INT_LITERAL,
      .id_ = cur_token_id_++
    }
  };
  t.data_.int_ = val;
  PushToken(t, advance);
}

void Tokenizer::PushFloatToken(double val, size_t advance) {
  Token t = {
    .meta_ = {
      .type_ = TokenType::FLOAT_LITERAL,
      .id_ = cur_token_id_++
    }
  };
  t.data_.fp_ = val;
  PushToken(t, advance);
}

void Tokenizer::ThrowSourceError(std::string message) {
  throw InSourceError({.offset = size_t(cur_ - begin_), .length = 1}, std::move(message));
}

} // namespace slip
