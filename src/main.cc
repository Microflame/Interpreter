#include <iostream>
#include <fstream>
#include <streambuf>


#include "scanner/scanner.h"
// #include "parser/expr.h"
// #include "parser/stmt.h"
// #include "parser/parser.h"
// #include "resolver/resolver.h"
// #include "interpreter/interpreter.h"

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

  scanner::Scanner scanner;
  scanner::TokenSpawner token_spawner;
  std::vector<scanner::Token> tokens = scanner.GetTokens(source, &token_spawner);

  if (scanner.HasError())
  {
    std::cerr << "Scanner error" << "\n";
    return 1;
  }

  for (auto t: tokens)
  {
    std::cout << token_spawner.ToString(t) << '\n';
  }

  // parser::Parser parser(source, tokens);
  // std::vector<std::shared_ptr<parser::stmt::Stmt>> statements = parser.Parse();

  // if (parser.HasError())
  // {
  //   return 1;
  // }

  // interpreter::Interpreter interpreter;
  // resolver::Resolver resolver(interpreter);

  // resolver.Resolve(statements);

  // interpreter.Interpret(statements);

  return 0;
}

int RunPrompt()
{
  return 1;
}

int main(int argc, const char* argv[])
{
  int retval = 0;
  if (argc > 1)
  {
    retval = ExecuteFile(argv[1]);
  }
  else
  {
    retval = ExecuteFile("./example.inp");
    retval = RunPrompt();
  }
  return retval;
}
