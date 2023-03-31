#include <fstream>
#include <iostream>
#include <span>
#include <streambuf>

#include "slip/expr.hpp"
#include "slip/interpreter.hpp"
#include "slip/parser.hpp"
#include "slip/resolver.hpp"
#include "slip/scanner.hpp"
#include "slip/stmt.hpp"
#include "slip/util/es_to_string.hpp"
#include "slip/util/version.hpp"

namespace slip {

std::string ReadFile(const char* path) {
  std::ifstream fin(path);
  if (!fin.is_open()) {
    std::cerr << "Can not open " << path << "\n";
  }

  std::string source((std::istreambuf_iterator<char>(fin)),
                     std::istreambuf_iterator<char>());
  return source;
}

int ExecuteFile(const char* path) {
  std::string source = ReadFile(path);

  ExprStmtPool es_pool;
  es_pool.PushStr("print");

  Scanner scanner;
  TokenSpawner token_spawner(&es_pool);
  std::vector<Token> tokens = scanner.GetTokens(source, &token_spawner);

  if (scanner.HasError()) {
    std::cerr << "Scanner error\n";
    return 1;
  }

  // for (auto t: tokens) {
  //   std::cout << token_spawner.ToString(t) << '\n';
  // }

  Parser parser(source, tokens, &es_pool);
  std::vector<StmtId> statements = parser.Parse();

  if (parser.HasError()) {
    return 1;
  }

  // for (StmtId stmt : statements) {
  //   std::cout << StmtToString(stmt, es_pool);
  // }

  Resolver resolver(es_pool);
  resolver.ResolveStmts(statements);

  Interpreter interpreter(es_pool, resolver);
  interpreter.Interpret(statements);

  return 0;
}

}  // namespace slip

int RunPrompt() { return 1; }

int main(int argc, const char* argv[]) {
  auto args = std::span(argv, size_t(argc));
  int retval = 0;
  if (args.size() > 1) {
    retval = slip::ExecuteFile(args[1]);
  } else {
    retval = RunPrompt();
  }
  return retval;
}
