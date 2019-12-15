#pragma once
#include "Header.h"

namespace Piyo
{
  class Executor
  {
  private:
    class IExecutor
    {
    private:
      unsigned int maxBuffers_ = 1024;
      std::string stdout_;
      // std::string stderr_;

    protected:
      void setStdout(const std::string& out);

    public:
      const std::string getStdout() const;

      const unsigned int getMaxBuffers() const;
      const unsigned int setMaxBuffers(unsigned int value);

      virtual int32_t execute(const std::string& commandLine, const std::string& currentDirectory = "") = 0;
    };

    struct ExecutorWindows : public IExecutor
    {
      virtual int32_t execute(const std::string& commandLine, const std::string& currentDirectory = "") override;
    };

    struct ExecutorLinux : public IExecutor
    {
      virtual int32_t execute(const std::string& commandLine, const std::string& currentDirectory = "") override;
    };

  private:
    static std::unique_ptr<IExecutor> instance_;
    static std::once_flag once_;

    Executor() = default;
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

  public:
    virtual ~Executor() = default;
    static IExecutor& getInstance();
  };
}

