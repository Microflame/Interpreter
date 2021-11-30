#pragma once

#include <vector>

#include "scanner/token.h"
#include "logger.h"
#include "expr.h"
#include "stmt.h"
#include "util/string_tools.h"

namespace ilang
{


class Parser
{
public:
  Parser(const std::string& source, const std::vector<Token>& tokens, ExprStmtPool* espool)
    : kSource(source),
      kTokens(tokens),
      expr_stmt_pool_(*espool),
      log_(Logger::kDebug),
      error_(false),
      id_(1)
  {}


  std::vector<StmtId> Parse()
  {
    cur_ = 0;
    std::vector<StmtId> statements;

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
  ExprStmtPool& expr_stmt_pool_;
  size_t cur_;
  Logger log_;
  bool error_;
  size_t id_;

  StmtId AddStmt(Stmt stmt)
  {
    // std::cout << "STMT: " << StmtTypeToString(stmt.type_) << std::endl;
    return expr_stmt_pool_.PushStmt(stmt);
  }

  StmtId AddDefStmt()
  {
    Stmt stmt = {Stmt::DEF};
    stmt.def_ = {.name_=0, .params_=0, .body_=0};
    return AddStmt(stmt);
  }

  StmtId AddReturnStmt()
  {
    Stmt stmt = {Stmt::RETURN};
    stmt.return_ = {};
    return AddStmt(stmt);
  }

  StmtId AddWhileStmt()
  {
    Stmt stmt = {Stmt::WHILE};
    stmt.while_ = {};
    return AddStmt(stmt);
  }

  StmtId AddIfStmt()
  {
    Stmt stmt = {Stmt::IF};
    stmt.if_ = {};
    return AddStmt(stmt);
  }

  StmtId AddBlockStmt()
  {
    Stmt stmt = {Stmt::BLOCK};
    stmt.block_ = {};
    return AddStmt(stmt);
  }

  StmtId AddExpressionStmt()
  {
    Stmt stmt = {Stmt::EXPRESSION};
    stmt.expression_ = {};
    return AddStmt(stmt);
  }


  ExprId AddExpr(Expr expr)
  {
    // std::cout << "EXPR: " << ExprTypeToString(expr.type_) << '\n';
    return expr_stmt_pool_.PushExpr(expr);
  }

  ExprId AddAssignExpr()
  {
    Expr expr = {Expr::ASSIGN};
    return AddExpr(expr);
  }

  ExprId AddSetExpr()
  {
    Expr expr = {Expr::SET};
    return AddExpr(expr);
  }

  ExprId AddLogicalExpr()
  {
    Expr expr = {Expr::LOGICAL};
    return AddExpr(expr);
  }

  ExprId AddBinaryExpr()
  {
    Expr expr = {Expr::BINARY};
    return AddExpr(expr);
  }

  ExprId AddUnaryExpr()
  {
    Expr expr = {Expr::UNARY};
    return AddExpr(expr);
  }

  ExprId AddGetExpr()
  {
    Expr expr = {Expr::GET};
    return AddExpr(expr);
  }

  ExprId AddCallExpr()
  {
    Expr expr = {Expr::CALL};
    return AddExpr(expr);
  }

  ExprId AddLiteralExpr()
  {
    Expr expr = {Expr::LITERAL};
    return AddExpr(expr);
  }

  ExprId AddThisExpr()
  {
    Expr expr = {Expr::THIS};
    return AddExpr(expr);
  }

  ExprId AddVariableExpr()
  {
    Expr expr = {Expr::VARIABLE};
    return AddExpr(expr);
  }

  StmtId ParseDeclarationOrStatement()
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
        return -1;
        //return ParseClassDeclaration();
      }

      return ParseStmt();
    }
    catch (const std::runtime_error& e)
    {
      Synchronize();
      return -1;
    }

