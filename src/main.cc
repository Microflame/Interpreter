#include <iostream>
#include <fstream>
#include <streambuf>


#include "scanner/scanner.h"
#include "parser/expr.h"
#include "parser/stmt.h"
#include "parser/parser.h"
// #include "experimental/ast_printer.h"
#include "resolver/resolver.h"
#include "interpreter/interpreter.h"

int ReadFile(const char* path)
{
  std::ifstream fin(path);
  if (!fin.is_open())
  {
    std::cerr << "Can not open " << path << "\n";
  }

  std::string source((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

  scanner::Scanner scanner(source);
  std::vector<scanner::Token> tokens = scanner.GetTokens();

  if (scanner.HasError())
  {
    return 1;
  }

  parser::Parser parser(source, tokens);
  std::vector<std::shared_ptr<parser::stmt::Stmt>> statements = parser.Parse();

  if (parser.HasError())
  {
    return 1;
  }

  // std::cout << AstPrinter::GetValue(*expr) << "\n";

  interpreter::Interpreter interpreter;
  resolver::Resolver resolver(interpreter);

  resolver.Resolve(statements);

  interpreter.Interpret(statements);

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
    retval = ReadFile(argv[1]);
  }
  else
  {
    retval = ReadFile("./example.inp");
    retval = RunPrompt();
  }
  return retval;
}
