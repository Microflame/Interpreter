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
  scanner::Scanner scan(fin);
  std::vector<scanner::Token> tokens = scan.GetTokens();
  for (const auto& t: tokens)
  {
    std::cout << t.ToString() << "\n";
  }
}

int RunPrompt()
{
  return 1;
}

void TestAst()
{
  auto unary = std::make_shared<parser::Unary>(
                 std::make_shared<scanner::Token>(scanner::Token::MINUS, "-", 1),
                 std::make_shared<parser::Literal>(
                   std::make_shared<scanner::Token>(scanner::Token::INT_LITERAL, "123", 3)
                 )
               );

  std::cout << AstPrinter::GetValue(*unary) << "\n";
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
  return 0;
}
