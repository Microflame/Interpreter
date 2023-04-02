#include "slip/parser.hpp"

#include "slip/context.hpp"
#include "slip/expr.hpp"
#include "slip/source.hpp"
#include "slip/stmt.hpp"
#include "slip/token.hpp"
#include "slip/util/string_tools.hpp"

namespace slip {

Parser::Parser(const Source& source, const std::vector<Token>& tokens, Context* ctx) :
  kSource(source),
  kTokens(tokens),
  ctx_(*ctx),
  cur_(0),
  log_(Logger::kDebug),
  resolve_id_(-1),
  scope_info_id_(0) {}

std::vector<StmtId> Parser::Parse() {
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

StmtId Parser::AddStmt(Stmt stmt) {
  return ctx_.PushStmt(stmt);
}

StmtId Parser::AddDefStmt(StrId name, StrBlockId params, StmtBlockId body, ScopeInfoId scope_info, ResolveId id) {
  Stmt stmt = {Stmt::DEF};
  stmt.def_ = {.name_ = name, .params_ = params, .body_ = body, .scope_info_ = scope_info, .id_ = id};
  return AddStmt(stmt);
}

StmtId Parser::AddReturnStmt(ExprId value) {
  Stmt stmt = {Stmt::RETURN};
  stmt.return_ = {.value_ = value};
  return AddStmt(stmt);
}

StmtId Parser::AddWhileStmt(ExprId condition, StmtId body) {
  Stmt stmt = {Stmt::WHILE};
  stmt.while_ = {.condition_ = condition, .body_ = body};
  return AddStmt(stmt);
}

StmtId Parser::AddIfStmt(ExprId condition, StmtId true_branch, StmtId false_branch) {
  Stmt stmt = {Stmt::IF};
  stmt.if_ = {.condition_ = condition,
              .true_branch_ = true_branch,
              .false_branch_ = false_branch};
  return AddStmt(stmt);
}

StmtId Parser::AddBlockStmt(StmtBlockId statements) {
  Stmt stmt = {Stmt::BLOCK};
  stmt.block_ = {.statements_ = statements};
  return AddStmt(stmt);
}

StmtId Parser::AddExpressionStmt(ExprId expr) {
  Stmt stmt = {Stmt::EXPRESSION};
  stmt.expression_ = {.expr_ = expr};
  return AddStmt(stmt);
}

ExprId Parser::AddExpr(Expr expr) {
  return ctx_.PushExpr(expr);
}

ExprId Parser::AddAssignExpr(ResolveId id, ExprId value, StrId name) {
  Expr expr = {Expr::ASSIGN};
  expr.assign_ = {.id_ = id, .value_ = value, .name_ = name};
  return AddExpr(expr);
}

ExprId Parser::AddSetExpr(ExprId object, ExprId value, StrId name) {
  Expr expr = {Expr::SET};
  expr.set_ = {.object_ = object, .value_ = value, .name_ = name};
  return AddExpr(expr);
}

ExprId Parser::AddLogicalExpr(ExprId left, ExprId right, TokenType op) {
  Expr expr = {Expr::LOGICAL};
  expr.logical_ = {.left_ = left, .right_ = right, .op_ = op};
  return AddExpr(expr);
}

ExprId Parser::AddBinaryExpr(ExprId left, ExprId right, TokenType op) {
  Expr expr = {Expr::BINARY};
  expr.binary_ = {.left_ = left, .right_ = right, .op_ = op};
  return AddExpr(expr);
}

ExprId Parser::AddComparisonExpr(ExprBlockId comparables, TokenTypeBlockId ops) {
  Expr expr = {Expr::COMPARISON};
  expr.comparison_ = {.comparables_ = comparables, .ops_ = ops};
  return AddExpr(expr);
}

ExprId Parser::AddUnaryExpr(ExprId right, TokenType op) {
  Expr expr = {Expr::UNARY};
  expr.unary_ = {.right_ = right, .op_ = op};
  return AddExpr(expr);
}

ExprId Parser::AddGetExpr(ExprId object, StrId name) {
  Expr expr = {Expr::GET};
  expr.get_ = {.object_ = object, .name_ = name};
  return AddExpr(expr);
}

ExprId Parser::AddCallExpr(ExprId callee, ExprBlockId args) {
  Expr expr = {Expr::CALL};
  expr.call_ = {.callee_ = callee, .args_ = args};
  return AddExpr(expr);
}

ExprId Parser::AddLiteralExpr(Object val) {
  Expr expr = {Expr::LITERAL};
  expr.literal_ = {.val_ = val};
  return AddExpr(expr);
}

ExprId Parser::AddThisExpr() {
  Expr expr = {Expr::THIS};
  return AddExpr(expr);
}

ExprId Parser::AddVariableExpr(ResolveId id, StrId name) {
  Expr expr = {Expr::VARIABLE};
  expr.variable_ = {.id_ = id, .name_ = name};
  return AddExpr(expr);
}


StmtId Parser::ParseDeclarationOrStatement() {
  while (IsAtEndStatement()) Advance();

  if (GetCurrentTokenType() == TokenType::DEF) {
    Advance();
    return ParseFunction();
  }
  if (GetCurrentTokenType() == TokenType::CLASS) {
    throw std::logic_error("Classes not implemented yet");
  }

  return ParseStmt();
}

StmtId Parser::ParseFunction() {
  StrId name = ConsumeToken(TokenType::IDENTIFIER).data_.str_idx_;

  StrBlockId params_block = -1;
  ConsumeToken(TokenType::LEFT_PAREN);

  if (GetCurrentTokenType() != TokenType::RIGHT_PAREN) {
    params_block = ctx_.AddStrBlock();
    StrId param_id = ConsumeToken(TokenType::IDENTIFIER).data_.str_idx_;
    ctx_.str_blocks_[params_block].push_back(param_id);
    while (GetCurrentTokenType() == TokenType::COMMA) {
      Advance();
      param_id = ConsumeToken(TokenType::IDENTIFIER).data_.str_idx_;
      ctx_.str_blocks_[params_block].push_back(param_id);
    }
  }

  ConsumeToken(TokenType::RIGHT_PAREN);
  ConsumeToken(TokenType::COLON);
  ConsumeToken(TokenType::NEWLINE);
  ConsumeToken(TokenType::INDENT);

  StmtBlockId body = ParseBlock();

  return AddDefStmt(name, params_block, body, GetNextScopeInfoId(), GetNextResolveId());
}

StmtId Parser::ParseStmt() {
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

StmtId Parser::ParseReturnStmt() {
  ExprId value = -1;
  if (!IsAtEndStatement()) {
    value = ParseExpr();
  }

  ConsumeEndStatement();

  return AddReturnStmt(value);
}

StmtId Parser::ParseWhileStmt() {
  ExprId condition = ParseExpr();
  ConsumeToken(TokenType::COLON);
  ConsumeToken(TokenType::NEWLINE);
  ConsumeToken(TokenType::INDENT);

  StmtId body = ParseBlockStmt();

  return AddWhileStmt(condition, body);
}

StmtId Parser::ParseIfStmt() {
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

StmtBlockId Parser::ParseBlock() {
  StmtBlockId block_id = ctx_.AddStmtBlock();

  while (GetCurrentTokenType() != TokenType::UNINDENT && Remaining()) {
    StmtId stmt_id = ParseDeclarationOrStatement();
    Stmt stmt = ctx_.stmts_[stmt_id];
    ctx_.stmt_blocks_[block_id].push_back(stmt);
  }

  ConsumeToken(TokenType::UNINDENT);
  return block_id;
}

StmtId Parser::ParseBlockStmt() {
  StmtBlockId block_id = ParseBlock();
  return AddBlockStmt(block_id);
}

StmtId Parser::ParseExpressionStmt() {
  ExprId expr = ParseExpr();

  ConsumeEndStatement();
  return AddExpressionStmt(expr);
}

ExprId Parser::ParseExpr() {
  try {
    return ParseAssign();
  } catch (std::runtime_error& e) {
    return {};
  }
}

ExprId Parser::ParseAssign() {
  ExprId lvalue_id = ParseOr();

  if (GetCurrentTokenType() == TokenType::EQUAL) {
    Advance();
    ExprId value = ParseAssign();
    Expr lvalue = ctx_.exprs_[lvalue_id];
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

ExprId Parser::ParseOr() {
  ExprId expr = ParseAnd();

  while (GetCurrentTokenType() == TokenType::OR) {
    Token or_token = Pop();
    ExprId right = ParseAnd();
    expr = AddLogicalExpr(expr, right, or_token.meta_.type_);
  }

  return expr;
}

ExprId Parser::ParseAnd() {
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

ExprId Parser::ParseComparison() {
  ExprId expr = ParseAddition();

  if (IsAtComparison() || IsAtEqualityCheck()) {
    ExprBlockId comparables = ctx_.AddExprBlock();
    TokenTypeBlockId ops = ctx_.AddTokenTypeBlock();
    Expr cmp = ctx_.exprs_[expr];
    ctx_.expr_blocks_[comparables].push_back(cmp);
    while (IsAtComparison() || IsAtEqualityCheck()) {
      Token op = Pop();
      ctx_.token_type_blocks_[ops].push_back(op.meta_.type_);
      ExprId right = ParseAddition();
      cmp = ctx_.exprs_[right];
      ctx_.expr_blocks_[comparables].push_back(cmp);
    }
    return AddComparisonExpr(comparables, ops);
  }
  return expr;
}

ExprId Parser::ParseAddition() {
  ExprId expr = ParseMultiplication();

  while (IsAtAddSub()) {
    Token op = Pop();
    ExprId right = ParseMultiplication();
    expr = AddBinaryExpr(expr, right, op.meta_.type_);
  }

  return expr;
}

ExprId Parser::ParseMultiplication() {
  ExprId expr = ParseUnary();

  while (IsAtMulDiv()) {
    Token op = Pop();
    ExprId right = ParseUnary();
    expr = AddBinaryExpr(expr, right, op.meta_.type_);
  }

  return expr;
}

ExprId Parser::ParseUnary() {
  if (IsAtUnary()) {
    Token op = Pop();
    ExprId right = ParseUnary();
    return AddUnaryExpr(right, op.meta_.type_);
  }

  return ParseCallOrGet();
}

ExprId Parser::ParseCallOrGet() {
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

ExprId Parser::ParseCall(ExprId callee) {
  ExprBlockId args = -1;

  if (GetCurrentTokenType() != TokenType::RIGHT_PAREN) {
    args = ctx_.AddExprBlock();
    ExprId expr_id = ParseExpr();
    Expr expr = ctx_.exprs_[expr_id];
    ctx_.expr_blocks_[args].push_back(expr);
    while (GetCurrentTokenType() == TokenType::COMMA) {
      Advance();
      ExprId expr_id = ParseExpr();
      Expr expr = ctx_.exprs_[expr_id];
      ctx_.expr_blocks_[args].push_back(expr);
    }
  }

  ConsumeToken(TokenType::RIGHT_PAREN);

  return AddCallExpr(callee, args);
}

ExprId Parser::ParsePrimary() {
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

size_t Parser::Advance() {
  return ++cur_;
}

Token Parser::Pop() { return kTokens[cur_++]; }

Token Parser::ConsumeToken(TokenType type) {
  Token t = ExpectToken(type);
  Advance();
  return t;
}

Token Parser::ConsumeEndStatement() {
  Token cur_token = GetCurrentToken();
  TokenType cur_type = cur_token.meta_.type_;
  if (cur_type != TokenType::NEWLINE && cur_type != TokenType::SEMICOLON) {
    auto pos = std::pair<int, int>{0, 0};
    log_(Logger::kError, "[PARSER]:%d:%d: Unexpected %s", pos.first,
          pos.second, GetTokenTypeName(cur_type));
    throw std::runtime_error("Unexpected token.");
  }
  Advance();
  return cur_token;
}

Token Parser::ExpectToken(TokenType type) const {
  Token cur_token = GetCurrentToken();
  TokenType cur_type = cur_token.meta_.type_;
  if (cur_type != type) {
    // auto pos = std::pair<int, int>{0, 0};
    // log_(Logger::kError, "[PARSER]:%d:%d: Expected \'%s\' before %s",
    //       pos.first, pos.second, GetTokenTypeName(type),
    //       GetTokenTypeName(cur_type));
    throw std::runtime_error("Unexpected token.");
  }
  return cur_token;
}

Token Parser::GetCurrentToken() const { return kTokens[cur_]; }

TokenType Parser::GetCurrentTokenType() const { return kTokens[cur_].meta_.type_; }

Token Parser::AssertToken(TokenType type) const { return ExpectToken(type); }

bool Parser::Remaining() const { return GetCurrentTokenType() != TokenType::END_OF_FILE; }

bool Parser::IsAtEndStatement() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::SEMICOLON || type == TokenType::NEWLINE;
}

bool Parser::IsAtSynchronizePivot() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::CLASS || type == TokenType::DEF ||
          type == TokenType::FOR || type == TokenType::WHILE ||
          type == TokenType::IF || type == TokenType::RETURN;
}

bool Parser::IsAtEqualityCheck() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::BANG_EQUAL || type == TokenType::EQUAL_EQUAL;
}

bool Parser::IsAtComparison() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::LESS || type == TokenType::LESS_EQUAL ||
          type == TokenType::GREATER || type == TokenType::GREATER_EQUAL;
}

bool Parser::IsAtAddSub() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::PLUS || type == TokenType::MINUS;
}

bool Parser::IsAtMulDiv() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::STAR || type == TokenType::SLASH;
}

bool Parser::IsAtUnary() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::PLUS || type == TokenType::MINUS ||
          type == TokenType::NOT;
}

bool Parser::IsAtLiteral() const {
  TokenType type = GetCurrentTokenType();
  return type == TokenType::FALSE || type == TokenType::TRUE ||
          type == TokenType::NONE || type == TokenType::STRING ||
          type == TokenType::INT_LITERAL || type == TokenType::FLOAT_LITERAL;
}

bool Parser::TryGetLiteral(Object* object) const {
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


ResolveId Parser::GetNextResolveId() { return ++resolve_id_; }
ScopeInfoId Parser::GetNextScopeInfoId() { return ++scope_info_id_; }

}  // namespace slip
