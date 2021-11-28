#pragma once

#include <vector>

#include "scanner/token.h"
#include "logger.h"
#include "expr.h"
#include "stmt.h"
#include "util/string_tools.h"

namespace ilang
{

using StmtBlockId = int32_t;

class Parser
{
public:
  Parser(const std::string& source, const std::vector<Token>& tokens)
    : kSource(source),
      kTokens(tokens),
      log_(Logger::kDebug),
      error_(false),
      id_(1)
  {}


  std::vector<const Stmt*> Parse()
  {
    cur_ = 0;
    std::vector<const Stmt*> statements;

    while (1)
    {
      while (IsAtEndStatement())
        ++cur_;
      if (!Remaining())
        break;
      statements.push_back(ParseDeclarationOrStatement());
    }

    return statements;
  }

  bool HasError() { return error_; }

private:
  const std::string& kSource;
  const std::vector<Token>& kTokens;
  std::vector<Stmt> statements_;
  std::vector<Expr> expressions_;
  std::vector<std::vector<const Stmt*>> stmt_blocks_;
  size_t cur_;
  Logger log_;
  bool error_;
  size_t id_;

  const Stmt* AddStmt(Stmt stmt)
  {
    // std::cout << "STMT: " << StmtTypeToString(stmt.type_) << std::endl;
    statements_.push_back(stmt);
    return &statements_.back();
  }

  const Stmt* AddDefStmt()
  {
    Stmt stmt = {Stmt::DEF};
    stmt.def_ = {};
    return AddStmt(stmt);
  }

  const Stmt* AddReturnStmt()
  {
    Stmt stmt = {Stmt::RETURN};
    stmt.return_ = {};
    return AddStmt(stmt);
  }

  const Stmt* AddWhileStmt()
  {
    Stmt stmt = {Stmt::WHILE};
    stmt.while_ = {};
    return AddStmt(stmt);
  }

  const Stmt* AddIfStmt()
  {
    Stmt stmt = {Stmt::IF};
    stmt.if_ = {};
    return AddStmt(stmt);
  }

  const Stmt* AddBlockStmt()
  {
    Stmt stmt = {Stmt::BLOCK};
    stmt.block_ = {};
    return AddStmt(stmt);
  }

  const Stmt* AddExpressionStmt()
  {
    Stmt stmt = {Stmt::EXPRESSION};
    stmt.expression_ = {};
    return AddStmt(stmt);
  }


  const Expr* AddExpr(Expr expr)
  {
    // std::cout << "EXPR: " << ExprTypeToString(expr.type_) << '\n';
    expressions_.push_back(expr);
    return &expressions_.back();
  }

