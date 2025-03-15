#include "Header.h"
#include "EnvironmentVariable.h"


std::unique_ptr<Piyo::EnvironmentVariable::IEnvironmentVariable> Piyo::EnvironmentVariable::instance_;
std::once_flag Piyo::EnvironmentVariable::once_;


Piyo::EnvironmentVariable::IEnvironmentVariable& Piyo::EnvironmentVariable::getInstance()
{
  std::call_once(once_, []() {
#if defined _UNIX || defined __linux__ || defined __CYGWIN__
    instance_.reset(new Linux());
#elif defined _WIN32
    instance_.reset(new Windows());
#endif  /* _WIN32 */
    });

    return *instance_;
}
