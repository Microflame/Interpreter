#pragma once

#include <vector>

#include "scanner/token.h"
#include "logger.h"
#include "expr.h"
#include "stmt.h"
#include "util/string_tools.h"

namespace parser
{

class Parser
{
public:
  Parser(const std::string& source, const std::vector<scanner::Token>& tokens)
    : kSource(source), kTokens(tokens), log_(Logger::kDebug), error_(false)
  {}


  std::vector<Ptr<stmt::Stmt>> Parse()
  {
    cur_ = 0;
    std::vector<Ptr<stmt::Stmt>> statements;

    while (Remaining())
    {
      statements.push_back(ParseDeclarationOrStatement());
    }

    return statements;
  }

  bool HasError() { return error_; }

private:
  const std::string& kSource;
  const std::vector<scanner::Token>& kTokens;
  size_t cur_;
  Logger log_;
  bool error_;

  Ptr<stmt::Stmt> ParseDeclarationOrStatement()
  {
    try
    {
      if (GetCurrentToken().GetType() == scanner::Token::VAR)
      {
        ++cur_;
        return ParseVarDeclaration();
      }

      return ParseStmt();
    }
    catch (const std::runtime_error& e)
    {
      Synchronize();
      return nullptr;
    }
    if (GetCurrentToken().GetType() == scanner::Token::VAR)
    {
      ++cur_;
      return ParsePrintStmt();
    }

    return ParseExpressionStmt();
  }

  Ptr<stmt::Stmt> ParseVarDeclaration()
  {
    ExpectToken(scanner::Token::IDENTIFIER, "identifier", false);
    Ptr<scanner::Token> name = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());

    Ptr<Expr> expr = nullptr;
    if (GetCurrentToken().GetType() == scanner::Token::EQUAL)
    {
      ++cur_;
      expr = ParseExpr();
    }

    ExpectToken(scanner::Token::SEMICOLON, ";");

    return std::make_shared<stmt::Var>(name, expr);
  }

  Ptr<stmt::Stmt> ParseStmt()
  {
    if (GetCurrentToken().GetType() == scanner::Token::PRINT)
    {
      ++cur_;
      return ParsePrintStmt();
    }
    if (GetCurrentToken().GetType() == scanner::Token::LEFT_BRACE)
    {
      ++cur_;
      return ParseBlockStmt();
    }

    return ParseExpressionStmt();
  }

  Ptr<stmt::Stmt> ParseBlockStmt()
  {
    Ptr<std::vector<Ptr<stmt::Stmt>>> statements = std::make_shared<std::vector<Ptr<stmt::Stmt>>>();

    while (GetCurrentToken().GetType() != scanner::Token::RIGHT_BRACE && Remaining())
    {
      statements->push_back(ParseDeclarationOrStatement());
    }

    ExpectToken(scanner::Token::RIGHT_BRACE, "}");
    return std::make_shared<stmt::Block>(statements);
  }

  Ptr<stmt::Stmt> ParsePrintStmt()
  {
    Ptr<Expr> expr = ParseExpr();

    ExpectToken(scanner::Token::SEMICOLON, ";");
    return std::make_shared<stmt::Print>(expr);
  }

  Ptr<stmt::Stmt> ParseExpressionStmt()
  {
    Ptr<Expr> expr = ParseExpr();

    ExpectToken(scanner::Token::SEMICOLON, ";");
    return std::make_shared<stmt::Expression>(expr);
  }

  bool Remaining()
  {
    return GetCurrentToken().GetType() != scanner::Token::END_OF_FILE;
  }

  bool Synchronize()
  {
    while (Remaining())
    {
      if (GetCurrentToken().GetType() == scanner::Token::SEMICOLON)
      {
        ++cur_;
        return true;
      }
      if (GetCurrentToken().OneOf(scanner::Token::CLASS,
                                  scanner::Token::FUNC,
                                  scanner::Token::VAR,
                                  scanner::Token::FOR,
                                  scanner::Token::IF,
                                  scanner::Token::WHILE,
                                  scanner::Token::RETURN))
      {
        return true;
      }
      ++cur_;
    }
    return false;
  }

  const scanner::Token& GetCurrentToken() const { return kTokens[cur_]; }

  const scanner::Token& GetCurrentTokenAndIncremetIterator() { return kTokens[cur_++]; }

  Ptr<Expr> ParseExpr()
  {
    try
    {
      return ParseAssign();
    }
    catch (std::runtime_error& e)
    {
      return {};
    }
  }

  Ptr<Expr> ParseAssign()
  {
    Ptr<Expr> expr = ParseEquality();

    if (GetCurrentToken().GetType() == scanner::Token::EQUAL)
    {
      Ptr<scanner::Token> tok = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());
      Ptr<Expr> value = ParseAssign();

      Variable* var = dynamic_cast<Variable*>(expr.get());

      if (var == nullptr)
      {
        throw std::runtime_error("Bad assignment target.");
      }

      expr = std::make_shared<Assign>(var->name_, value);
    }

    return expr;
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
                                   scanner::Token::GREATER_EQUAL))
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

    if (GetCurrentToken().GetType() == scanner::Token::IDENTIFIER)
    {
      const scanner::Token& op = GetCurrentTokenAndIncremetIterator();
      return std::make_shared<Variable>(std::make_shared<scanner::Token>(op));
    }

    ExpectToken(scanner::Token::LEFT_PAREN, "expression");
    Ptr<Expr> expr = ParseExpr();
    ExpectToken(scanner::Token::RIGHT_PAREN, ")");
    return std::make_shared<Grouping>(expr);
  }

  void ExpectToken(scanner::Token::Type type, const char* name, bool incremet = true)
  {
    if (GetCurrentToken().GetType() != type)
    {
      error_ = true;
      auto pos = GetCurrentToken().GetPosition(kSource);
      log_(Logger::kError, "[PARSER]:%d:%d: Expected \'%s\' before %s",
           pos.first,
           pos.second,
           name,
           GetCurrentToken().ToRawString().c_str());
      throw std::runtime_error("Unexpected token.");
    }
    cur_ += incremet;
  }
};

} // parser