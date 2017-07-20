// MultiPutty.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MultiPutty.h"
#include "MainFrame.h"
#include "Helpers.h"

//////////////////////////////////////////////////////////////////////////////
// Global variables

CAppModule					_Module;
MainFrame*                  pWndMain;


int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    // create main window
    MainFrame wndMain(lpstrCmdLine);
    pWndMain = &wndMain;

    if (wndMain.CreateEx() == NULL)
    {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 1;
    }

    //wndMain.ShowWindow(SW_SHOWDEFAULT);
    wndMain.ShowWindow(nCmdShow);


    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();
    return nRet;
}

//////////////////////////////////////////////////////////////////////////////

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{

  SetCrashHandlers();

#ifdef UNICODE
  char* cmd = w2m(lpstrCmdLine);
  if (!PuttyWinMain(hInstance, cmd))
#else
  if (!PuttyWinMain(hInstance, lpstrCmdLine))
#endif //UNICODE
    {
#ifdef UNICODE
      delete cmd;
#endif //UNICODE
      // this is not a new_multi_putti
      return PuttyStart();
    }
#ifdef UNICODE
  delete cmd;
#endif //UNICODE

  //stdlog("OK\n");

    // CoInitializeEx(COINIT_MULTITHREADED) can affect functions that rely on shell objects
    HRESULT hRes = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    ATLASSERT(SUCCEEDED(hRes));


    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();

    ::CoUninitialize();

    PuttyCleanup();

    return nRet;
}
