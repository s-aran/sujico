#include "Header.h"
#include "Executor.h"


int32_t Piyo::Executor::ExecutorWindows::execute(const std::string& commandLine, const std::string& currentDirectory)
{
  // based: http://www.usamimi.info/~hellfather/win32api/API_CreatePipe.xml

  static constexpr int TimeoutMs = 10 * 1000;
  int32_t r = -1;
  LPTSTR buffer = new TCHAR[this->getMaxBuffers()];
  ZeroMemory(buffer, this->getMaxBuffers());
  {
    HANDLE read, write;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD len = 0;

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    if (!CreatePipe(&read, &write, &sa, 0))
    {
      return -1;
    }

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;

    si.hStdOutput = write;


    LPTSTR cmd = new TCHAR[commandLine.length() + 1];
    {
      lstrcpy(cmd, commandLine.c_str());

      do
      {
        if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        {
          std::cout << GetLastError() << std::endl;
          break;
        }

        FlushFileBuffers(read);
        FlushFileBuffers(write);

        if (WaitForInputIdle(pi.hProcess, TimeoutMs) != 0)
        {
          if (GetLastError() != ERROR_NOT_GUI_PROCESS)
          {
            break;
          }
        }

        if (WaitForSingleObject(pi.hProcess, TimeoutMs) != WAIT_OBJECT_0)
        {
          break;
        }

        DWORD result = 0;
        if (GetExitCodeProcess(pi.hProcess, &result) == TRUE)
        {
          if (result == STILL_ACTIVE)
          {
            continue;
          }

          r = result;
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        if (!PeekNamedPipe(read, NULL, 0, NULL, &len, NULL))
        {
          break;
        }

        if (len > 0 && !ReadFile(read, buffer, this->getMaxBuffers() - 1, &len, NULL))
        {
          break;
        }
      } while (0);

      CloseHandle(read);
      CloseHandle(write);
    }
    delete[] cmd;
  }
  this->setStdout(std::string(buffer));
  delete[] buffer;


  return r;
}
