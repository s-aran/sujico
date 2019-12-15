#pragma once

#include "Header.h"
#include "EnvironmentVariable.h"

#define NAMESPACE_REGEX boost

namespace Piyo
{
  class VCS
  {
  private:
    using ParsingCallback = std::function<const std::string(const std::string&, const std::string&)>;
    using CallbackMap = std::map<std::string, ParsingCallback>;
    using CacheMap = std::map<std::string, std::string>;

    class VCSBase
    {
    private:
      std::string result_;

      CallbackMap callbacks_;
      CacheMap caches_;

    protected:
      const CallbackMap& getCallbacks() const;

      bool execute(const std::string& commandline, const std::string& currentDirectory = "");
      virtual bool executeVcs() = 0;

      void addCallback(const std::string& key, ParsingCallback callback);
      bool parse();

    public:
      // TODO: back to protected
      const CacheMap& getCaches() const;

      bool hasKey(const std::string& key) const;
      const std::string get(const std::string& key) const;
    };


  public:
    class Svn : public VCSBase
    {
    private:
      virtual bool executeVcs() override;

    public:
      Svn();
    };


    class Git : public VCSBase
    {
    private:
      virtual bool executeVcs() override;

    public:
      Git();
    };


    class Mercurial : public VCSBase
    {
    private:
      virtual bool executeVcs() override;
      void prepareCallbacks();

    public:
      Mercurial();
    };
  };
}


