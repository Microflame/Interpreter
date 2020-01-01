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
    : kSource(source),
      kTokens(tokens),
      log_(Logger::kDebug),
      error_(false),
      id_(1)
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
  size_t id_;

  Ptr<stmt::Stmt> ParseDeclarationOrStatement()
  {
    try
    {
      if (GetCurrentToken().GetType() == scanner::Token::VAR)
      {
        ++cur_;
        return ParseVarDeclaration();
      }
      if (GetCurrentToken().GetType() == scanner::Token::FUNC)
      {
        ++cur_;
        return ParseFuncDeclaration();
      }
      if (GetCurrentToken().GetType() == scanner::Token::CLASS)
      {
        ++cur_;
        return ParseClassDeclaration();
      }

      return ParseStmt();
    }
    catch (const std::runtime_error& e)
    {
      Synchronize();
      return nullptr;
    }

    return ParseExpressionStmt();
  }

  Ptr<stmt::Func> ParseFuncDeclaration()
  {
    ExpectToken(scanner::Token::IDENTIFIER, "identifier", false);
    Ptr<scanner::Token> name = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());

    Ptr<std::vector<Ptr<scanner::Token>>> params = std::make_shared<std::vector<Ptr<scanner::Token>>>();

    ExpectToken(scanner::Token::LEFT_PAREN, "(");
    if (GetCurrentToken().GetType() != scanner::Token::RIGHT_PAREN)
    {
      params->push_back(std::make_shared<scanner::Token>(GetCurrentToken()));
      while (GetCurrentToken().GetType() == scanner::Token::COMMA)
      {
        ++cur_;
        params->push_back(std::make_shared<scanner::Token>(GetCurrentToken()));
      }
    }
    ExpectToken(scanner::Token::RIGHT_PAREN, ")");

    ExpectToken(scanner::Token::LEFT_BRACE, "{");
    Ptr<std::vector<Ptr<stmt::Stmt>>> body = ParseBlock();

    return std::make_shared<stmt::Func>(name, params, body);
  }

  Ptr<stmt::Stmt> ParseClassDeclaration()
  {
    ExpectToken(scanner::Token::IDENTIFIER, "identifier", false);
    Ptr<scanner::Token> name = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());

    ExpectToken(scanner::Token::LEFT_BRACE, "{");

    Ptr<std::vector<Ptr<stmt::Func>>> methods = std::make_shared<std::vector<Ptr<stmt::Func>>>();
    while (GetCurrentToken().GetType() != scanner::Token::RIGHT_BRACE && Remaining())
    {
      methods->push_back(ParseFuncDeclaration());
    }

    ExpectToken(scanner::Token::RIGHT_BRACE, "}");

    return std::make_shared<stmt::Class>(name, methods);
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
    if (GetCurrentToken().GetType() == scanner::Token::IF)
    {
      ++cur_;
      return ParseIfStmt();
    }
    if (GetCurrentToken().GetType() == scanner::Token::PRINT)
    {
      ++cur_;
      return ParsePrintStmt();
    }
    if (GetCurrentToken().GetType() == scanner::Token::WHILE)
    {
      ++cur_;
      return ParseWhileStmt();
    }
    if (GetCurrentToken().GetType() == scanner::Token::LEFT_BRACE)
    {
      ++cur_;
      return ParseBlockStmt();
    }
    if (GetCurrentToken().GetType() == scanner::Token::RETURN)
    {
      return ParseReturnStmt();
    }

    return ParseExpressionStmt();
  }

  Ptr<stmt::Stmt> ParseReturnStmt()
  {
    auto tok = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());

    std::shared_ptr<Expr> value;
    if (GetCurrentToken().GetType() != scanner::Token::SEMICOLON)
    {
      value = ParseExpr();
    }

    ExpectToken(scanner::Token::SEMICOLON, ";");

    return std::make_shared<stmt::Return>(tok, value);
  }

  Ptr<stmt::Stmt> ParseWhileStmt()
  {
    ExpectToken(scanner::Token::LEFT_PAREN, "(");
    Ptr<Expr> condition = ParseExpr();
    ExpectToken(scanner::Token::RIGHT_PAREN, ")");

    Ptr<stmt::Stmt> body = ParseStmt();

    return std::make_shared<stmt::While>(condition, body);
  }

  Ptr<stmt::Stmt> ParseIfStmt()
  {
    ExpectToken(scanner::Token::LEFT_PAREN, "(");
    Ptr<Expr> condition = ParseExpr();
    ExpectToken(scanner::Token::RIGHT_PAREN, ")");

    Ptr<stmt::Stmt> stmt_true = ParseStmt();
    Ptr<stmt::Stmt> stmt_false = nullptr;
    if (GetCurrentToken().GetType() == scanner::Token::ELSE)
    {
      ++cur_;
      stmt_false = ParseStmt();
    }

    return std::make_shared<stmt::If>(condition, stmt_true, stmt_false);
  }

  Ptr<std::vector<Ptr<stmt::Stmt>>> ParseBlock()
  {
    Ptr<std::vector<Ptr<stmt::Stmt>>> statements = std::make_shared<std::vector<Ptr<stmt::Stmt>>>();

    while (GetCurrentToken().GetType() != scanner::Token::RIGHT_BRACE && Remaining())
    {
      statements->push_back(ParseDeclarationOrStatement());
    }

    ExpectToken(scanner::Token::RIGHT_BRACE, "}");

    return statements;
  }

  Ptr<stmt::Stmt> ParseBlockStmt()
  {
    return std::make_shared<stmt::Block>(ParseBlock());
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
    Ptr<Expr> expr = ParseOr();

    if (GetCurrentToken().GetType() == scanner::Token::EQUAL)
    {
      Ptr<scanner::Token> tok = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());
      Ptr<Expr> value = ParseAssign();

      if (Variable* ptr = dynamic_cast<Variable*>(expr.get()))
      {
        expr = std::make_shared<Assign>(ptr->name_, value, id_++);
      }
      else if (Get* ptr = dynamic_cast<Get*>(expr.get()))
      {
        expr = std::make_shared<Set>(ptr->object_, ptr->name_, value);
      }
      else
      {
        throw std::runtime_error("Bad assignment target.");
      }
    }

    return expr;
  }

  Ptr<Expr> ParseOr()
  {
    Ptr<Expr> expr = ParseAnd();

    while (GetCurrentToken().GetType() == scanner::Token::OR)
    {
      Ptr<scanner::Token> tok = std::make_shared<scanner::Token>(GetCurrentToken());
      ++cur_;
      Ptr<Expr> right = ParseAnd();
      expr = std::make_shared<Logical>(expr, tok, right);
    }

    return expr;
  }

  Ptr<Expr> ParseAnd()
  {
    Ptr<Expr> expr = ParseEquality();

    while (GetCurrentToken().GetType() == scanner::Token::AND)
    {
      Ptr<scanner::Token> tok = std::make_shared<scanner::Token>(GetCurrentToken());
      ++cur_;
      Ptr<Expr> right = ParseEquality();
      expr = std::make_shared<Logical>(expr, tok, right);
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



    return ParseCall();
  }

  Ptr<Expr> ParseCall()
  {
    Ptr<Expr> expr = ParsePrimary();

    while (1)
    {
      if (GetCurrentToken().GetType() == scanner::Token::LEFT_PAREN)
      {
        ++cur_;
        expr = FinishCall(expr);
      }
      else if (GetCurrentToken().GetType() == scanner::Token::DOT)
      {
        ++cur_;
        std::shared_ptr<scanner::Token> name = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());
        expr = std::make_shared<Get>(expr, name);
      }
      else
      {
        break;
      }
    }

    return expr;
  }

  Ptr<Expr> FinishCall(Ptr<Expr> callee)
  {
    Ptr<std::vector<Ptr<Expr>>> args = std::make_shared<std::vector<Ptr<Expr>>>();

    if (GetCurrentToken().GetType() != scanner::Token::RIGHT_PAREN)
    {
      args->push_back(ParseExpr());
      while (GetCurrentToken().GetType() == scanner::Token::COMMA)
      {
        ++cur_;
        args->push_back(ParseExpr());
      }
    }

    const scanner::Token& tok = ExpectToken(scanner::Token::RIGHT_PAREN, ")");
    auto tok_ptr = std::make_shared<scanner::Token>(tok);


    return std::make_shared<Call>(callee, tok_ptr, args);
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
      return std::make_shared<Variable>(std::make_shared<scanner::Token>(op), id_++);
    }

    ExpectToken(scanner::Token::LEFT_PAREN, "expression");
    Ptr<Expr> expr = ParseExpr();
    ExpectToken(scanner::Token::RIGHT_PAREN, ")");
    return std::make_shared<Grouping>(expr);
  }

  const scanner::Token& ExpectToken(scanner::Token::Type type, const char* name, bool incremet = true)
  {
    const scanner::Token& tok = GetCurrentToken();
    if (tok.GetType() != type)
    {
      error_ = true;
      auto pos = tok.GetPosition(kSource);
      log_(Logger::kError, "[PARSER]:%d:%d: Expected \'%s\' before %s",
           pos.first,
           pos.second,
           name,
           tok.ToRawString().c_str());
      throw std::runtime_error("Unexpected token.");
    }
    cur_ += incremet;
    return tok;
  }

};

} // parser