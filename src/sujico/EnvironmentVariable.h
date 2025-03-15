#pragma once

#include "Header.h"

namespace Piyo
{
  class EnvironmentVariable
  {
  private:
    struct IEnvironmentVariable
    {
      virtual bool isExists(const std::string& name) = 0;
      virtual bool set(const std::string& name, const std::string& value) = 0;
      virtual bool concat(const std::string& name, const std::string& value, bool toFirst = false, const std::string delimiter = ";") = 0;
      virtual std::string get(const std::string& name, bool fromFirst = false, const std::string delimiter = ",") = 0;
      virtual bool remove(const std::string& name) = 0;
    };

    struct Windows : public IEnvironmentVariable
    {
      bool isExists(const std::string& name) override;
      bool set(const std::string& name, const std::string& value) override;
      bool concat(const std::string& name, const std::string& value, bool toFirst = false, const std::string delimiter = ";") override;
      std::string get(const std::string& name, bool fromFirst = false, const std::string delimiter = ",") override;
      bool remove(const std::string& name) override;
    };

    struct Linux: public IEnvironmentVariable
    {
      bool isExists(const std::string& name) override;
      bool set(const std::string& name, const std::string& value) override;
      bool concat(const std::string& name, const std::string& value, bool toFirst = false, const std::string delimiter = ";") override;
      std::string get(const std::string& name, bool fromFirst = false, const std::string delimiter = ",") override;
      bool remove(const std::string& name) override;
    };

  private:
    static std::unique_ptr<IEnvironmentVariable> instance_;
    static std::once_flag once_;

    EnvironmentVariable() = default;
    EnvironmentVariable(const EnvironmentVariable&) = delete;
    EnvironmentVariable& operator=(const EnvironmentVariable*) = delete;
  
  public:
    virtual ~EnvironmentVariable() = default;

    static IEnvironmentVariable& getInstance();

  };
}
