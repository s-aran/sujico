#pragma once

#include "Header.h"
#include "EnvironmentVariable.h"

#define NAMESPACE_REGEX boost


namespace Piyo
{
  class VCS
  {
    // TODO: back to private
  public:
    using ParsingCallback = std::function<const std::string(const std::string&, const std::string&)>;
    using CallbackMap = std::map<std::string, ParsingCallback>;
    using CacheMap = std::map<std::string, std::string>;

    class VCSBase
    {
    private:
      std::string result_;

      CallbackMap callbacks_;
      CacheMap caches_;

      void setResult(const std::string& result);

    protected:
      const CallbackMap& getCallbacks() const;
      const std::string& getResult() const;

      bool execute(const std::string& commandline, const std::string& currentDirectory = "");
      
      bool parse();
      bool parse(const std::string& result, const CallbackMap& callbacks);

      void addCallback(const std::string& key, ParsingCallback callback);

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
      bool executeVcs();

    public:
      Svn();
    };


    class Git : public VCSBase
    {
    private:
      bool executeVcs();
      bool getShortHash();
      void prepareCallbacks();
      CallbackMap createCallbackMapForShortHash();

    public:
      Git();
    };


    class Mercurial : public VCSBase
    {
    private:
      bool executeVcs();
      void prepareCallbacks();

    public:
      Mercurial();
    };
  };
}


