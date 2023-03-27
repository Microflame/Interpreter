#include <fstream>
#include <iostream>
#include <span>
#include <streambuf>

#include "expr.h"
#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"
#include "stmt.h"
#include "util/es_to_string.h"
#include "util/version.h"

namespace ilang {

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

}  // namespace ilang

int RunPrompt() { return 1; }

int main(int argc, const char* argv[]) {
  auto args = std::span(argv, size_t(argc));
  int retval = 0;
  if (args.size() > 1) {
    retval = ilang::ExecuteFile(args[1]);
  } else {
    retval = RunPrompt();
  }
  return retval;
}
