#include <iostream>
#include <fstream>
#include <streambuf>


#include "scanner/scanner.h"
#include "parser/expr.h"
#include "parser/parser.h"
#include "experimental/ast_printer.h"

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
  std::shared_ptr<parser::Expr> expr = parser.Parse();

  if (parser.HasError())
  {
    return 1;
  }

  std::cout << AstPrinter::GetValue(*expr) << "\n";
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
    retval = RunPrompt();
  }
  return retval;
}
