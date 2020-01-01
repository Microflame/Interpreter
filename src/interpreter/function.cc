#include "function.h"

#include "interpreter.h"

namespace interpreter
{

UserDefinedFunction::UserDefinedFunction(std::shared_ptr<parser::stmt::Func> func,  
                                         std::shared_ptr<Environment> closure)
  : func_(func),
    closure_(closure)
{}

common::Object UserDefinedFunction::Call(interpreter::Interpreter& interpreter,
                                         std::vector<common::Object>& args) const
{
  auto g = interpreter.environment_stack_.GetGuard(std::make_shared<Environment>(closure_));

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

std::string UserDefinedFunction::GetName() const
{
  return func_->name_->ToRawString();
}

size_t UserDefinedFunction::GetArity() const
{
  return func_->params_->size();
}

std::shared_ptr<common::ICallable> UserDefinedFunction::Bind(const std::string& name, common::Object arg) const
{
  std::shared_ptr<Environment> wrapper = std::make_shared<Environment>(closure_);
  wrapper->Define(name, arg);
  return std::make_shared<UserDefinedFunction>(func_, wrapper);
}

} // namespace interpreter
