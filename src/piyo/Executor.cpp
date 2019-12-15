#include "Header.h"
#include "Executor.h"


std::unique_ptr < Piyo::Executor::IExecutor> Piyo::Executor::instance_;
std::once_flag Piyo::Executor::once_;


void Piyo::Executor::IExecutor::setStdout(const std::string& out)
{
  this->stdout_ = out;
}

const std::string Piyo::Executor::IExecutor::getStdout() const
{
  return this->stdout_;
}

const unsigned int Piyo::Executor::IExecutor::getMaxBuffers() const
{
  return this->maxBuffers_;
}

const unsigned int Piyo::Executor::IExecutor::setMaxBuffers(unsigned int value)
{
  auto r = this->getMaxBuffers();
  this->maxBuffers_ = value;
  return r;
}

Piyo::Executor::IExecutor& Piyo::Executor::getInstance()
{
  std::call_once(once_, []() {
#if defined _UNIX || defined __linux__ || defined __CYGWIN__
    instance_.reset(new ExecutorLinux());
#elif defined _WIN32
    instance_.reset(new ExecutorWindows());
#endif
    });

  return *instance_;
}
