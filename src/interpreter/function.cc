#include "function.h"

#include "interpreter.h"

namespace interpreter
{

UserDefinedFunction::UserDefinedFunction(std::shared_ptr<parser::stmt::Func> func)
  : func_(func)
{}

common::Object UserDefinedFunction::Call(interpreter::Interpreter& interpreter,
                                         std::vector<common::Object>& args) const
{
  auto g = interpreter.environment_stack_.GetGuard(std::make_shared<Environment>(interpreter.environment_stack_.GetRoot()));

  for (size_t i = 0; i < args.size(); ++i)
  {
    interpreter.GetCurrentEnv().Define((*(func_->params_))[i]->ToRawString(), args[i]);
  }
  
  interpreter.ExecuteUnguardedBlock(parser::stmt::Block(func_->body_));

  common::Object retval;
  if (interpreter.retval_)
  {
    retval = *interpreter.retval_;
    interpreter.retval_ = nullptr;
  }

  return retval;
}

size_t UserDefinedFunction::GetArity() const
{
  return func_->params_->size();
}

} // namespace interpreter
