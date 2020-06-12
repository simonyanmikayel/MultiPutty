#include "stdafx.h"
#include "resource.h"

#include "TabView.h"
#include "MainFrame.h"
#include "KonsoleThread.h"

#define PUTTY_OFFSET_Y 0

//////////////////////////////////////////////////////////////////////////////

TabView::TabView(const ConsoleOptions& consoleOptions)
    : m_hwndConsoleView()
    , m_consoleOptions(consoleOptions)
    , m_puttyId()
    , m_bMatchCase(0)
    , m_bMatchWholeWord(0)
    ,m_wSearchTotal(0)
    , m_wSearchCur(0)
    , m_pKonsoleThread()
    , m_isActive(false)
{
    m_consoleOptions.pTabView = this;
#ifdef _USE_KONSOLE_THREAD
    m_pKonsoleThread = new KonsoleThread();
    m_pKonsoleThread->StartWork(&m_consoleOptions);
#else
    Create(
        m_consoleOptions.mainWnd,
        rcDefault,
        NULL,
        WS_CHILD | WS_VISIBLE,
        0,
        0U,
        0);
#endif

}

TabView::~TabView()
{
#ifdef _USE_KONSOLE_THREAD
  delete m_pKonsoleThread;
#endif
}

LRESULT TabView::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
#ifdef _USE_KONSOLE_THREAD
  ::PostMessage(m_hwndConsoleView, WM_CLOSE, 0, 0);
  while (::IsWindow(m_hwndConsoleView))
    Sleep(0);
  //delete m_pKonsoleThread;
  DestroyWindow();
  PostQuitMessage(0);
#endif
  return 1;
}

DWORD TabView::getTID()
{ 
#ifdef _USE_KONSOLE_THREAD
  return m_pKonsoleThread->getTID();
#else
  return GetCurrentThreadId();
#endif
}

//////////////////////////////////////////////////////////////////////////////

// TODO onClose PostQuitMessage 
LRESULT TabView::OnCreate(LPCREATESTRUCT lpcs)
{
  ModifyStyle(0, WS_CLIPCHILDREN);
  ModifyStyleEx(0, WS_EX_TRANSPARENT);

  CreateConsole();

  return 0; // windows sets focus to first control
}

void TabView::CreateConsole()
{
  m_hwndConsoleView = NULL;
#ifdef UNICODE
  char* cmd = w2m(m_consoleOptions.strInitialCmd.c_str());
  m_puttyId = PuttyDuplicate(m_hWnd, cmd);
  delete cmd;
#else
  m_puttyId = PuttyDuplicate(m_hWnd, m_consoleOptions.strInitialCmd.c_str());
#endif //UNICODE
}

LRESULT TabView::OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    return 1; // handled
}

LRESULT TabView::OnPositionChanging(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    WINDOWPOS*	pWinPos = reinterpret_cast<WINDOWPOS*>(lParam);
    if (!(pWinPos->flags & SWP_NOSIZE))
    {
      //::SetWindowPos(m_hwndConsoleView, m_hWnd, 0, 0, pWinPos->cx, pWinPos->cy, SWP_FRAMECHANGED | SWP_DEFERERASE);
      ::MoveWindow(m_hwndConsoleView, 0, PUTTY_OFFSET_Y, pWinPos->cx, pWinPos->cy - PUTTY_OFFSET_Y, TRUE);
    }
    return 1;
}

LRESULT TabView::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);
	
    if (!m_hwndConsoleView || !::IsWindow(m_hwndConsoleView))
    {
		DoPaint(hdc);
	}
	EndPaint(&ps);
	return 0;
}

