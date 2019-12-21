#include <iostream>
#include <fstream>


#include "scanner/scanner.h"

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
  int retval = 0;
  if (argc > 1)
  {
    retval = ReadFile(argv[1]);
  }
  else
  {
    // ReadFile("example.inp");
    retval = RunPrompt();
  }
  return 0;
}
