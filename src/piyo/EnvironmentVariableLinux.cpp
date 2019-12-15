#include "Header.h"
#include "EnvironmentVariable.h"


bool Piyo::EnvironmentVariable::Linux::isExists(const std::string& name)
{
  static constexpr auto MaxSize = 1;
  bool r = true;

  return r;
}

bool Piyo::EnvironmentVariable::Linux::set(const std::string& name, const std::string& value)
{
  bool r = true;

  return r;
}

bool Piyo::EnvironmentVariable::Linux::concat(const std::string& name, const std::string& value, bool toFirst, const std::string delimiter)
{
  // TODO
  return false;
}

std::string Piyo::EnvironmentVariable::Linux::get(const std::string& name, bool fromFirst, const std::string delimiter)
{
  std::string r;
  
  return std::move(r);
}

bool Piyo::EnvironmentVariable::Linux::remove(const std::string& name)
{
  return Piyo::EnvironmentVariable::Linux::set(name, "");
}

