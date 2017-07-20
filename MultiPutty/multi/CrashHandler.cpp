#include "stdafx.h"
#include <signal.h>
#include "StackWalker.h"

int CLOSING = 0;

// Simple implementation of an additional output to the console:
class MyStackWalker : public StackWalker
{
public:
  MyStackWalker() : StackWalker() {}
  ~MyStackWalker(){ if (fp) fclose(fp); }
  void CreateReportFile(){
    char szReportPath[2 * MAX_PATH + 1];
    GetModuleFileNameA(NULL, szReportPath, 2 * MAX_PATH);
    char *p = strrchr(szReportPath, '\\');
    p[0] = 0;
    strcat(szReportPath, "\\DebugInfo");
    time_t t = time(0);
    char file_path[MAX_PATH];
    CreateDirectoryA(szReportPath, NULL);
    tm *pTime = localtime(&t);
    sprintf(file_path, "%s\\CrashReport.%02d.%02d.%02d.%02d.%010ud.txt", szReportPath, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec, GetTickCount());
    fp = fopen(file_path, "w");
  }
  MyStackWalker(DWORD dwProcessId, HANDLE hProcess) : StackWalker(dwProcessId, hProcess) {}
  virtual void OnOutput(LPCSTR szText) {
    if (!fp)
      CreateReportFile();
    if (fp) {
      fputs(szText, fp);
      fputs("\r\n", fp); 
    }
  }
  FILE *fp;
};

#ifdef _DEBUG
MyStackWalker sw;
#endif

void print_stack_and_exit(char* reason)
{
#ifdef _DEBUG
  if (!CLOSING) {
    if (fp_putty_mem_log) {
      fputs("\r\n", fp_putty_mem_log);
      fputs(reason, fp_putty_mem_log);
      fflush(fp_putty_mem_log);
      fclose(fp_putty_mem_log);
    }
    sw.OnOutput(reason);
    sw.ShowCallstack();
    fatal_error(reason);
  }
#endif
}

LONG CALLBACK VectoredHandler(_In_ PEXCEPTION_POINTERS ExceptionInfo)
{
  print_stack_and_exit("VectoredHandler:");
  //StackWalk64
  return 0;
}

void signal_handler(int i)
{
  // Caught SIGABRT C++ signal
  // Terminate program
  print_stack_and_exit("signal_handler:");
}

void SetCrashHandlers()
{
  AddVectoredExceptionHandler(FALSE, VectoredHandler);
  signal(SIGABRT, signal_handler);//Abnormal termination
  signal(SIGFPE, signal_handler);//Floating - point error
  signal(SIGILL, signal_handler);//Illegal instruction
  signal(SIGINT, signal_handler);//CTRL + C signal
  signal(SIGSEGV, signal_handler);//Illegal storage access
  signal(SIGTERM, signal_handler);//Termination request
}