  const Expr* AddAssignExpr()
  {
    Expr expr = {Expr::ASSIGN};
    expr.assign_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddSetExpr()
  {
    Expr expr = {Expr::SET};
    expr.set_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddLogicalExpr()
  {
    Expr expr = {Expr::LOGICAL};
    expr.logical_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddBinaryExpr()
  {
    Expr expr = {Expr::BINARY};
    expr.binary_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddUnaryExpr()
  {
    Expr expr = {Expr::UNARY};
    expr.unary_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddGetExpr()
  {
    Expr expr = {Expr::GET};
    expr.get_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddCallExpr()
  {
    Expr expr = {Expr::CALL};
    expr.call_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddLiteralExpr()
  {
    Expr expr = {Expr::LITERAL};
    expr.literal_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddThisExpr()
  {
    Expr expr = {Expr::THIS};
    expr.this_ = {{-1}};
    return AddExpr(expr);
  }

  const Expr* AddVariableExpr()
  {
    Expr expr = {Expr::VARIABLE};
    expr.variable_ = {{-1}};
    return AddExpr(expr);
  }

  const Stmt* ParseDeclarationOrStatement()
  {
    while (IsAtEndStatement())
      ++cur_;

    try
    {
      if (GetCurrentTokenType() == TokenType::DEF)
      {
        ++cur_;
        return ParseFunction();
      }
      if (GetCurrentTokenType() == TokenType::CLASS)
      {
        ++cur_;
        return nullptr;
        //return ParseClassDeclaration();
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

  const Stmt* ParseFunction()
  {
    Token name = ConsumeToken(TokenType::IDENTIFIER);

    std::vector<Token> params;

    ConsumeToken(TokenType::LEFT_PAREN);
    if (GetCurrentTokenType() != TokenType::RIGHT_PAREN)
    {
      params.push_back(GetCurrentTokenAndIncremetIterator());
      while (GetCurrentTokenType() == TokenType::COMMA)
      {
        ++cur_;
        params.push_back(GetCurrentTokenAndIncremetIterator());
      }
    }
    ConsumeToken(TokenType::RIGHT_PAREN);
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    StmtBlockId body = ParseBlock();

    return AddDefStmt();
  }

  // Ptr<stmt::Stmt> ParseClassDeclaration()
  // {
  //   ExpectToken(TokenType::IDENTIFIER, "identifier", false);
  //   Ptr<scanner::Token> name = std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator());

  //   Ptr<Variable> super;
  //   if (GetCurrentTokenType() == TokenType::COLON)
  //   {
  //     ++cur_;
  //     ExpectToken(TokenType::IDENTIFIER, "identifier", false);
  //     super = std::make_shared<Variable>(std::make_shared<scanner::Token>(GetCurrentTokenAndIncremetIterator()), id_++);
  //   }

  //   ExpectToken(TokenType::LEFT_BRACE, "{");

  //   Ptr<std::vector<Ptr<stmt::Func>>> methods = std::make_shared<std::vector<Ptr<stmt::Func>>>();
  //   while (GetCurrentTokenType() != TokenType::RIGHT_BRACE && Remaining())
  //   {
  //     methods->push_back(ParseFuncDeclaration());
  //   }

  //   ExpectToken(TokenType::RIGHT_BRACE, "}");

  //   return std::make_shared<stmt::Class>(name, super, methods);
  // }

  const Stmt* ParseStmt()
  {
    switch (GetCurrentTokenType())
    {
      case TokenType::IF:     ++cur_; return ParseIfStmt();
      case TokenType::WHILE:  ++cur_; return ParseWhileStmt();
      case TokenType::INDENT: ++cur_; return ParseBlockStmt();
      case TokenType::RETURN: ++cur_; return ParseReturnStmt();
      default: break;
    }

    return ParseExpressionStmt();
  }

  const Stmt* ParseReturnStmt()
  {
    const Expr* value;
    if (!IsAtEndStatement())
    {
      value = ParseExpr();
    }

    ConsumeEndStatement();

    return AddReturnStmt();
  }

  const Stmt* ParseWhileStmt()
  {
    const Expr* condition = ParseExpr();
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    const Stmt* body = ParseBlockStmt();

    return AddWhileStmt();
  }

  const Stmt* ParseIfStmt()
  {
    const Expr* condition = ParseExpr();
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    const Stmt* stmt_true = ParseBlockStmt();
    const Stmt* stmt_false = nullptr;
    if (GetCurrentTokenType() == TokenType::ELSE)
    {
      ++cur_;
      ConsumeToken(TokenType::COLON);
      ConsumeToken(TokenType::NEWLINE);
      ConsumeToken(TokenType::INDENT);
      stmt_false = ParseBlockStmt();
    }

    return AddIfStmt();
  }

  StmtBlockId ParseBlock()
  {
    StmtBlockId block_id = stmt_blocks_.size();
    stmt_blocks_.emplace_back();

    while (GetCurrentTokenType() != TokenType::UNINDENT && Remaining())
    {
      const Stmt* stmt = ParseDeclarationOrStatement();
      stmt_blocks_[block_id].push_back(stmt);
    }

    ConsumeToken(TokenType::UNINDENT);
    return block_id;
  }

  const Stmt* ParseBlockStmt()
  {
    StmtBlockId block_id = ParseBlock();
    return AddBlockStmt();
  }

  const Stmt* ParseExpressionStmt()
  {
    const Expr* expr = ParseExpr();

    ConsumeEndStatement();
    return AddExpressionStmt();
  }


  const Expr* ParseExpr()
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

  const Expr* ParseAssign()
  {
    const Expr* expr = ParseOr();

    if (GetCurrentTokenType() == TokenType::EQUAL)
    {
      Token tok = GetCurrentTokenAndIncremetIterator();
      const Expr* value = ParseAssign();

      if (expr->type_ == Expr::VARIABLE)
      {
        // expr = std::make_shared<Assign>(variable.name_, value, id_++);
        return AddAssignExpr();
      }
      else if (expr->type_ == Expr::GET)
      {
        // expr = std::make_shared<Set>(get.object_, ptr->name_, value);
        return AddSetExpr();
      }
      else
      {
        throw std::runtime_error("Bad assignment target.");
      }
    }

    return expr;
  }

  const Expr* ParseOr()
  {
    const Expr* expr = ParseAnd();

    while (GetCurrentTokenType() == TokenType::OR)
    {
      Token or_token = GetCurrentTokenAndIncremetIterator();
      const Expr* right = ParseAnd();
      // expr = std::make_shared<Logical>(expr, tok, right);
      return AddLogicalExpr();
    }

    return expr;
  }

  const Expr* ParseAnd()
  {
    const Expr* expr = ParseEquality();

    while (GetCurrentTokenType() == TokenType::AND)
    {
      Token and_token = GetCurrentTokenAndIncremetIterator();
      const Expr* right = ParseAnd();
      // expr = std::make_shared<Logical>(expr, tok, right);
      return AddLogicalExpr();
    }

    return expr;
  }

  const Expr* ParseEquality()
  {
    const Expr* expr = ParseComparison();

    while (IsAtEqualityCheck())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      const Expr* right = ParseComparison();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  const Expr* ParseComparison()
  {
    const Expr* expr = ParseAddition();

    while (IsAtComparison())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      const Expr* right = ParseAddition();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  const Expr* ParseAddition()
  {
    const Expr* expr = ParseMultiplication();

    while (IsAtAddSub())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      const Expr* right = ParseMultiplication();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  const Expr* ParseMultiplication()
  {
    const Expr* expr = ParseUnary();

    while (IsAtMulDiv())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      const Expr* right = ParseUnary();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  const Expr* ParseUnary()
  {
    if (IsAtUnary())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      const Expr* right = ParseUnary();
      // return std::make_shared<Unary>(std::make_shared<scanner::Token>(op), right);
      return AddUnaryExpr();
    }

    return ParseCall();
  }

  const Expr* ParseCall()
  {
    const Expr* expr = ParsePrimary();

    while (1)
    {
      if (GetCurrentTokenType() == TokenType::LEFT_PAREN)
      {
        ++cur_;
        expr = FinishCall(expr);
      }
      else if (GetCurrentTokenType() == TokenType::DOT)
      {
        ++cur_;
        Token name = GetCurrentTokenAndIncremetIterator();
        // expr = std::make_shared<Get>(expr, name);
        expr = AddGetExpr();
      }
      else
      {
        break;
      }
    }

    return expr;
  }

  const Expr* FinishCall(const Expr* callee)
  {
    std::vector<const Expr*> args;

    if (GetCurrentTokenType() != TokenType::RIGHT_PAREN)
    {
      args.push_back(ParseExpr());
      while (GetCurrentTokenType() == TokenType::COMMA)
      {
        ++cur_;
        args.push_back(ParseExpr());
      }
    }

    Token tok = ConsumeToken(TokenType::RIGHT_PAREN);

    // return std::make_shared<Call>(callee, tok_ptr, args);
    return AddCallExpr();
  }

  const Expr* ParsePrimary()
  {
    if (IsAtLiteral())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      return AddLiteralExpr();
    }

    if (GetCurrentTokenType() == TokenType::THIS)
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      return AddThisExpr();
      // return std::make_shared<This>(std::make_shared<scanner::Token>(op), id_++);
    }

    // if (GetCurrentTokenType() == TokenType::SUPER)
    // {
    //   const scanner::Token& name = GetCurrentTokenAndIncremetIterator();
    //   ExpectToken(TokenType::DOT, ".");
    //   const scanner::Token& method = ExpectToken(TokenType::IDENTIFIER, "method name");
    //   return std::make_shared<Super>(std::make_shared<scanner::Token>(name),
    //                                  std::make_shared<scanner::Token>(method),
    //                                  id_++);
    // }

    Token op = ConsumeToken(TokenType::IDENTIFIER);
      // return std::make_shared<Variable>(std::make_shared<scanner::Token>(op), id_++);
    return AddVariableExpr();

    // ExpectToken(TokenType::LEFT_PAREN, "expression");
    // const Expr* expr = ParseExpr();
    // ExpectToken(TokenType::RIGHT_PAREN, ")");
    // return std::make_shared<Grouping>(expr);
  }

  Token GetCurrentToken() const { return kTokens[cur_]; }

  TokenType GetCurrentTokenType() const { return kTokens[cur_].meta_.type_; }

  Token GetCurrentTokenAndIncremetIterator() { return kTokens[cur_++]; }

  Token AssertToken(TokenType type)
  {
    return _ExpectToken(type, false);
  }

  Token ConsumeToken(TokenType type)
  {
    return _ExpectToken(type, true);
  }

  Token ConsumeEndStatement()
  {
    Token cur_token = GetCurrentToken();
    TokenType cur_type = cur_token.meta_.type_;
    if (cur_type != TokenType::NEWLINE && cur_type != TokenType::SEMICOLON)
    {
      error_ = true;
      // auto pos = tok.GetPosition(kSource);
      auto pos = std::pair<int, int>{0,0};
      log_(Logger::kError, "[PARSER]:%d:%d: Unexpected %s",
           pos.first,
           pos.second,
           GetTokenTypeName(cur_type));
      throw std::runtime_error("Unexpected token.");
    }
    cur_ += 1;
    return cur_token;
  }

  Token _ExpectToken(TokenType type, bool incremet = true)
  {
    Token cur_token = GetCurrentToken();
    TokenType cur_type = cur_token.meta_.type_;
    if (cur_type != type)
    {
      error_ = true;
      // auto pos = tok.GetPosition(kSource);
      auto pos = std::pair<int, int>{0,0};
      log_(Logger::kError, "[PARSER]:%d:%d: Expected \'%s\' before %s",
           pos.first,
           pos.second,
           GetTokenTypeName(type),
           GetTokenTypeName(cur_type));
      throw std::runtime_error("Unexpected token.");
    }
    cur_ += incremet;
    return cur_token;
  }

  bool Remaining()
  {
    return GetCurrentTokenType() != TokenType::END_OF_FILE;
  }

  bool Synchronize()
  {
    while (Remaining())
    {
      if (IsAtEndStatement())
      {
        ++cur_;
        return true;
      }
      if (IsAtSynchronizePivot())
      {
        return true;
      }
      ++cur_;
    }
    return false;
  }

  bool IsAtEndStatement()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::SEMICOLON || type == TokenType::NEWLINE;
  }

  bool IsAtSynchronizePivot()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::CLASS ||
           type == TokenType::DEF   ||
           type == TokenType::FOR   ||
           type == TokenType::WHILE ||
           type == TokenType::IF    ||
           type == TokenType::RETURN;
  }

  bool IsAtEqualityCheck()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::BANG_EQUAL  ||
           type == TokenType::EQUAL_EQUAL;
  }

  bool IsAtComparison()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::LESS          ||
           type == TokenType::LESS_EQUAL    ||
           type == TokenType::GREATER       ||
           type == TokenType::GREATER_EQUAL;
  }

  bool IsAtAddSub()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::PLUS  ||
           type == TokenType::MINUS;
  }

  bool IsAtMulDiv()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::STAR  ||
           type == TokenType::SLASH;
  }

  bool IsAtUnary()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::MINUS ||
           type == TokenType::NOT;
  }

  bool IsAtLiteral()
  {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::FALSE       ||
           type == TokenType::TRUE        ||
           type == TokenType::NONE        ||
           type == TokenType::STRING      ||
           type == TokenType::INT_LITERAL ||
           type == TokenType::FLOAT_LITERAL;
  }

};

} // ilang