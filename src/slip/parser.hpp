#pragma once

#include <vector>

#include "slip/indexing.hpp"
#include "slip/util/logger.hpp"

namespace slip {

class Context;
class Object;
class Source;

struct Expr;
struct Stmt;
struct Token;

enum class TokenType: uint8_t;

class Parser {
public:
  Parser(const Source& source, const std::vector<Token>& tokens, Context* ctx);

  std::vector<StmtId> Parse();

private:
  const Source& kSource;
  const std::vector<Token>& kTokens;
  Context& ctx_;
  size_t cur_;
  Logger log_;
  ResolveId resolve_id_;
  ScopeInfoId scope_info_id_;

  StmtId AddStmt(Stmt stmt);
  StmtId AddDefStmt(StrId name, StrBlockId params, StmtBlockId body, ScopeInfoId scope_info, ResolveId id);
  StmtId AddReturnStmt(ExprId value);
  StmtId AddWhileStmt(ExprId condition, StmtId body);
  StmtId AddIfStmt(ExprId condition, StmtId true_branch, StmtId false_branch);
  StmtId AddBlockStmt(StmtBlockId statements);
  StmtId AddExpressionStmt(ExprId expr);

  ExprId AddExpr(Expr expr);
  ExprId AddAssignExpr(ResolveId id, ExprId value, StrId name);
  ExprId AddSetExpr(ExprId object, ExprId value, StrId name);
  ExprId AddLogicalExpr(ExprId left, ExprId right, TokenType op);
  ExprId AddBinaryExpr(ExprId left, ExprId right, TokenType op);
  ExprId AddComparisonExpr(ExprBlockId comparables, TokenTypeBlockId ops);
  ExprId AddUnaryExpr(ExprId right, TokenType op);
  ExprId AddGetExpr(ExprId object, StrId name);
  ExprId AddCallExpr(ExprId callee, ExprBlockId args);
  ExprId AddLiteralExpr(Object val);
  ExprId AddThisExpr();
  ExprId AddVariableExpr(ResolveId id, StrId name);

  StmtId ParseDeclarationOrStatement();
  StmtId ParseFunction();
  StmtId ParseStmt();
  StmtId ParseReturnStmt();
  StmtId ParseWhileStmt();
  StmtId ParseIfStmt();
  StmtBlockId ParseBlock();
  StmtId ParseBlockStmt();
  StmtId ParseExpressionStmt();

  ExprId ParseExpr();
  ExprId ParseAssign();
  ExprId ParseOr();
  ExprId ParseAnd();
  ExprId ParseComparison();
  ExprId ParseAddition();
  ExprId ParseMultiplication();
  ExprId ParseUnary();
  ExprId ParseCallOrGet();
  ExprId ParseCall(ExprId callee);
  ExprId ParsePrimary();

  size_t Advance();
  Token Pop();
  Token ConsumeToken(TokenType type);
  Token ConsumeEndStatement();

  Token ExpectToken(TokenType type) const;
  Token GetCurrentToken() const;
  TokenType GetCurrentTokenType() const;
  Token AssertToken(TokenType type) const;
  bool Remaining() const;
  bool Synchronize() const;
  bool IsAtEndStatement() const;
  bool IsAtSynchronizePivot() const;
  bool IsAtEqualityCheck() const;
  bool IsAtComparison() const;
  bool IsAtAddSub() const;
  bool IsAtMulDiv() const;
  bool IsAtUnary() const;
  bool IsAtLiteral() const;
  bool TryGetLiteral(Object* object) const;

  ResolveId GetNextResolveId();
  ScopeInfoId GetNextScopeInfoId();
};

}  // namespace slip
