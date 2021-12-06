#pragma once

#include <vector>

#include "expr.h"
#include "expr_stmt_pool.h"
#include "stmt.h"
#include "token.h"
#include "util/logger.h"
#include "util/string_tools.h"

namespace ilang {

class Parser {
 public:
  Parser(const std::string& source, const std::vector<Token>& tokens,
         ExprStmtPool* espool)
      : kSource(source),
        kTokens(tokens),
        expr_stmt_pool_(*espool),
        cur_(0),
        log_(Logger::kDebug),
        error_(false),
        resolve_id_(-1) {}

  std::vector<StmtId> Parse() {
    std::vector<StmtId> statements;

    while (1) {
      while (IsAtEndStatement()) {
        Advance();
      }
      if (!Remaining()) {
        break;
      }
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
  ResolveId resolve_id_;

  StmtId AddStmt(Stmt stmt) {
    // std::cout << "STMT: " << StmtTypeToString(stmt.type_) << std::endl;
    return expr_stmt_pool_.PushStmt(stmt);
  }

  template <typename T>
  StmtId AddStmt(T stmt_data) {
    return AddStmt({stmt_data.TYPE, stmt_data});
  }

  StmtId AddDefStmt(StrId name, StrBlockId params, StmtBlockId body) {
    Stmt stmt = {Stmt::DEF};
    stmt.def_ = {.name_ = name, .params_ = params, .body_ = body};
    return AddStmt(stmt);
  }

  StmtId AddReturnStmt(ExprId value) {
    Stmt stmt = {Stmt::RETURN};
    stmt.return_ = {.value_ = value};
    return AddStmt(stmt);
  }

  StmtId AddWhileStmt(ExprId condition, StmtId body) {
    Stmt stmt = {Stmt::WHILE};
    stmt.while_ = {.condition_ = condition, .body_ = body};
    return AddStmt(stmt);
  }

  StmtId AddIfStmt(ExprId condition, StmtId true_branch, StmtId false_branch) {
    Stmt stmt = {Stmt::IF};
    stmt.if_ = {.condition_ = condition,
                .true_branch_ = true_branch,
                .false_branch_ = false_branch};
    return AddStmt(stmt);
  }

  StmtId AddBlockStmt(StmtBlockId statements) {
    Stmt stmt = {Stmt::BLOCK};
    stmt.block_ = {.statements_ = statements};
    return AddStmt(stmt);
  }

  StmtId AddExpressionStmt(ExprId expr) {
    Stmt stmt = {Stmt::EXPRESSION};
    stmt.expression_ = {.expr_ = expr};
    return AddStmt(stmt);
  }

  ExprId AddExpr(Expr expr) {
    // std::cout << "EXPR: " << ExprTypeToString(expr.type_) << '\n';
    return expr_stmt_pool_.PushExpr(expr);
  }

  ExprId AddAssignExpr(ResolveId id, ExprId value, StrId name) {
    Expr expr = {Expr::ASSIGN};
    expr.assign_ = {.id_ = id, .value_ = value, .name_ = name};
    return AddExpr(expr);
  }

  ExprId AddSetExpr(ExprId object, ExprId value, StrId name) {
    Expr expr = {Expr::SET};
    expr.set_ = {.object_ = object, .value_ = value, .name_ = name};
    return AddExpr(expr);
  }

  ExprId AddLogicalExpr(ExprId left, ExprId right, TokenType op) {
    Expr expr = {Expr::LOGICAL};
    expr.logical_ = {.left_ = left, .right_ = right, .op_ = op};
    return AddExpr(expr);
  }

  ExprId AddBinaryExpr(ExprId left, ExprId right, TokenType op) {
    Expr expr = {Expr::BINARY};
    expr.binary_ = {.left_ = left, .right_ = right, .op_ = op};
    return AddExpr(expr);
  }

  ExprId AddComparisonExpr(ExprBlockId comparables, TokenTypeBlockId ops) {
    Expr expr = {Expr::COMPARISON};
    expr.comparison_ = {.comparables_ = comparables, .ops_ = ops};
    return AddExpr(expr);
  }

  ExprId AddUnaryExpr(ExprId right, TokenType op) {
    Expr expr = {Expr::UNARY};
    expr.unary_ = {.right_ = right, .op_ = op};
    return AddExpr(expr);
  }

  ExprId AddGetExpr(ExprId object, StrId name) {
    Expr expr = {Expr::GET};
    expr.get_ = {.object_ = object, .name_ = name};
    return AddExpr(expr);
  }

  ExprId AddCallExpr(ExprId callee, ExprBlockId args) {
    Expr expr = {Expr::CALL};
    expr.call_ = {.callee_ = callee, .args_ = args};
    return AddExpr(expr);
  }

  ExprId AddLiteralExpr(Object val) {
    Expr expr = {Expr::LITERAL};
    expr.literal_ = {.val_ = val};
    return AddExpr(expr);
  }

  ExprId AddThisExpr() {
    Expr expr = {Expr::THIS};
    return AddExpr(expr);
  }

  ExprId AddVariableExpr(ResolveId id, StrId name) {
    Expr expr = {Expr::VARIABLE};
    expr.variable_ = {.id_ = id, .name_ = name};
    return AddExpr(expr);
  }

  StmtId ParseDeclarationOrStatement() {
    while (IsAtEndStatement()) Advance();

    try {
      if (GetCurrentTokenType() == TokenType::DEF) {
        Advance();
        return ParseFunction();
      }
      if (GetCurrentTokenType() == TokenType::CLASS) {
        Advance();
        return -1;
        // return ParseClassDeclaration();
      }

      return ParseStmt();
    } catch (const std::runtime_error& e) {
      Synchronize();
      return -1;
    }

    return ParseExpressionStmt();
  }

  StmtId ParseFunction() {
    StrId name = ConsumeToken(TokenType::IDENTIFIER).data_.str_idx_;

    StrBlockId params_block = -1;
    ConsumeToken(TokenType::LEFT_PAREN);

    if (GetCurrentTokenType() != TokenType::RIGHT_PAREN) {
      params_block = expr_stmt_pool_.MakeNewStrBlock();
      StrId param_id = ConsumeToken(TokenType::IDENTIFIER).data_.str_idx_;
      expr_stmt_pool_.str_blocks_[params_block].push_back(param_id);
      while (GetCurrentTokenType() == TokenType::COMMA) {
        Advance();
        param_id = ConsumeToken(TokenType::IDENTIFIER).data_.str_idx_;
        expr_stmt_pool_.str_blocks_[params_block].push_back(param_id);
      }
    }

    ConsumeToken(TokenType::RIGHT_PAREN);
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    StmtBlockId body = ParseBlock();

    return AddDefStmt(name, params_block, body);
  }

  // Ptr<stmt::Stmt> ParseClassDeclaration()
  // {
  //   ExpectToken(TokenType::IDENTIFIER, "identifier", false);
  //   Ptr<scanner::Token> name = std::make_shared<scanner::Token>(Pop());

  //   Ptr<Variable> super;
  //   if (GetCurrentTokenType() == TokenType::COLON)
  //   {
  //     Advance();;
  //     ExpectToken(TokenType::IDENTIFIER, "identifier", false);
  //     super =
  //     std::make_shared<Variable>(std::make_shared<scanner::Token>(Pop()),
  //     id_++);
  //   }

  //   ExpectToken(TokenType::LEFT_BRACE, "{");

  //   Ptr<std::vector<Ptr<stmt::Func>>> methods =
  //   std::make_shared<std::vector<Ptr<stmt::Func>>>(); while
  //   (GetCurrentTokenType() != TokenType::RIGHT_BRACE && Remaining())
  //   {
  //     methods->push_back(ParseFuncDeclaration());
  //   }

  //   ExpectToken(TokenType::RIGHT_BRACE, "}");

  //   return std::make_shared<stmt::Class>(name, super, methods);
  // }

  StmtId ParseStmt() {
    switch (GetCurrentTokenType()) {
      case TokenType::IF:
        Advance();
        return ParseIfStmt();
      case TokenType::WHILE:
        Advance();
        return ParseWhileStmt();
      case TokenType::INDENT:
        Advance();
        return ParseBlockStmt();
      case TokenType::RETURN:
        Advance();
        return ParseReturnStmt();
      default:
        break;
    }

    return ParseExpressionStmt();
  }

  StmtId ParseReturnStmt() {
    ExprId value = -1;
    if (!IsAtEndStatement()) {
      value = ParseExpr();
    }

    ConsumeEndStatement();

    return AddReturnStmt(value);
  }

  StmtId ParseWhileStmt() {
    ExprId condition = ParseExpr();
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    StmtId body = ParseBlockStmt();

    return AddWhileStmt(condition, body);
  }

  StmtId ParseIfStmt() {
    ExprId condition = ParseExpr();
    ConsumeToken(TokenType::COLON);
    ConsumeToken(TokenType::NEWLINE);
    ConsumeToken(TokenType::INDENT);

    StmtId true_branch = ParseBlockStmt();
    StmtId false_branch = -1;
    if (GetCurrentTokenType() == TokenType::ELSE) {
      Advance();
      ConsumeToken(TokenType::COLON);
      ConsumeToken(TokenType::NEWLINE);
      ConsumeToken(TokenType::INDENT);
      false_branch = ParseBlockStmt();
    }

    return AddIfStmt(condition, true_branch, false_branch);
  }

  StmtBlockId ParseBlock() {
    StmtBlockId block_id = expr_stmt_pool_.MakeNewStmtBlock();

    while (GetCurrentTokenType() != TokenType::UNINDENT && Remaining()) {
      StmtId stmt_id = ParseDeclarationOrStatement();
      Stmt stmt = expr_stmt_pool_.stmts_[stmt_id];
      expr_stmt_pool_.stmt_blocks_[block_id].push_back(stmt);
    }

    ConsumeToken(TokenType::UNINDENT);
    return block_id;
  }

  StmtId ParseBlockStmt() {
    StmtBlockId block_id = ParseBlock();
    return AddBlockStmt(block_id);
  }

  StmtId ParseExpressionStmt() {
    ExprId expr = ParseExpr();

    ConsumeEndStatement();
    return AddExpressionStmt(expr);
  }

  ExprId ParseExpr() {
    try {
      return ParseAssign();
    } catch (std::runtime_error& e) {
      return {};
    }
  }

  ExprId ParseAssign() {
    ExprId lvalue_id = ParseOr();

    if (GetCurrentTokenType() == TokenType::EQUAL) {
      Advance();
      ExprId value = ParseAssign();
      Expr lvalue = expr_stmt_pool_.exprs_[lvalue_id];
      Expr::Type lvalue_type = lvalue.type_;

      if (lvalue_type == Expr::VARIABLE) {
        return AddAssignExpr(GetNextResolveId(), value, lvalue.variable_.name_);
      } else if (lvalue_type == Expr::GET) {
        return AddSetExpr(lvalue.get_.object_, value, lvalue.get_.name_);
      } else {
        throw std::runtime_error("Bad assignment target.");
      }
    }

    return lvalue_id;
  }

  ExprId ParseOr() {
    ExprId expr = ParseAnd();

    while (GetCurrentTokenType() == TokenType::OR) {
      Token or_token = Pop();
      ExprId right = ParseAnd();
      expr = AddLogicalExpr(expr, right, or_token.meta_.type_);
    }

    return expr;
  }

  ExprId ParseAnd() {
    ExprId expr = ParseComparison();

    while (GetCurrentTokenType() ==
           TokenType::AND)  // TODO: 'if' shall be fine here
    {
      Token and_token = Pop();
      ExprId right = ParseAnd();
      expr = AddLogicalExpr(expr, right, and_token.meta_.type_);
    }

    return expr;
  }

  ExprId ParseComparison() {
    ExprId expr = ParseAddition();

    if (IsAtComparison() || IsAtEqualityCheck()) {
      ExprBlockId comparables = expr_stmt_pool_.MakeNewExprBlock();
      TokenTypeBlockId ops = expr_stmt_pool_.MakeNewTokenTypeBlock();
      Expr cmp = expr_stmt_pool_.exprs_[expr];
      expr_stmt_pool_.expr_blocks_[comparables].push_back(cmp);
      while (IsAtComparison() || IsAtEqualityCheck()) {
        Token op = Pop();
        expr_stmt_pool_.token_type_blocks_[ops].push_back(op.meta_.type_);
        ExprId right = ParseAddition();
        cmp = expr_stmt_pool_.exprs_[right];
        expr_stmt_pool_.expr_blocks_[comparables].push_back(cmp);
      }
      return AddComparisonExpr(comparables, ops);
    }
    return expr;
  }

  ExprId ParseAddition() {
    ExprId expr = ParseMultiplication();

    while (IsAtAddSub()) {
      Token op = Pop();
      ExprId right = ParseMultiplication();
      expr = AddBinaryExpr(expr, right, op.meta_.type_);
    }

    return expr;
  }

  ExprId ParseMultiplication() {
    ExprId expr = ParseUnary();

    while (IsAtMulDiv()) {
      Token op = Pop();
      ExprId right = ParseUnary();
      expr = AddBinaryExpr(expr, right, op.meta_.type_);
    }

    return expr;
  }

  ExprId ParseUnary() {
    if (IsAtUnary()) {
      Token op = Pop();
      ExprId right = ParseUnary();
      return AddUnaryExpr(right, op.meta_.type_);
    }

    return ParseCallOrGet();
  }

  ExprId ParseCallOrGet() {
    ExprId expr = ParsePrimary();

    while (1) {
      if (GetCurrentTokenType() == TokenType::LEFT_PAREN) {
        Advance();
        expr = ParseCall(expr);
      } else if (GetCurrentTokenType() == TokenType::DOT) {
        Advance();
        Token name = ConsumeToken(TokenType::IDENTIFIER);
        expr = AddGetExpr(expr, name.data_.str_idx_);
      } else {
        break;
      }
    }

    return expr;
  }

  ExprId ParseCall(ExprId callee) {
    ExprBlockId args = -1;

    if (GetCurrentTokenType() != TokenType::RIGHT_PAREN) {
      args = expr_stmt_pool_.MakeNewExprBlock();
      ExprId expr_id = ParseExpr();
      Expr expr = expr_stmt_pool_.exprs_[expr_id];
      expr_stmt_pool_.expr_blocks_[args].push_back(expr);
      while (GetCurrentTokenType() == TokenType::COMMA) {
        Advance();
        ExprId expr_id = ParseExpr();
        Expr expr = expr_stmt_pool_.exprs_[expr_id];
        expr_stmt_pool_.expr_blocks_[args].push_back(expr);
      }
    }

    ConsumeToken(TokenType::RIGHT_PAREN);

    return AddCallExpr(callee, args);
  }

  ExprId ParsePrimary() {
    Object object;
    if (TryGetLiteral(&object)) {
      Advance();
      return AddLiteralExpr(object);
    }

    if (GetCurrentTokenType() == TokenType::THIS) {
      Advance();
      return AddThisExpr();
    }

    if (GetCurrentTokenType() == TokenType::IDENTIFIER) {
      Token op = ConsumeToken(TokenType::IDENTIFIER);
      return AddVariableExpr(GetNextResolveId(), op.data_.str_idx_);
    }

    ConsumeToken(TokenType::LEFT_PAREN);
    ExprId group_expr = ParseExpr();
    ConsumeToken(TokenType::RIGHT_PAREN);
    return group_expr;
  }

  Token GetCurrentToken() const { return kTokens[cur_]; }

  TokenType GetCurrentTokenType() const { return kTokens[cur_].meta_.type_; }

  Token Pop() { return kTokens[cur_++]; }

  Token AssertToken(TokenType type) { return _ExpectToken(type, false); }

  Token ConsumeToken(TokenType type) { return _ExpectToken(type, true); }

  Token ConsumeEndStatement() {
    Token cur_token = GetCurrentToken();
    TokenType cur_type = cur_token.meta_.type_;
    if (cur_type != TokenType::NEWLINE && cur_type != TokenType::SEMICOLON) {
      error_ = true;
      // auto pos = tok.GetPosition(kSource);
      auto pos = std::pair<int, int>{0, 0};
      log_(Logger::kError, "[PARSER]:%d:%d: Unexpected %s", pos.first,
           pos.second, GetTokenTypeName(cur_type));
      throw std::runtime_error("Unexpected token.");
    }
    cur_ += 1;
    return cur_token;
  }

  Token _ExpectToken(TokenType type, bool incremet = true) {
    Token cur_token = GetCurrentToken();
    TokenType cur_type = cur_token.meta_.type_;
    if (cur_type != type) {
      error_ = true;
      // auto pos = tok.GetPosition(kSource);
      auto pos = std::pair<int, int>{0, 0};
      log_(Logger::kError, "[PARSER]:%d:%d: Expected \'%s\' before %s",
           pos.first, pos.second, GetTokenTypeName(type),
           GetTokenTypeName(cur_type));
      throw std::runtime_error("Unexpected token.");
    }
    cur_ += incremet;
    return cur_token;
  }

  size_t Advance() {
    // std::cout << GetTokenTypeName(kTokens[cur_ + 1].meta_.type_) << '\n';
    return ++cur_;
  }

  bool Remaining() { return GetCurrentTokenType() != TokenType::END_OF_FILE; }

  bool Synchronize() {
    while (Remaining()) {
      if (IsAtEndStatement()) {
        Advance();
        ;
        return true;
      }
      if (IsAtSynchronizePivot()) {
        return true;
      }
      Advance();
      ;
    }
    return false;
  }

  bool IsAtEndStatement() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::SEMICOLON || type == TokenType::NEWLINE;
  }

  bool IsAtSynchronizePivot() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::CLASS || type == TokenType::DEF ||
           type == TokenType::FOR || type == TokenType::WHILE ||
           type == TokenType::IF || type == TokenType::RETURN;
  }

