#include <iostream>
#include <fstream>
#include <streambuf>


#include "scanner/scanner.h"
#include "parser/expr.h"
#include "parser/stmt.h"
#include "parser/parser.h"
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

void Print(StmtId id, const ExprStmtPool& es_pool)
{
  Stmt stmt = es_pool.statements_[id];
  std::cout << StmtTypeToString(stmt.type_) << '\n';
  switch (stmt.type_)
  {
    case Stmt::RETURN:
    {
      
    }
    case Stmt::DEF:
    {
      
    }
    case Stmt::CLASS:
    {
      
    }
    case Stmt::IF:
    {
      
    }
    case Stmt::BLOCK:
    {
      
    }
    case Stmt::EXPRESSION:
    {
      
    }
    case Stmt::WHILE:
    {
      
    }
  }
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

  for (StmtId stmt: statements)
  {
    Print(stmt, es_pool);
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
