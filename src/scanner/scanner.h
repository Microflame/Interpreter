#pragma once

#include <istream>
#include <sstream>
#include <vector>

#include "token.h"
#include "logger.h"
#include "util/string_tools.h"

namespace scanner
{

class Scanner
{
public:
  Scanner(const std::string& source)
    : kSource(source),
      cur_(kSource.begin()),
      log_(Logger::kDebug),
      error_(false)
  {
    
  }

  std::vector<Token> GetTokens()
  {
    log_(Logger::kDebug, "Scanner started.");

    std::vector<Token> result;
    cur_ = kSource.begin();

    while (true)
    {
      Token tok = GetNextToken();
      if (tok.GetType() == Token::EMPTY_TOKEN || tok.GetType() == Token::COMMENT)
      {
        continue;
      }
      result.push_back(tok);
      if (tok.GetType() == Token::END_OF_FILE)
      {
        break;
      }
    }

    log_(Logger::kDebug, "Scanner finished with %d non empty tokens.", result.size());

    return result;
  }

  bool HasError() { return error_; }

private:
  const std::string& kSource;

  std::string::const_iterator cur_;

  Token cur_token_;

  Logger log_;

  bool error_;

  Token GetNextToken()
  {
    if (cur_ == kSource.end())
    {
      return ExtractToken(Token::END_OF_FILE, 0);
    }

    cur_token_ = Token();

    TryGetKeywordToken("and", Token::AND);
    TryGetKeywordToken("class", Token::CLASS);
    TryGetKeywordToken("else", Token::ELSE);
    TryGetKeywordToken("false", Token::FALSE);
    TryGetKeywordToken("for", Token::FOR);
    TryGetKeywordToken("func", Token::FUNC);
    TryGetKeywordToken("if", Token::IF);
    TryGetKeywordToken("none", Token::NONE);
    TryGetKeywordToken("or", Token::OR);
    TryGetKeywordToken("print", Token::PRINT);
    TryGetKeywordToken("super", Token::SUPER);
    TryGetKeywordToken("print", Token::THIS);
    TryGetKeywordToken("true", Token::TRUE);
    TryGetKeywordToken("var", Token::VAR);
    TryGetKeywordToken("while", Token::WHILE);
    TryGetKeywordToken("Int", Token::INT_TYPE);
    TryGetKeywordToken("Float", Token::FLOAT_TYPE);

    TryGetIntToken();
    TryGetFloatToken();
    TryGetIdentifierToken();
    TryGetStringToken();

    if (cur_token_.Length())
    {
      return ExtractToken(cur_token_);
    }

    switch (*cur_)
    {
      case '\n':
      case ' ':
      case '\t':
      case '\r':
      case '\v': // Vertical tab
      case '\f': // Formfeed
        return ExtractToken(Token::EMPTY_TOKEN, 1);
      case '(': return ExtractToken(Token::LEFT_PAREN, 1);
      case ')': return ExtractToken(Token::RIGHT_PAREN, 1);
      case '{': return ExtractToken(Token::LEFT_BRACE, 1);
      case '}': return ExtractToken(Token::RIGHT_BRACE, 1);
      case ',': return ExtractToken(Token::COMMA, 1);
      case '.': return ExtractToken(Token::DOT, 1);
      case '+': return ExtractToken(Token::PLUS, 1);
      case '-': return ExtractToken(Token::MINUS, 1);
      case ':': return ExtractToken(Token::COLON, 1);
      case ';': return ExtractToken(Token::SEMICOLON, 1);
      case '*': return ExtractToken(Token::STAR, 1);
      // Double char op
      case '!':
        return MatchChar(1, '=') ? ExtractToken(Token::BANG_EQUAL, 2) : ExtractToken(Token::BANG, 1);
      case '=':
        return MatchChar(1, '=') ? ExtractToken(Token::EQUAL_EQUAL, 2) : ExtractToken(Token::EQUAL, 1);
      case '>':
        return MatchChar(1, '=') ? ExtractToken(Token::GREATER_EQUAL, 2) : ExtractToken(Token::GREATER, 1);
      case '<':
        return MatchChar(1, '=') ? ExtractToken(Token::LESS_EQUAL, 2) : ExtractToken(Token::LESS, 1);
      // Comments
      case '/':
        if (MatchChar(1, '/'))
        {
          size_t terminator_pos = kSource.find('\n', GetOffset() + 2);
          size_t end_pos = terminator_pos != std::string::npos ? terminator_pos + 1 : kSource.size();
          return ExtractToken(Token::COMMENT, end_pos - GetOffset());
        }
        else if (MatchChar(1, '*'))
        {
          size_t terminator_pos = kSource.find("*/", GetOffset() + 2);
          size_t end_pos = terminator_pos != std::string::npos ? terminator_pos + 2 : kSource.size();
          return ExtractToken(Token::COMMENT, end_pos - GetOffset());
        }
        else
        {
          return ExtractToken(Token::SLASH, 1);
        }
      default:
        auto pos = util::string_tools::GetPosition(kSource, GetOffset());
        ReportError("[SCANNER]:%d:%d: bad token.", pos.first, pos.second);
        return ExtractToken(Token::BAD_TOKEN, 1);
    }
  }

