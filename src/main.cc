#include <iostream>
#include <fstream>


#include "scanner/scanner.h"
#include "parser/expr.h"
#include "experimental/ast_printer.h"

int ReadFile(const char* path)
{
  std::ifstream fin(path);
  if (!fin.is_open())
  {
    std::cerr << "Can not open " << path << "\n";
  }
  Scanner scan(fin);
  std::vector<Token> tokens = scan.GetTokens();
  for (const auto& t: tokens)
  {
    std::cout << t.ToString() << "\n";
  }
}

int RunPrompt()
{
  return 1;
}

int main(int argc, const char* argv[])
{
  Token minus(Token::MINUS, "-", 1);
  Token num123(Token::INT_LITERAL, "123", 3);
  Literal literal(num123);
  Unary unary(minus, literal);

  std::cout << AstPrinter::GetValue(unary) << "\n";
  // int retval = 0;
  // if (argc > 1)
  // {
  //   retval = ReadFile(argv[1]);
  // }
  // else
  // {
  //   retval = RunPrompt();
  // }
  return 0;
}
