#pragma once

#include <sstream>
#include <vector>
#include <cstring>

#include "token.h"
#include "logger.h"
#include "util/string_tools.h"

namespace ilang
{

class Scanner
{
public:
  Scanner() :
    log_(Logger::kWarning)
  {}

  std::vector<Token> GetTokens(const std::string& source, TokenSpawner* token_spawner)
  {
    token_spawner_ = token_spawner;
    begin_ = source.data();
    cur_ = source.data();
    end_ = source.data() + source.size();
    error_ = false;
    is_at_bol_ = true;
    indent_level_ = 0;
    current_indent_level_ = 0;
    current_indent_col_ = 0;
    indent_stack_ = {};

    log_(Logger::kDebug, "Scanner started.");

    std::vector<Token> result;

    while (!IsAtEOF())
    {
      Token tok = GetNextToken();
      if (tok.GetType() == TokenType::EMPTY_TOKEN || tok.GetType() == TokenType::COMMENT)
      {
        continue;
      }
      result.push_back(tok);
    }
    HandleEOF(&result);

    log_(Logger::kDebug, "Scanner finished with %d non empty tokens.", result.size());
    return result;
  }

  bool HasError() { return error_; }

private:
  TokenSpawner* token_spawner_;

  const char* begin_;
  const char* cur_;
  const char* end_;

  TokenType cur_token_type_;
  size_t cur_token_length_;
  struct
  {
    int64_t int_;
    double fp_;
    std::string str_;
  } token_content_;

  Logger log_;
  bool error_;
  bool is_at_bol_;
  
  int indent_level_;
  int current_indent_level_;
  int current_indent_col_;
  std::vector<int> indent_stack_;

  void HandleEOF(std::vector<Token>* tokens)
  {
    tokens->push_back(ExtractToken(TokenType::NEWLINE, 0));
    while (indent_stack_.size())
    {
      indent_stack_.pop_back();
      tokens->push_back(ExtractToken(TokenType::UNINDENT, 0));
    }
    tokens->push_back(ExtractToken(TokenType::END_OF_FILE, 0));
  }

  bool IsAtEOF() { return cur_ == end_; }

