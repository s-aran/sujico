#include "Header.h"
#include "Executor.h"


int32_t Piyo::Executor::ExecutorLinux::execute(const std::string& commandLine, const std::string& currentDirector)
{
  int32_t r = -1;
  std::string out = "";
  char* buffer = new char[this->getMaxBuffers()];
  {
    FILE* fp = popen(commandLine.c_str(), "r");
    if (fp == nullptr)
    {
      std::cerr << "popen() failed." << std::endl;
      return -1;
    }

    while (fgets(buffer, this->getMaxBuffers(), fp) != NULL)
    {
      out += std::string(buffer);
    }

    r = pclose(fp);
  }
  delete[] buffer;

  this->setStdout(out);

  return r;
}
