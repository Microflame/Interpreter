#include <iostream>
#include <fstream>


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

  scanner::Scanner scanner(fin);
  std::vector<scanner::Token> tokens = scanner.GetTokens();

  parser::Parser parser(tokens);
  std::shared_ptr<parser::Expr> expr = parser.Parse();

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