  Token GetNextToken()
  {
    cur_token_type_ = TokenType::BAD_TOKEN;
    cur_token_length_ = 0;

    if (is_at_bol_)
    {
      is_at_bol_ = false;
      int line_indent = 0;
      size_t i = 0;
      size_t remaining = Remaining();
      while (i < remaining && (cur_[i] == ' ' || cur_[i] == '\t'))
      {
        line_indent += 1;
        i += 1;
      }
      if (Remaining() && cur_[i] == '#')
      {
        return ExtractToken(TokenType::EMPTY_TOKEN, i);
      }

      if (line_indent > current_indent_col_)
      {
        indent_stack_.push_back(line_indent - current_indent_col_);
        current_indent_col_ = line_indent;
        current_indent_level_ += 1;
      }
      while (line_indent < current_indent_col_ && indent_stack_.size())
      {
        current_indent_col_ -= indent_stack_.back();
        indent_stack_.pop_back();
        current_indent_level_ -= 1;
      }
      if (line_indent != current_indent_col_)
      {
        auto pos = util::string_tools::GetPosition(begin_, GetOffsetFromSrcStart());
        ReportError("[SCANNER]:%d:%d: indentation error.", pos.line, pos.column);
        return ExtractToken(TokenType::BAD_TOKEN, i);
      }
      return ExtractToken(TokenType::EMPTY_TOKEN, i);
    }

    while (current_indent_level_ != indent_level_)
    {
      if (current_indent_level_ > indent_level_)
      {
        indent_level_ += 1;
        return ExtractToken(TokenType::INDENT, 0);
      }
      else
      {
        indent_level_ -= 1;
        return ExtractToken(TokenType::UNINDENT, 0);
      }
    }

    TryGetKeywordToken("and", TokenType::AND);
    TryGetKeywordToken("or", TokenType::OR);
    TryGetKeywordToken("not", TokenType::NOT);
    TryGetKeywordToken("if", TokenType::IF);
    TryGetKeywordToken("else", TokenType::ELSE);
    TryGetKeywordToken("True", TokenType::TRUE);
    TryGetKeywordToken("False", TokenType::FALSE);
    TryGetKeywordToken("class", TokenType::CLASS);
    TryGetKeywordToken("def", TokenType::DEF);
    TryGetKeywordToken("return", TokenType::RETURN);
    TryGetKeywordToken("for", TokenType::FOR);
    TryGetKeywordToken("in", TokenType::IN);
    TryGetKeywordToken("while", TokenType::WHILE);
    TryGetKeywordToken("none", TokenType::NONE);
    TryGetKeywordToken("super", TokenType::SUPER);
    TryGetKeywordToken("self", TokenType::THIS);

    TryGetIntToken();
    TryGetFloatToken();
    TryGetIdentifierToken();
    TryGetStringToken();
    // TryGetMultilineStringToken();
    TryGetCommentToken();

    if (cur_token_length_)
    {
      return ExtractCurrentToken();
    }

    switch (*cur_)
    {
      case '\n':
        is_at_bol_ = true;
        return ExtractToken(TokenType::NEWLINE, 1);
      case ' ':
      case '\t':
      case '\r':
      case '\v':
      case '\f':
        return ExtractToken(TokenType::EMPTY_TOKEN, 1);
      case '(': return ExtractToken(TokenType::LEFT_PAREN, 1);
      case ')': return ExtractToken(TokenType::RIGHT_PAREN, 1);
      case '{': return ExtractToken(TokenType::LEFT_BRACE, 1);
      case '}': return ExtractToken(TokenType::RIGHT_BRACE, 1);
      case '[': return ExtractToken(TokenType::LEFT_BRAKET, 1);
      case ']': return ExtractToken(TokenType::RIGHT_BRAKET, 1);
      case ',': return ExtractToken(TokenType::COMMA, 1);
      case '.': return ExtractToken(TokenType::DOT, 1);
      case '+': return ExtractToken(TokenType::PLUS, 1);
      case '-': return ExtractToken(TokenType::MINUS, 1);
      case ':': return ExtractToken(TokenType::COLON, 1);
      case ';': return ExtractToken(TokenType::SEMICOLON, 1);
      case '*': return ExtractToken(TokenType::STAR, 1);
      case '/': return ExtractToken(TokenType::SLASH, 1);
      // Double char op
      case '!':
        return MatchChar(1, '=') ? ExtractToken(TokenType::BANG_EQUAL, 2) : ExtractToken(TokenType::BANG, 1);
      case '=':
        return MatchChar(1, '=') ? ExtractToken(TokenType::EQUAL_EQUAL, 2) : ExtractToken(TokenType::EQUAL, 1);
      case '>':
        return MatchChar(1, '=') ? ExtractToken(TokenType::GREATER_EQUAL, 2) : ExtractToken(TokenType::GREATER, 1);
      case '<':
        return MatchChar(1, '=') ? ExtractToken(TokenType::LESS_EQUAL, 2) : ExtractToken(TokenType::LESS, 1);
      default:
        auto pos = util::string_tools::GetPosition(begin_, GetOffsetFromSrcStart());
        ReportError("[SCANNER]:%d:%d: bad token.", pos.line, pos.column);
        return ExtractToken(TokenType::BAD_TOKEN, 1);
    }
  }

  void UpdateCurrentToken(TokenType type, size_t size)
  {
    if (size > cur_token_length_)
    {
      cur_token_type_ = type;
      cur_token_length_ = size;
    }
  }

  void TryGetKeywordToken(const std::string& target, TokenType type)
  {
    size_t token_len = target.size();
    if (token_len < cur_token_length_)
    {
      return;
    }
    if (Remaining() >= token_len && memcmp(target.data(), cur_, token_len) == 0)
    {
      cur_token_type_ = type;
      cur_token_length_ = token_len;
    }
  }

  void TryGetIntToken()
  {
    size_t i = 0;
    while (IsDigit(cur_[i])) { ++i; }

    if (!i)
    {
      return;
    }

    int64_t value = std::stoll(std::string(cur_, cur_ + i));
    token_content_.int_ = value;
    UpdateCurrentToken(TokenType::INT_LITERAL, i);
  }

  void TryGetFloatToken()
  {
    if (!IsDigit(cur_[0]) && cur_[0] != '.')
    {
      return;
    }
    size_t i = 0;
    size_t whole_part_size = 0;
    size_t fractional_part_size = 0;
    bool has_dot = false;

    // whole part
    while (IsDigit(cur_[whole_part_size]))
    {
      ++whole_part_size;
    } 
    i += whole_part_size;

    if (cur_[i] == '.')
    {
      ++i;
      has_dot = true;
    }

    // fractional part
    while (IsDigit(cur_[i + fractional_part_size]))
    {
      ++fractional_part_size;
    } 
    i += fractional_part_size;

    if (!whole_part_size && !fractional_part_size)
    {
      return;
    }

    if (cur_[i] != 'e' && cur_[i] != 'E')
    {
      if (has_dot)
      {
        double value = std::stod(std::string(cur_, cur_ + i));
        token_content_.fp_ = value;
        UpdateCurrentToken(TokenType::FLOAT_LITERAL, i);
      }
      return;
    }

    // exponent
    size_t mantissa = i;
    ++i;
    if ((cur_[i] == '-') || (cur_[i] == '+'))
    {
      ++i;
    }

    if (!IsDigit(cur_[i]))
    {
      double value = std::stod(std::string(cur_, cur_ + mantissa));
      token_content_.fp_ = value;
      UpdateCurrentToken(TokenType::FLOAT_LITERAL, mantissa);
      return;
    }

    while (IsDigit(cur_[i]))
    {
      ++i;
    }
    double value = std::stod(std::string(cur_, cur_ + i));
    token_content_.fp_ = value;
    UpdateCurrentToken(TokenType::FLOAT_LITERAL, i);
  }