    return ParseExpressionStmt();
  }

  StmtId ParseFunction()
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

  StmtId ParseStmt()
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

  StmtId ParseReturnStmt()
  {
    ExprId value;
    if (!IsAtEndStatement())
    {
      value = ParseExpr();
    }

    ConsumeEndStatement();

    return AddReturnStmt();
  }

  StmtId ParseWhileStmt()
  {
    ExprId condition = ParseExpr();
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    StmtId body = ParseBlockStmt();

    return AddWhileStmt();
  }

  StmtId ParseIfStmt()
  {
    ExprId condition = ParseExpr();
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    StmtId stmt_true = ParseBlockStmt();
    StmtId stmt_false = -1;
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
    StmtBlockId block_id = expr_stmt_pool_.MakeNewStmtBlock();

    while (GetCurrentTokenType() != TokenType::UNINDENT && Remaining())
    {
      Stmt stmt = expr_stmt_pool_.statements_[ParseDeclarationOrStatement()];
      expr_stmt_pool_.stmt_blocks_[block_id].push_back(stmt);
    }

    ConsumeToken(TokenType::UNINDENT);
    return block_id;
  }

  StmtId ParseBlockStmt()
  {
    StmtBlockId block_id = ParseBlock();
    return AddBlockStmt();
  }

  StmtId ParseExpressionStmt()
  {
    ExprId expr = ParseExpr();

    ConsumeEndStatement();
    return AddExpressionStmt();
  }


  ExprId ParseExpr()
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

  ExprId ParseAssign()
  {
    ExprId lvalue_id = ParseOr();

    if (GetCurrentTokenType() == TokenType::EQUAL)
    {
      Token tok = GetCurrentTokenAndIncremetIterator();
      ExprId value = ParseAssign();
      Expr::Type lvalue_type = expr_stmt_pool_.expressions_[lvalue_id].type_;

      if (lvalue_type == Expr::VARIABLE)
      {
        // expr = std::make_shared<Assign>(variable.name_, value, id_++);
        return AddAssignExpr();
      }
      else if (lvalue_type == Expr::GET)
      {
        // expr = std::make_shared<Set>(get.object_, ptr->name_, value);
        return AddSetExpr();
      }
      else
      {
        throw std::runtime_error("Bad assignment target.");
      }
    }

    return lvalue_id;
  }

  ExprId ParseOr()
  {
    ExprId expr = ParseAnd();

    while (GetCurrentTokenType() == TokenType::OR)
    {
      Token or_token = GetCurrentTokenAndIncremetIterator();
      ExprId right = ParseAnd();
      // expr = std::make_shared<Logical>(expr, tok, right);
      return AddLogicalExpr();
    }

    return expr;
  }

  ExprId ParseAnd()
  {
    ExprId expr = ParseEquality();

    while (GetCurrentTokenType() == TokenType::AND)
    {
      Token and_token = GetCurrentTokenAndIncremetIterator();
      ExprId right = ParseAnd();
      // expr = std::make_shared<Logical>(expr, tok, right);
      return AddLogicalExpr();
    }

    return expr;
  }

  ExprId ParseEquality()
  {
    ExprId expr = ParseComparison();

    while (IsAtEqualityCheck())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      ExprId right = ParseComparison();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  ExprId ParseComparison()
  {
    ExprId expr = ParseAddition();

    while (IsAtComparison())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      ExprId right = ParseAddition();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  ExprId ParseAddition()
  {
    ExprId expr = ParseMultiplication();

    while (IsAtAddSub())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      ExprId right = ParseMultiplication();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  ExprId ParseMultiplication()
  {
    ExprId expr = ParseUnary();

    while (IsAtMulDiv())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      ExprId right = ParseUnary();
      // expr = std::make_shared<Binary>(expr, std::make_shared<scanner::Token>(op), right);
      return AddBinaryExpr();
    }

    return expr;
  }

  ExprId ParseUnary()
  {
    if (IsAtUnary())
    {
      Token op = GetCurrentTokenAndIncremetIterator();
      ExprId right = ParseUnary();
      // return std::make_shared<Unary>(std::make_shared<scanner::Token>(op), right);
      return AddUnaryExpr();
    }

    return ParseCall();
  }

  ExprId ParseCall()
  {
    ExprId expr = ParsePrimary();

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

  ExprId FinishCall(ExprId callee)
  {
    ExprBlockId block_id = expr_stmt_pool_.MakeNewExprBlock();

    if (GetCurrentTokenType() != TokenType::RIGHT_PAREN)
    {
      ExprId expr_id = ParseExpr();
      Expr expr = expr_stmt_pool_.expressions_[expr_id];
      expr_stmt_pool_.expr_blocks_[block_id].push_back(expr);
      while (GetCurrentTokenType() == TokenType::COMMA)
      {
        ++cur_;
        ExprId expr_id = ParseExpr();
        Expr expr = expr_stmt_pool_.expressions_[expr_id];
        expr_stmt_pool_.expr_blocks_[block_id].push_back(expr);
      }
    }

    Token tok = ConsumeToken(TokenType::RIGHT_PAREN);

    // return std::make_shared<Call>(callee, tok_ptr, args);
    return AddCallExpr();
  }

  ExprId ParsePrimary()
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
    // ExprId expr = ParseExpr();
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