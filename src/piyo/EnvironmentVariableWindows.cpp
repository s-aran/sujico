#include "Header.h"
#include "EnvironmentVariable.h"

bool Piyo::EnvironmentVariable::Windows::isExists(const std::string& name)
{
  static constexpr auto MaxSize = 1;
  bool r = true;

  // get size
  {
    TCHAR buffer[1] = { '\0' };
    UINT size = GetEnvironmentVariable(name.c_str(), buffer, 1);
    r = (GetLastError() == ERROR_ENVVAR_NOT_FOUND || size == 0) ? false : true;
  }

  return r;
}

bool Piyo::EnvironmentVariable::Windows::set(const std::string& name, const std::string& value)
{
  bool r = true;

  r = SetEnvironmentVariable(name.c_str(), value.c_str()) == TRUE;

  return r;
}

bool Piyo::EnvironmentVariable::Windows::concat(const std::string& name, const std::string& value, bool toFirst, const std::string delimiter)
{
  // TODO
  return false;
}

std::string Piyo::EnvironmentVariable::Windows::get(const std::string& name, bool fromFirst, const std::string delimiter)
{
  std::string r;

#if defined(_WIN32)
  UINT size;

  // get size
  {
    TCHAR buffer[1] = { '\0' };
    size = GetEnvironmentVariable(name.c_str(), buffer, 1);
    size += sizeof(TCHAR);   // null-terminating character
  }

  // get string
  {
    LPSTR buffer = new TCHAR[size];
    GetEnvironmentVariable(name.c_str(), buffer, size);
    r = std::string(buffer);
    delete[] buffer;
  }
#endif  /* _WIN32 */

  return std::move(r);
}

bool Piyo::EnvironmentVariable::Windows::remove(const std::string& name)
{
  return Piyo::EnvironmentVariable::Windows::set(name, "");
}