void TabView::DoPaint(HDC hdc)
{
        CDCHandle dc = (HDC)hdc;
        RECT rect = { 0 };
        GetClientRect(&rect);
        dc.FillRect(&rect, COLOR_MENUTEXT);//COLOR_MENUTEXT COLOR_MENU
        if (m_hwndConsoleView && !::IsWindow(m_hwndConsoleView))
        {
          CString strError = _T("\nTerminal closed.\n\nPress 'Enter' to reopen.\n");
          rect.left += 10;
          rect.top += 10;
          ::DrawText(hdc, strError, strError.GetLength(), &rect, DT_WORDBREAK);
          //::ExtTextOut(hdc, 0, 0, ETO_CLIPPED, &rect, strError, strError.GetLength(), NULL);
          //dc.TextOut(10, 10, strError);
        }
}

void TabView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if ((!m_hwndConsoleView || !::IsWindow(m_hwndConsoleView)) && nChar == 13)
  {
    CreateConsole();
  }

}

void TabView::SetTitle(const tstring& strTitle)
{
  m_consoleOptions.strTitle = strTitle;
}

bool TabView::CloseView()
{
	//TODO
	return false;
}

void TabView::SetActive(bool bActive)
{
  m_isActive = bActive;
}

LRESULT TabView::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

  if (!m_hwndConsoleView || !::IsWindow(m_hwndConsoleView)) return 0;

  ::SetFocus(m_hwndConsoleView);
  return 0;
}

void TabView::PostMessageToConsoles(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (m_hwndConsoleView)
        ::PostMessage(m_hwndConsoleView, Msg, wParam, lParam);
}

LRESULT TabView::SendMessageToConsoles(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (m_hwndConsoleView)
        return ::SendMessage(m_hwndConsoleView, Msg, wParam, lParam);
    else
        return 0;
}

void TabView::SendCommandToConsoles(WORD wID)
{
    PostMessageToConsoles(WM_MENUCOMMAND, MSG_PUTTY_COMMAND, wID);
    //SendMessageToConsoles(WM_MENUCOMMAND, MSG_PUTTY_COMMAND, wID);
}

LRESULT TabView::OnMenuCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    switch (wParam)
    {
    case MSG_PUTTY_CONTEXT_MENU:
    {
      pWndMain->ShowPopupMenu();
    }
    case MSG_PUTTY_SEARCH_RESULT:
    {
        m_wSearchTotal = HIWORD(lParam);
        m_wSearchCur = LOWORD(lParam);
        pWndMain->UpdateSearchResults();
    }
    break;
    case MSG_PUTTY_HWND:
    {
        m_hwndConsoleView = (HWND)lParam;
        //::SetParent(m_hwndConsoleView, m_hWnd);

        //long style = ::GetWindowLong(m_hwndConsoleView, GWL_STYLE);
        //style |= WS_CHILD;
        //::SetWindowLong(m_hwndConsoleView, GWL_STYLE, style);

        //::ShowWindow(m_hwndConsoleView, SW_SHOW);//SW_SHOWMAXIMIZED

        CRect rectClient;
        GetClientRect(&rectClient);
        //::SetWindowPos(m_hwndConsoleView, m_hWnd, 0, PUTTY_OFFSET_Y, rectClient.Size().cx, rectClient.Size().cy - PUTTY_OFFSET_Y, SWP_FRAMECHANGED | SWP_DEFERERASE);
        ::MoveWindow(m_hwndConsoleView, 0, PUTTY_OFFSET_Y, rectClient.Size().cx, rectClient.Size().cy - PUTTY_OFFSET_Y, TRUE);
#ifdef _USE_KONSOLE_THREAD
        DWORD idConsoleView = GetWindowThreadProcessId(m_hwndConsoleView, NULL);
        if (idConsoleView)
        {
          AttachThreadInput(GetCurrentThreadId(), idConsoleView, TRUE);
        }
        m_pKonsoleThread->OnThreadReady();
#endif
        if (m_isActive)
          ::SetFocus(m_hwndConsoleView);
        return (LRESULT)pWndMain->m_hWnd;
    }
        break;
    default:
        break;
    }
    return 0;
}

LRESULT TabView::OnCopyData(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
  PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
  switch (pCDS->dwData)
  {
  case DATA_PUTTY_ERROR:
      CreateConsole();
    return 1;
  }
  return 0;
}