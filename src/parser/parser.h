#pragma once



#include "scanner/token.h"
#include "logger.h"
#include "expr.h"

namespace parser
{

class Parser
{
public:
  Parser(const std::vector<scanner::Token>& tokens)
    : kTokens(tokens), log_(Logger::kDebug)
  {}


  Ptr<Expr> Parse()
  {
    cur_ = 0;
    return ParseExpr();
  }

private:
  const std::vector<scanner::Token>& kTokens;
  size_t cur_;
  Logger log_;


  const scanner::Token& GetCurrentToken() const { return kTokens[cur_]; }

  const scanner::Token& GetCurrentTokenAndIncremetIterator() { return kTokens[cur_++]; }

  Ptr<Expr> ParseExpr()
  {
    return ParseEquality();
  }

  Ptr<Expr> ParseEquality()
  {
    Ptr<Expr> expr = ParseComparison();

    while (GetCurrentToken().OneOf(scanner::Token::EQUAL_EQUAL, scanner::Token::BANG_EQUAL))
    {
      const scanner::Token& op = GetCurrentTokenAndIncremetIterator();
      Ptr<Expr> right = ParseComparison();
      expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
    }

    return expr;
  }

  Ptr<Expr> ParseComparison()
  {
    Ptr<Expr> expr = ParseAddition();

    while (GetCurrentToken().OneOf(scanner::Token::LESS,
                                   scanner::Token::LESS_EQUAL,
                                   scanner::Token::GREATER,
                                   scanner::Token::GREATER))
    {
      const scanner::Token& op = GetCurrentTokenAndIncremetIterator();
      Ptr<Expr> right = ParseAddition();
      expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
    }

    return expr;
  }

  Ptr<Expr> ParseAddition()
  {
    Ptr<Expr> expr = ParseMultiplication();

    while (GetCurrentToken().OneOf(scanner::Token::MINUS, scanner::Token::PLUS))
    {
      const scanner::Token& op = GetCurrentTokenAndIncremetIterator();
      Ptr<Expr> right = ParseMultiplication();
      expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
    }

    return expr;
  }

  Ptr<Expr> ParseMultiplication()
  {
    Ptr<Expr> expr = ParseUnary();

    while (GetCurrentToken().OneOf(scanner::Token::STAR, scanner::Token::SLASH))
    {
      const scanner::Token& op = GetCurrentTokenAndIncremetIterator();
      Ptr<Expr> right = ParseUnary();
      expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
    }

    return expr;
  }

  Ptr<Expr> ParseUnary()
  {
    if (GetCurrentToken().OneOf(scanner::Token::BANG, scanner::Token::MINUS))
    {
      const scanner::Token& op = GetCurrentTokenAndIncremetIterator();
      Ptr<Expr> right = ParseUnary();
      return std::make_shared<Unary>(std::make_shared<scanner::Token>(op), right);
    }

    return ParsePrimary();
  }

  Ptr<Expr> ParsePrimary()
  {
    if (GetCurrentToken().OneOf(scanner::Token::FALSE,
                                scanner::Token::TRUE,
                                scanner::Token::NONE,
                                scanner::Token::STRING,
                                scanner::Token::INT_LITERAL,
                                scanner::Token::FLOAT_LITERAL))
    {
      const scanner::Token& op = GetCurrentTokenAndIncremetIterator();
      return std::make_shared<Literal>(std::make_shared<scanner::Token>(op));
    }

    if (GetCurrentToken().GetType() == scanner::Token::LEFT_PAREN)
    {
      ++cur_;
      Ptr<Expr> expr = ParseExpr();
      if (GetCurrentToken().GetType() != scanner::Token::RIGHT_PAREN)
      {
        log_(Logger::kError, "Expected \')\' before %s", GetCurrentToken().ToRawString().c_str());
      }
      ++cur_;
      return std::make_shared<Grouping>(expr);
    }

    log_(Logger::kError, "\'(\' or literal expected.");
    throw std::runtime_error("\'(\' or literal expected.");
  }


};

} // parser