  void UpdateCurrentToken(Token::Type type, size_t size)
  {
    if (size > cur_token_.Length())
    {
      cur_token_ = Token(type, &*cur_, size);
    }
  }

  void UpdateCurrentToken(const Token& tok)
  {
    if (tok.Length() > cur_token_.Length())
    {
      cur_token_ = tok;
    }
  }

  void TryGetKeywordToken(const std::string& target, Token::Type type)
  {
    size_t token_len = target.size();
    if (MatchStr(target))
    {
      UpdateCurrentToken(type, token_len);
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
    Token tok(Token::INT_LITERAL, &*cur_, i, value);
    UpdateCurrentToken(tok);
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
        Token tok(Token::FLOAT_LITERAL, &*cur_, i, value);
        UpdateCurrentToken(tok);
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
      Token tok(Token::FLOAT_LITERAL, &*cur_, mantissa, value);
      UpdateCurrentToken(tok);
      return;
    }

    while (IsDigit(cur_[i]))
    {
      ++i;
    }
    double value = std::stod(std::string(cur_, cur_ + i));
    Token tok(Token::FLOAT_LITERAL, &*cur_, i, value);
    UpdateCurrentToken(tok);
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


    std::string value(cur_, cur_ + i);
    Token tok(Token::IDENTIFIER, &*cur_, i, value);
    UpdateCurrentToken(tok);
  }

  void TryGetStringToken()
  {
    if (*cur_ != '"') return;

    std::string result;

    bool escape = false;
    for (size_t i = 1; i < Remaining(); ++i)
    {
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
      if (cur_[i] == '"')
      {
        Token tok(Token::STRING, &*cur_, i + 1, result);
        UpdateCurrentToken(tok);
        return;
      }
      if (cur_[i] == '\n')
      {
        auto pos = util::string_tools::GetPosition(kSource, GetOffset());
        ReportError("[SCANNER]:%d:%d: unexpected end of line inside of string.", pos.first, pos.second);
        UpdateCurrentToken(Token::BAD_TOKEN, i);
        return;
      }
      result += cur_[i];
    }
    auto pos = util::string_tools::GetPosition(kSource, GetOffset());
    ReportError("[SCANNER]:%d:%d: invalid symbol.", pos.first, pos.second);
    UpdateCurrentToken(Token::BAD_TOKEN, Remaining());
  }

  bool MatchStr(const std::string& target)
  {
    return (target.size() <= Remaining()) && (kSource.compare(GetOffset(), target.size(), target) == 0);
  }

  bool MatchChar(size_t offset, char chr)
  {
    std::string::const_iterator target = cur_ + offset;
    return (target < kSource.end()) && (*target == chr);
  }

  Token ExtractToken(Token::Type type, size_t size)
  {
    Token tok(type, &*cur_, size);
    cur_ += size;
    return tok;
  }

  Token ExtractToken(Token token)
  {
    cur_ += token.Length();
    return token;
  }

  size_t GetOffset()
  {
    return cur_ - kSource.begin();
  }

  size_t Remaining()
  {
    return kSource.end() - cur_;
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

} // scanner