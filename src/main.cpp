#include <fstream>
#include <iostream>
#include <span>
#include <streambuf>

#include "slip/expr.hpp"
#include "slip/interpreter.hpp"
#include "slip/parser.hpp"
#include "slip/resolver.hpp"
#include "slip/source.hpp"
#include "slip/stmt.hpp"
#include "slip/tokenizer.hpp"
#include "slip/util/es_to_string.hpp"
#include "slip/util/version.hpp"

namespace slip {

int ExecuteFile(const char* path) {
  Source source = Source::LoadFromFile(path);

  ExprStmtPool es_pool;
  es_pool.PushStr("print");

  TokenSpawner token_spawner(&es_pool);
  Tokenizer tokenizer;
  std::vector<Token> tokens;
  try {
    tokens = tokenizer.Run(source, &token_spawner);
  } catch (InSourceError& e) {
    SourceIntersection si = source.FindLine(e.range_.offset);
    std::cerr << "Error:\n";
    std::cerr << source.GetFileName() << ":" << si.row + 1 << ":" << si.column + 1 << ": " << e.message_ << "\n";
    exit(1);
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    exit(1);
  }

  for (auto t: tokens) {
    std::cout << token_spawner.ToString(t) << '\n';
  }

  // Parser parser(source, tokens, &es_pool);
  // std::vector<StmtId> statements = parser.Parse();

  // if (parser.HasError()) {
  //   return 1;
  // }

  // for (StmtId stmt : statements) {
  //   std::cout << StmtToString(stmt, es_pool);
  // }

  // Resolver resolver(es_pool);
  // resolver.ResolveStmts(statements);

  // Interpreter interpreter(es_pool, resolver);
  // interpreter.Interpret(statements);

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
