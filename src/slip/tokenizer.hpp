#pragma once

#include <vector>
#include <unordered_map>

#include "slip/token.hpp"
#include "slip/util/logger.hpp"

namespace slip {

class Source;
class Context;

class Tokenizer {
public:
  Tokenizer();

  std::vector<Token> Run(const Source& source, Context* ctx);

private:
  const std::unordered_map<std::string, TokenType> keywords_;
  std::vector<Token> result_;

  Logger log_;
  Context* ctx_;

  const char* begin_;
  const char* cur_;
  const char* end_;
  int col_idx_;
  int single_indent_length_;
  int current_indent_level_;
  TokenId cur_token_id_;

  void Parse();

  bool IsAtEOF() const;
  std::string_view GetIdentifier() const;
  size_t GetNumRemainingChars() const;

  void FinishFile();
  bool TryGetIndentation();
  bool TryGetFloatOrIntToken();
  bool TryGetStringToken();

  void FinishLine();
  void PushNothing(size_t advance);
  void PushToken(Token t, size_t advance);
  void PushAnyToken(TokenType type, size_t advance);
  void PushStringToken(std::string str, size_t advance);
  void PushIdentifierToken(std::string name, size_t advance);
  void PushIntToken(int64_t val, size_t advance);
  void PushFloatToken(double val, size_t advance);

  void ThrowSourceError(std::string message);
};

}  // namespace slip