#pragma once

#include "Helpers.h"
#include "MainFrame.h"

class KonsoleThread;


class TabView
  : public CWindowImpl<TabView>
{
public:
    DECLARE_WND_CLASS_EX(_T("Console_2_TabView"), CS_DBLCLKS, COLOR_WINDOW)

  TabView(const ConsoleOptions& consoleOptions);
  ~TabView();

  BEGIN_MSG_MAP(TabView)
    MSG_WM_CREATE(OnCreate)
    MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnPositionChanging)
    MESSAGE_HANDLER(WM_MENUCOMMAND, OnMenuCommand)
    MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
    MESSAGE_HANDLER(WM_CLOSE, OnClose)
    MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
    MSG_WM_CHAR(OnChar)
  END_MSG_MAP()

  LRESULT OnCreate(LPCREATESTRUCT lpcs);
  LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
  LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnPositionChanging(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMenuCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
  LRESULT OnCopyData(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
  void TabView::DoPaint(HDC hdc);

  void SetTitle(const tstring& strTitle);
  const tstring& GetTitle() const { return m_consoleOptions.strTitle; }
  void SetActive(bool bActive);
  void PostMessageToConsoles(UINT Msg, WPARAM wParam, LPARAM lParam);
  LRESULT SendMessageToConsoles(UINT Msg, WPARAM wParam, LPARAM lParam);
  void SendCommandToConsoles(WORD wID);
  bool CloseView();
  void CreateConsole();
  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  DWORD getTID();

  HWND m_hwndConsoleView;
  BOOL m_bMatchCase;
  BOOL m_bMatchWholeWord;
  CStringW m_strFind;
  WORD m_wSearchTotal, m_wSearchCur;
private:
	ConsoleOptions      m_consoleOptions;
    DWORD m_puttyId;
    CString m_strError;
    KonsoleThread* m_pKonsoleThread;
    bool m_isActive;
};

//////////////////////////////////////////////////////////////////////////////