  bool IsAtEqualityCheck() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::BANG_EQUAL || type == TokenType::EQUAL_EQUAL;
  }

  bool IsAtComparison() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::LESS || type == TokenType::LESS_EQUAL ||
           type == TokenType::GREATER || type == TokenType::GREATER_EQUAL;
  }

  bool IsAtAddSub() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::PLUS || type == TokenType::MINUS;
  }

  bool IsAtMulDiv() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::STAR || type == TokenType::SLASH;
  }

  bool IsAtUnary() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::PLUS || type == TokenType::MINUS ||
           type == TokenType::NOT;
  }

  bool IsAtLiteral() {
    TokenType type = GetCurrentTokenType();
    return type == TokenType::FALSE || type == TokenType::TRUE ||
           type == TokenType::NONE || type == TokenType::STRING ||
           type == TokenType::INT_LITERAL || type == TokenType::FLOAT_LITERAL;
  }

  bool TryGetLiteral(Object* object) {
    Token token = GetCurrentToken();
    switch (token.meta_.type_) {
      case TokenType::TRUE:
        *object = MakeBool(true);
        break;
      case TokenType::FALSE:
        *object = MakeBool(false);
        break;
      case TokenType::NONE:
        *object = MakeNone();
        break;
      case TokenType::STRING:
        *object = MakeString(token.data_.str_idx_);
        break;
      case TokenType::INT_LITERAL:
        *object = MakeInt(token.data_.int_);
        break;
      case TokenType::FLOAT_LITERAL:
        *object = MakeFloat(token.data_.fp_);
        break;
      default:
        return false;
    }
    return true;
  }

  ResolveId GetNextResolveId() { return ++resolve_id_; }
};

}  // namespace ilang