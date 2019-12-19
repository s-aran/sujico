#include "Header.h"
#include "VCS.h"
#include "Executor.h"



const std::string Piyo::VCS::VCSBase::get(const std::string& key) const
{
  const auto& itr = this->getCaches().find(key);
  std::string result = itr != std::cend(this->getCaches()) ? itr->second : "";  
  return std::move(result);
}

const Piyo::VCS::CallbackMap& Piyo::VCS::VCSBase::getCallbacks() const
{
  return this->callbacks_;
}

const std::string& Piyo::VCS::VCSBase::getResult() const
{
  return this->result_;
}

void Piyo::VCS::VCSBase::setResult(const std::string& result)
{
  this->result_ = result;
}

const Piyo::VCS::CacheMap& Piyo::VCS::VCSBase::getCaches() const
{
  return this->caches_;
}

bool Piyo::VCS::VCSBase::execute(const std::string& commandline, const std::string& currentDirectory)
{
  auto& executor = Executor::getInstance();
  auto result = executor.execute(commandline, currentDirectory);
  this->setResult(std::move(executor.getStdout()));
  return result == 0;
}

void Piyo::VCS::VCSBase::addCallback(const std::string& key, ParsingCallback callback)
{
  callbacks_[key] = std::move(callback);
}

bool Piyo::VCS::VCSBase::parse()
{
  return this->parse(this->getResult(), this->getCallbacks());
}

bool Piyo::VCS::VCSBase::parse(const std::string& result, const Piyo::VCS::CallbackMap& callbacks)
{
  bool r = true;

  for (auto itr = std::begin(callbacks); itr != std::end(callbacks); ++itr)
  {
    auto& key = itr->first;
    auto& callback = itr->second;

    this->caches_[key] = callback(key, result_);
  }

  return r;
}

bool Piyo::VCS::VCSBase::hasKey(const std::string& key) const
{
  const auto& list = getCaches();
  const auto itr = list.find(key);
  return itr != std::cend(list);
}
