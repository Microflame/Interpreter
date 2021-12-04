#include <iostream>
#include <fstream>
#include <streambuf>


#include "scanner.h"
#include "expr.h"
#include "stmt.h"
#include "parser.h"
#include "util/es_to_string.h"
#include "util/version.h"
// #include "resolver/resolver.h"
// #include "interpreter/interpreter.h"

namespace ilang
{

std::string ReadFile(const char* path)
{
  std::ifstream fin(path);
  if (!fin.is_open())
  {
    std::cerr << "Can not open " << path << "\n";
  }

  std::string source((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
  return source;
}

int ExecuteFile(const char* path)
{
  std::string source = ReadFile(path);

  Scanner scanner;
  TokenSpawner token_spawner;
  std::vector<Token> tokens = scanner.GetTokens(source, &token_spawner);

  if (scanner.HasError())
  {
    std::cerr << "Scanner error" << "\n";
    return 1;
  }

  // for (auto t: tokens)
  // {
  //   std::cout << token_spawner.ToString(t) << '\n';
  // }

  ExprStmtPool es_pool;
  Parser parser(source, tokens, &es_pool);
  std::vector<StmtId> statements = parser.Parse();

  if (parser.HasError())
  {
    return 1;
  }

  Pools pools{es_pool, token_spawner};
  for (StmtId stmt: statements)
  {
    std::cout << StmtToString(stmt, pools);
  }

  // interpreter::Interpreter interpreter;
  // resolver::Resolver resolver(interpreter);

  // resolver.Resolve(statements);

  // interpreter.Interpret(statements);

  return 0;
}

} // namespace ilang



int RunPrompt()
{
  return 1;
}

int main(int argc, const char* argv[])
{
  int retval = 0;
  if (argc > 1)
  {
    retval = ilang::ExecuteFile(argv[1]);
  }
  else
  {
    retval = ilang::ExecuteFile("./example.inp");
    retval = RunPrompt();
  }
  return retval;
}
