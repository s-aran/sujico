#include "Header.h"
#include "Replacer.h"



std::string Piyo::Replacer::tomValue2String(const toml::value& value, const std::function<void(const toml::value&, const std::exception&) > errorHandler)
{
  std::string result;

  try
  {
    result = value.as_string().str;
  }
  catch ([[maybe_unused]] const std::exception & e)
  {
    errorHandler(value, e);
  }

  return std::move(result);
}

bool Piyo::Replacer::replace(const std::string& format, const toml::table& table)
{
  bool result = true;

  for (auto itr = std::begin(table); itr != std::end(table); ++itr)
  {
    auto i = this->callbacks_.find(itr->first);
    if (i == std::end(this->callbacks_))
    {
      continue;
    }
    auto& callback = i->second;

    std::string value = Replacer::tomValue2String(itr->second);
    result &= callback(itr->first, value, format);
  }

  return result;
}

bool Piyo::Replacer::replace(const std::string& format, const Replacer::TableMap& table)
{
  bool result = true;

  for (auto itr = std::begin(table); itr != std::end(table); ++itr)
  {
    auto i = this->callbacks_.find(itr->first);
    if (i == std::end(this->callbacks_))
    {
      continue;
    }
    auto& callback = i->second;

    result &= callback(itr->first, itr->second, format);
  }

  return result;
}

void Piyo::Replacer::add(const std::string& name, Callback function)
{
  callbacks_[name] = std::move(function);
}

