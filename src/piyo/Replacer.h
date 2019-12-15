#pragma once

#include "Header.h"


namespace Piyo
{
  class Replacer
  {
  public:
    using Callback = std::function<bool(const std::string&, const std::string&, const std::string&)>;
    using TableMap = std::map<std::string, std::string>;

  private:
    using CallbackMap = std::map<std::string, Callback>;

    CallbackMap callbacks_;

    static std::string tomValue2String(const toml::value& value, const std::function<void(const toml::value&, const std::exception&)> errorHandler = 
      [](const auto& msg, const auto& exception) { std::cerr << "internal error" << std::endl; });

  public:
    // Replacer();
    // virtual ~Replacer();




    bool replace(const std::string& format, const toml::table& table);
    bool replace(const std::string& format, const TableMap& table);

    void add(const std::string& name, Callback function);


    static bool sample(const std::string& key, const std::string& value, const std::string& format)
    {
      std::cout << key << ": " << value << " (" << format << ")" << std::endl;
      return true;
    }

  };


}