  void TryGetIdentifierToken()
  {
    size_t i = 0;
    if (!IsAlpha(*cur_))
    {
      return;
    }
    ++i;

    while (IsAlphanum(cur_[i])) { ++i; }


    token_content_.str_ = std::string(cur_, cur_ + i);
    UpdateCurrentToken(TokenType::IDENTIFIER, i);
  }

  void TryGetCommentToken()
  {
    if (*cur_ != '#') return;
    size_t remaining = Remaining();
    size_t i = 1;
    while (i < remaining && cur_[i] != '\n')
    {
      ++i;
    }
    token_content_.str_ = std::string(cur_ + 1, cur_ + i);
    UpdateCurrentToken(TokenType::COMMENT, i);
  }

  void TryGetStringToken()
  {
    char quote = '\0';
    if (*cur_ == '"' || *cur_ == '\'')
    {
      quote = *cur_;
    }
    else
    {
      return;
    }

    std::string result;

    bool escape = false;
    for (size_t i = 1; i < Remaining(); ++i)
    {
      if (cur_[i] == '\n')
      {
        auto pos = util::string_tools::GetPosition(begin_, GetOffsetFromSrcStart());
        ReportError("[SCANNER]:%d:%d: unexpected end of line inside of string.", pos.line, pos.column);
        UpdateCurrentToken(TokenType::BAD_TOKEN, i);
        return;
      }
      if (escape)
      {
        escape = false;
        switch (cur_[i])
        {
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
      if (cur_[i] == '\\')
      {
        escape = true;
        continue;
      }
      if (cur_[i] == quote)
      {
        token_content_.str_ = std::move(result);
        UpdateCurrentToken(TokenType::STRING, i + 1);
        return;
      }
      result += cur_[i];
    }
    auto pos = util::string_tools::GetPosition(begin_, GetOffsetFromSrcStart());
    ReportError("[SCANNER]:%d:%d: invalid symbol.", pos.line, pos.column);
    UpdateCurrentToken(TokenType::BAD_TOKEN, Remaining());
  }

  bool MatchChar(size_t offset, char chr)
  {
    const char* s = cur_ + offset;
    return (s < end_) && (*s == chr);
  }

  Token ExtractCurrentToken()
  {
    return ExtractToken(cur_token_type_, cur_token_length_);
  }

  Token ExtractToken(TokenType type, size_t size)
  {
    cur_ += size;
    switch (type)
    {
      case TokenType::COMMENT:
      case TokenType::STRING:
      case TokenType::IDENTIFIER:
        return token_spawner_->Spawn(type, std::move(token_content_.str_));
      case TokenType::INT_LITERAL:
        return token_spawner_->Spawn(type, token_content_.int_);
      case TokenType::FLOAT_LITERAL:
        return token_spawner_->Spawn(type, token_content_.fp_);
      default:
        return token_spawner_->Spawn(type);
    }
  }

  size_t GetOffsetFromSrcStart()
  {
    return cur_ - begin_;
  }

  size_t Remaining()
  {
    return end_ - cur_;
  }

  bool IsInRange(char chr, char lo, char hi)
  {
    return (chr >= lo) && (chr <= hi);
  }

  bool IsDigit(char chr)
  {
    return IsInRange(chr, '0', '9');
  }

  bool IsAlpha(char chr)
  {
    return IsInRange(chr, 'a', 'z') || IsInRange(chr, 'A', 'Z') || (chr == '_');
  }

  bool IsAlphanum(char chr)
  {
    return IsAlpha(chr) || IsDigit(chr);
  }

  template <size_t N, typename ... Args>
  void ReportError(const char (&message)[N], Args ... args)
  {
    error_ = true;
    log_(Logger::kError, message, args...);
  }

  void ReportError(const char* message)
  {
    error_ = true;
    log_(Logger::kError, message);
  }
};

} // ilang