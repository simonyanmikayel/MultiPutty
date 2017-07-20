#include "stdafx.h"
#include "KonsoleThread.h"
#include "resource.h"

void KonsoleThread::Work(LPVOID pWorkParam)
{
  ConsoleOptions* pConsoleOptions = (ConsoleOptions*)pWorkParam;
  pConsoleOptions->pTabView->Create(
    pConsoleOptions->mainWnd,
    pConsoleOptions->pTabView->rcDefault,
    NULL,
    WS_CHILD | WS_VISIBLE,
    0,
    0U,
    0);

  MSG msg;
  BOOL bRet;
  while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
  {
    if (bRet == -1)
    {
      // handle the error and possibly exit
    }
    else
    {
      //if (
      //  WM_SYSKEYDOWN == msg.message ||
      //  WM_SYSKEYUP == msg.message ||
      //  WM_KEYDOWN == msg.message ||
      //  WM_KEYUP == msg.message
      //  )
      //{
      //  //stdlog("1 Focus=%d hwndHostMain=%d hwndHost=%d hwnd = %d\n", GetFocus(), hwndHostMain, hwndHost, hwnd);
      //  if (pConsoleOptions->pTabView->m_hwndConsoleView)
      //  {
      //    SendMessage(pConsoleOptions->pTabView->m_hwndConsoleView, msg.message, msg.wParam, msg.lParam);
      //    continue;
      //  }
      //}
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}