#include "stdafx.h"
#include "MainFrame.h"
#include "aboutdlg.h"
#include "TabView.h"
#include "DlgRenameTab.h"

//we are not destributing udp commands
// note: currently we are destributing debug build due to issue with menu highlitenug and "tab rename dialog" showing issue
#ifdef _DEBUG
#define USE_COMMAND_THREAD
#endif

//////////////////////////////////////////////////////////////////////////////
WNDPROC oldEditProc;
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
//////////////////////////////////////////////////////////////////////////////


MainFrame::MainFrame
(
	LPCTSTR lpstrCmdLine
)
: m_bOnCreateDone(false)

, m_activeTabView()
,m_dwNextTabNum(0)
, m_bMenuVisible     (true)
, m_bMenuChecked     (true)
, m_tabs()
, m_dwWindowWidth(0)
, m_dwWindowHeight(0)
, m_dwResizeWindowEdge(WMSZ_BOTTOM)
, m_bRestoringWindow(false)
, m_bShowingHidingWindow(false)
, m_hwndPreviousForeground(NULL)
, m_pCommandThread(NULL)
, m_uUdpCommand(::RegisterWindowMessage(TEXT("UdpCommand")))
{
	m_Margins.cxLeftWidth    = 0;
	m_Margins.cxRightWidth   = 0;
	m_Margins.cyTopHeight    = 0;
	m_Margins.cyBottomHeight = 0;  
}

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
  if (CTabbedFrameImpl<MainFrame>::PreTranslateMessage(pMsg)) return TRUE;
  if (pMsg->message == m_uUdpCommand) { OnUdpCommand(pMsg->wParam, pMsg->lParam);  return TRUE; }


  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void MainFrame::UpdateSearchResults()
{
    if (m_activeTabView)
    {
        TCHAR szText[32];
        _stprintf_s(szText, _T("%d of %d"), m_activeTabView->m_wSearchCur, m_activeTabView->m_wSearchTotal);
        m_searchResult.SetWindowText(szText);
    }
}

BOOL MainFrame::OnIdle()
{
  //ActivatePutty(false);
  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  HWND hWndAfter;
  if (m_activeTabView && ::IsWindow(m_activeTabView->m_hwndConsoleView))
    hWndAfter = m_activeTabView->m_hwndConsoleView;
  else
    hWndAfter = NULL;

  CAboutDlg dlg;
  dlg.DoModal(m_hWnd);

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT MainFrame::CreateInitialTabs
(
	const CommandLineOptions& commandLineOptions
)
{
#ifdef USE_COMMAND_THREAD
  m_pCommandThread = new CommandThread();
  m_pCommandThread->StartWork(0);
#endif

	bool bAtLeastOneStarted = false;

    // TODO - get tabs from Putty
    ConsoleOptions consoleOptions;

    for (int i = 0;; i++) {
      char* sz_val = get_multi_session(i);
      if (!sz_val)
        break;
      char *title = strdup(sz_val);
      char *command = strchr(title, '\t');
      if (!command)
        command = "";
      else
        *(command++) = 0;

      bAtLeastOneStarted = true;
#ifdef UNICODE
      wchar_t* wtitle = m2w(title);
      wchar_t* wcommand = m2w(command);
      consoleOptions.strTitle = wtitle;
      consoleOptions.strInitialCmd = wcommand;
      delete wtitle;
      delete wcommand;
#else
      consoleOptions.strTitle = title;
      consoleOptions.strInitialCmd = command;
#endif //UNICODE
      CreateNewTab(consoleOptions);
      free(title);
      //Sleep(200);
    };

    if (!bAtLeastOneStarted)
    {
        consoleOptions.strTitle = _T("");
        consoleOptions.strInitialCmd = _T("");
        CreateNewTab(consoleOptions);
    }

	return 0;
}

LRESULT MainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
#ifdef UNICODE
  wchar_t* wname = m2w(get_config_name());
  wchar_t* whost = m2w(get_host_name());
  m_strWindowTitle += wname;
  if (m_strWindowTitle.length() > 0)
      m_strWindowTitle += _T(" - ");
  m_strWindowTitle += whost;
  delete wname;
  delete whost;
#else
  m_strWindowTitle += get_config_name();
  if (m_strWindowTitle.length() > 0)
      m_strWindowTitle += _T(" - ");
  m_strWindowTitle += get_host_name();
#endif //UNICODE
  if (m_strWindowTitle.length() > 0)
    m_strWindowTitle += _T(" - ");

    m_strWindowTitle += PROG_NAME;
    SetWindowText(m_strWindowTitle.c_str());

	// add opened tabs submenu
	// you cannot define an empty submenu in resource file
	CMenuHandle menu(GetMenu());

	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(menu);
	// remove old menu
	SetMenu(NULL);

	m_tabsRPopupMenu.LoadMenu(IDR_TAB_POPUP_MENU);

	// TODO UpdateMenuHotKeys();

  m_contextMenu.CreatePopupMenu();
  CMenuHandle mainMenu = m_CmdBar.GetMenu();
  int count = mainMenu.GetMenuItemCount();
  for (int i = 0; i < count; ++i)
  {
    CString title;
    mainMenu.GetMenuString(i, title, MF_BYPOSITION);
    m_contextMenu.InsertMenu(i, MF_BYPOSITION, mainMenu.GetSubMenu(i), title);
  }

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);

  AddSimpleReBarBand(hWndCmdBar, NULL, FALSE);

	SizeSimpleReBarBands();

  HWND hWndToolBar2 = CreateSimpleToolBarCtrl(m_hWnd, Helpers::GetHighDefinitionResourceId(IDR_SEARCH_16), FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
  m_searchbar = hWndToolBar2;
  AddSimpleReBarBand(hWndToolBar2, NULL, FALSE);
  SizeSimpleReBarBands();


  TBBUTTONINFO tbi;
  tbi.cbSize = sizeof(TBBUTTONINFO);
  tbi.dwMask = TBIF_SIZE | TBIF_STATE | TBIF_STYLE;

  // Make sure the underlying button is disabled
  tbi.fsState = 0;
  // BTNS_SHOWTEXT will allow the button size to be altered
  tbi.fsStyle = BTNS_SHOWTEXT;
  tbi.cx = static_cast<WORD>(35 * ::GetSystemMetrics(SM_CXSMICON));

  m_searchbar.SetButtonInfo(ID_SEARCH_COMBO, &tbi);

  SizeSimpleReBarBands();

  // Get the button rect
  CRect rcCombo, rcButton0, rcSearcResult;
  m_searchbar.GetItemRect(0, rcButton0);

  rcSearcResult = rcButton0;
  rcSearcResult.top += 4;
  rcSearcResult.right = rcSearcResult.left + 4 * GetSystemMetrics(SM_CXSMICON) - 4;
  m_searchResult.Create(m_hWnd, rcSearcResult, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_RIGHT);// | WS_BORDER
  m_searchResult.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT), FALSE);
  m_searchResult.SetParent(hWndToolBar2);

  rcCombo = rcButton0;
  rcCombo.left += rcSearcResult.Width() + 4;

  // create search bar combo
  DWORD dwComboStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | CBS_DROPDOWN | CBS_AUTOHSCROLL;

  //m_cb.m_hWnd = ::CreateWindowExW(0, WC_COMBOBOXEXW, L"", dwComboStyle,
  //  rcCombo.left, rcCombo.top, rcCombo.right - rcCombo.left,
  //  rcCombo.bottom - rcCombo.top, m_hWnd, NULL,
  //  _AtlBaseModule.GetModuleInstance(), NULL);

  m_cb.Create(m_hWnd, rcCombo, NULL, dwComboStyle);
  m_cb.SetParent(hWndToolBar2);

  // set 15 lines visible in combo list
  m_cb.GetComboCtrl().ResizeClient(rcCombo.Width(), rcCombo.Height() + 15 * m_cb.GetItemHeight(0));
  m_searchbox = m_cb.GetComboCtrl();
  m_searchedit = m_cb.GetEditCtrl();
  oldEditProc = (WNDPROC)m_searchedit.SetWindowLongPtr(GWLP_WNDPROC, (LONG_PTR)subEditProc);
  //m_searchedit.SetCueBannerText(L"Search...");

  // The combobox might not be centred vertically, and we won't know the
  // height until it has been created.  Get the size now and see if it
  // needs to be moved.
  CRect rectToolBar;
  CRect rectCombo;
  m_searchbar.GetClientRect(&rectToolBar);
  m_cb.GetWindowRect(rectCombo);

  // Get the different between the heights of the toolbar and
  // the combobox
  int nDiff = rectToolBar.Height() - rectCombo.Height();
  // If there is a difference, then move the combobox
  if (nDiff > 1)
  {
    m_searchbar.ScreenToClient(&rectCombo);
    m_cb.MoveWindow(rectCombo.left, rectCombo.top + (nDiff / 2), rectCombo.Width(), rectCombo.Height());
  }

	// initialize tabs
	SetReflectNotifications(true);

  DWORD dwTabStyles = CTCS_TOOLTIPS | CTCS_DRAGREARRANGE | CTCS_HOTTRACK | CTCS_SCROLL | CTCS_CLOSEBUTTON;
#ifndef _KONSOLE_TAB
  dwTabStyles |= CTCS_CLOSEBUTTON;
#endif
	if (1) dwTabStyles |= CTCS_BOTTOM; // TODO
	if (0) dwTabStyles |= CTCS_CLOSELASTTAB; // TODO

	CreateTabWindow(m_hWnd, rcDefault, dwTabStyles);

  CreateInitialTabs(m_commandLineOptions);

	UISetBlockAccelerators(true);

	SetWindowStyles();

    m_bMenuChecked = true; // TODO controlsSettings.ShowMenu();
	//ShowMenu(m_bMenuChecked);

    //UISetCheck(ID_SEARCH_MATCH_CASE, true); // TODO searchSettings.bMatchCase);
    //UISetCheck(ID_SEARCH_MATCH_WHOLE_WORD, true); // TODO searchSettings.bMatchWholeWord);

	DWORD dwFlags	= SWP_NOSIZE|SWP_NOZORDER;
    /* TODO 
	if( !positionSettings.bSavePosition &&
	    (positionSettings.nX == -1 || positionSettings.nY == -1) )
	{
		// do not reposition the window
		dwFlags |= SWP_NOMOVE;
	}
	else
	{
		// check we're not out of desktop bounds 
		int	nDesktopLeft	= ::GetSystemMetrics(SM_XVIRTUALSCREEN);
		int	nDesktopTop		= ::GetSystemMetrics(SM_YVIRTUALSCREEN);

		int	nDesktopRight	= nDesktopLeft + ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int	nDesktopBottom	= nDesktopTop + ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		if ((positionSettings.nX < nDesktopLeft) || (positionSettings.nX > nDesktopRight)) positionSettings.nX = 50;
		if ((positionSettings.nY < nDesktopTop) || (positionSettings.nY > nDesktopBottom)) positionSettings.nY = 50;
	}

	SetTransparency();
	SetWindowPos(NULL, positionSettings.nX, positionSettings.nY, 0, 0, dwFlags);
	DockWindow(positionSettings.dockPosition);
	SetZOrder(positionSettings.zOrder);
    */


	AdjustWindowSize(ADJUSTSIZE_NONE);

	CRect rectWindow;
	GetWindowRect(&rectWindow);

	m_dwWindowWidth	= rectWindow.Width();
	m_dwWindowHeight= rectWindow.Height();

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	// this is the only way I know that other message handlers can be aware 
	// if they're being called after OnCreate has finished
	m_bOnCreateDone = true;
    /* TODO 
	if( positionSettings.bSaveSize ||
	    (positionSettings.nW != -1 && positionSettings.nH != -1) )
	{
		// resize the window
		SetWindowPos(NULL, 0, 0, positionSettings.nW, positionSettings.nH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
		AdjustWindowSize(ADJUSTSIZE_WINDOW);
	}

	if( g_settingsHandler->GetAppearanceSettings().fullScreenSettings.bStartInFullScreen )
		ShowFullScreen(true);
    */

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////


LRESULT MainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{   
  /*~~
	if(g_settingsHandler->GetBehaviorSettings().closeSettings.bConfirmClosingMultipleViews)
	{
		MutexLock lock(m_tabsMutex);

		if(m_tabs.size() > 1)
		{
			if(MessageBox(Helpers::LoadString(MSG_MAINFRAME_CLOSE_ALL_TABS).c_str(), L"MultuPutty", MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
				return 0;
		}
		else if(m_tabs.size() == 1 && m_tabs.begin()->second->GetViewsCount() > 1)
		{
			if(MessageBox(Helpers::LoadString(MSG_MAINFRAME_CLOSE_ALL_VIEWS).c_str(), L"MultuPutty", MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
				return 0;
		}
	}

	// save settings on exit
	bool				bSaveSettings		= false;
	ConsoleSettings&	consoleSettings		= g_settingsHandler->GetConsoleSettings();
	PositionSettings&	positionSettings	= g_settingsHandler->GetAppearanceSettings().positionSettings;

	if (consoleSettings.bSaveSize)
	{
#if 0
		consoleSettings.dwRows		= m_dwRows;
		consoleSettings.dwColumns	= m_dwColumns;
#endif
		bSaveSettings = true;
	}

	if (positionSettings.bSavePosition || positionSettings.bSaveSize)
	{
		// we store position and size of the non fullscreen window
		if( !m_bFullScreen )
			GetWindowRect(&m_rectWndNotFS);

		if (positionSettings.bSavePosition)
		{
			positionSettings.nX = m_rectWndNotFS.left;
			positionSettings.nY = m_rectWndNotFS.top;
		}

		if (positionSettings.bSaveSize)
		{
			positionSettings.nW = m_rectWndNotFS.Width();
			positionSettings.nH = m_rectWndNotFS.Height();
		}

		bSaveSettings = true;
	}


	if (bSaveSettings) g_settingsHandler->SaveSettings();

	SaveSearchMRU();


	if (g_settingsHandler->GetAppearanceSettings().stylesSettings.bTrayIcon) SetTrayIcon(NIM_DELETE);

    ~~*/

  CLOSING = 1;
  // destroy all views
  for (TabViewMap::iterator it = m_tabs.begin(); it != m_tabs.end(); ++it)
  {
    RemoveTab(it->second->m_hWnd);
    it->second->DestroyWindow();
  }
  m_activeTabView = NULL;
  if (m_pCommandThread)
    m_pCommandThread->Terminate();
  delete m_pCommandThread;

  DestroyWindow();
  PostQuitMessage(0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////


LRESULT MainFrame::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Start timer that will force a call to ResizeWindow (called from WM_EXITSIZEMOVE handler
	// when the MultuPutty window is resized using a mouse)
	// External utilities that might resize MultuPutty window usually don't send WM_EXITSIZEMOVE
	// message after resizing a window.
	if(wParam == 0)
	{
		SetTimer(TIMER_SIZING, TIMER_SIZING_INTERVAL);
	}

	if (wParam == SIZE_MAXIMIZED)
	{
		PostMessage(WM_EXITSIZEMOVE, 1, 0);
	}
	else if (m_bRestoringWindow && (wParam == SIZE_RESTORED))
	{
		m_bRestoringWindow = false;
		PostMessage(WM_EXITSIZEMOVE, 1, 0);
	}

// 	CRect rectWindow;
// 	GetWindowRect(&rectWindow);
// 


	bHandled = FALSE;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnSizing(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_dwResizeWindowEdge = static_cast<DWORD>(wParam);

	if (!m_activeTabView)
		return 0;

	// TODO m_activeTabView->SetResizing(true);
#if 0
	CPoint pointSize = m_activeView->GetCellSize();
	RECT *rectNew = (RECT *)lParam;

	CRect rectWindow;
	GetWindowRect(&rectWindow);

	if (rectWindow.top != rectNew->top)
		rectNew->top += (rectWindow.top - rectNew->top) - (rectWindow.top - rectNew->top) / pointSize.y * pointSize.y;

	if (rectWindow.bottom != rectNew->bottom)
		rectNew->bottom += (rectWindow.bottom - rectNew->bottom) - (rectWindow.bottom - rectNew->bottom) / pointSize.y * pointSize.y;

	if (rectWindow.left != rectNew->left)
		rectNew->left += (rectWindow.left - rectNew->left) - (rectWindow.left - rectNew->left) / pointSize.x * pointSize.x;

	if (rectWindow.right != rectNew->right)
		rectNew->right += (rectWindow.right - rectNew->right) - (rectWindow.right - rectNew->right) / pointSize.x * pointSize.x;
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////


LRESULT MainFrame::OnMouseButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (::GetCapture() == m_hWnd)
	{
		::ReleaseCapture();
	}
	else
	{
		bHandled = FALSE;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnExitSizeMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ResizeWindow();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

void MainFrame::ResizeWindow()
{
    CRect rectWindow;
    GetWindowRect(&rectWindow);

    DWORD dwWindowWidth = rectWindow.Width();
    DWORD dwWindowHeight = rectWindow.Height();

    if ((dwWindowWidth != m_dwWindowWidth) ||
        (dwWindowHeight != m_dwWindowHeight))
    {
        AdjustWindowSize(ADJUSTSIZE_WINDOW);
    }

    SendMessage(WM_NULL, 0, 0);
    m_dwResizeWindowEdge = WMSZ_BOTTOM;

    // TODO if (m_activeTabView) m_activeTabView->SetResizing(false);
}

//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (wParam == TIMER_SIZING)
	{
		KillTimer(TIMER_SIZING);
		ResizeWindow();
	}

	return 0;
}

LRESULT MainFrame::OnConsoleResized(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /* bHandled */)
{
	AdjustWindowSize(ADJUSTSIZE_NONE);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void MainFrame::ShowPopupMenu()
{
  POINT point;
  GetCursorPos(&point);
  m_CmdBar.TrackPopupMenu(m_contextMenu.GetSubMenu(1), 0, point.x, point.y);
  //m_CmdBar.TrackPopupMenu(m_contextMenu, 0, point.x, point.y);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnStartMouseDrag(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// do nothing for minimized or maximized or fullscreen windows
	if (IsIconic() || IsZoomed() ) return 0;

	ReleaseCapture();
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnTabChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	NMCTC2ITEMS*				pTabItems	= reinterpret_cast<NMCTC2ITEMS*>(pnmh);

	CTabViewTabItem*			pTabItem1	= (pTabItems->iItem1 != 0xFFFFFFFF) ? m_TabCtrl.GetItem(pTabItems->iItem1) : NULL;
	CTabViewTabItem*			pTabItem2	= m_TabCtrl.GetItem(pTabItems->iItem2);

	TabViewMap::iterator	it;

	if (pTabItem1)
	{
		it = m_tabs.find(pTabItem1->GetTabView());
		if (it != m_tabs.end())
		{
      SetActiveTab(it->second, false);
		}
	}

	if (pTabItem2)
	{
		it = m_tabs.find(pTabItem2->GetTabView());
		if (it != m_tabs.end())
		{
      SetActiveTab(it->second, true);

			// clear the highlight in case it's on
			HighlightTab(m_activeTabView->m_hWnd, false);
		}
		else
		{
      m_activeTabView = std::shared_ptr<TabView>();
		}
	}

	UpdateUI();

	bHandled = FALSE;
	return 0;
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnTabClose(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /* bHandled */)
{
	NMCTC2ITEMS*		pTabItems	= reinterpret_cast<NMCTC2ITEMS*>(pnmh);
	CTabViewTabItem*	pTabItem	= (pTabItems->iItem1 != 0xFFFFFFFF) ? m_TabCtrl.GetItem(pTabItems->iItem1) : NULL;

	CloseTab(pTabItem);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnTabMiddleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMCTCITEM*       pTabItems = reinterpret_cast<NMCTCITEM*>(pnmh);
	CTabViewTabItem* pTabItem  = (pTabItems->iItem != 0xFFFFFFFF) ? m_TabCtrl.GetItem(pTabItems->iItem) : NULL;

	if (pTabItem == NULL)
	{

		if (!m_tabsMenu.IsNull())
		{
			CPoint point(pTabItems->pt.x, pTabItems->pt.y);
			CPoint screenPoint(point);
			this->m_TabCtrl.ClientToScreen(&screenPoint);
			m_CmdBar.TrackPopupMenu(m_tabsMenu, 0, screenPoint.x, screenPoint.y);
		}
	}
	else
	{
		CloseTab(pTabItem);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

  if (!m_activeTabView) return 0;

  //if (GetActiveWindow() == m_hWnd)
  ::SetFocus(m_activeTabView->m_hWnd);
  return 0;
}

LRESULT MainFrame::OnTabRightClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	if( idCtrl != m_TabCtrl.GetDlgCtrlID() )
	{
		bHandled = FALSE;
		return 0;
	}

	NMCTCITEM*       pTabItems = reinterpret_cast<NMCTCITEM*>(pnmh);
	CTabViewTabItem* pTabItem  = (pTabItems->iItem != 0xFFFFFFFF) ? m_TabCtrl.GetItem(pTabItems->iItem) : NULL;

	if (pTabItem)
	{
		// select the tab
		// this will update the menu UI
		m_TabCtrl.SetCurSel(pTabItems->iItem);

		CPoint point(pTabItems->pt.x, pTabItems->pt.y);
		CPoint screenPoint(point);
		this->m_TabCtrl.ClientToScreen(&screenPoint);
		m_CmdBar.TrackPopupMenu(m_tabsRPopupMenu.GetSubMenu(0), 0, screenPoint.x, screenPoint.y);
	}

	// tab is already selected so return 1
	return 1;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnRebarHeightChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	AdjustWindowSize(ADJUSTSIZE_WINDOW);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnFileNewTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ConsoleOptions consoleOptions;
    CreateNewTab(consoleOptions);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT MainFrame::OnSearchStart(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    SetForegroundWindow(m_hWnd);
    m_searchedit.SetFocus();
    return 0;
}

void MainFrame::UpdateSerchList(WCHAR* szText)
{
  if (!szText[0])
    return;
#ifdef UNICODE
  tstring str = szText;
  int i = m_cb.FindStringExact(0, str.c_str());
  if (i >= 0)
    m_cb.DeleteString(i);
  COMBOBOXEXITEMW item = { 0 };
  item.mask = CBEIF_TEXT;
  item.pszText = (WCHAR*)str.c_str();
  item.iItem = 0;
  m_cb.InsertItem(&item);
  if (m_cb.GetCount() > 15) {
    m_cb.DeleteItem(15);
    m_cb.SetCurSel(0);
  }
#endif //UNICODE
}

LRESULT MainFrame::OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (!m_activeTabView || !m_activeTabView->m_hwndConsoleView) return 0;
  if (wID == ID_SEARCH_CLEAR)
  {
    ::SetWindowTextW(m_searchedit.m_hWnd, L"");
    m_activeTabView->m_strFind = L"";
  }
  COPYDATASTRUCT cds;
  WORD wSearchSettings = 0;
  if (m_activeTabView->m_bMatchCase)
    wSearchSettings |= SEARCH_MATCHCASE;
  if (m_activeTabView->m_bMatchWholeWord)
    wSearchSettings |= SEARCH_MATCHWHOLEWORD;
  cds.dwData = MAKEWPARAM(wID, wSearchSettings);
  cds.cbData = (m_activeTabView->m_strFind.GetLength() + 1) * sizeof(wchar_t);
  cds.lpData = m_activeTabView->m_strFind.GetBuffer();
  //if (wID == ID_SEARCH_CLEAR) cds.lpData = 0; cds.cbData = 100;
  LRESULT lr = SendMessage(m_activeTabView->m_hwndConsoleView, WM_COPYDATA, (WPARAM)(HWND)m_activeTabView->m_hWnd, (LPARAM)(LPVOID)&cds);
  SetFocus();
  UpdateSerchList(m_activeTabView->m_strFind.GetBuffer());
  return 0;
}

LRESULT MainFrame::OnSearchSettings(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
  if (!m_activeTabView) return 0;
  switch (wID)
  {
  case ID_SEARCH_MATCH_CASE:
    m_activeTabView->m_bMatchCase = !m_activeTabView->m_bMatchCase;
    m_searchbar.CheckButton(ID_SEARCH_MATCH_CASE, m_activeTabView->m_bMatchCase);
    break;

  case ID_SEARCH_MATCH_WHOLE_WORD:
    m_activeTabView->m_bMatchWholeWord = !m_activeTabView->m_bMatchWholeWord;
    m_searchbar.CheckButton(ID_SEARCH_MATCH_WHOLE_WORD, m_activeTabView->m_bMatchWholeWord);
    break;
  }
  PostMessage(WM_COMMAND, ID_SEARCH_REFRESH, 0);
  //OnSearch(0, wID, 0, bHandled);
  return 0;
}
 
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CUT:
    case WM_PASTE:
    case WM_CLEAR:
    case WM_UNDO:
    case WM_KEYUP:
    case WM_IME_NOTIFY:
        if (pWndMain->m_activeTabView)
        {
            LRESULT lr = CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
            wchar_t text[256];
            ::GetWindowTextW(pWndMain->m_searchedit.m_hWnd, text, sizeof(text) / sizeof(text[0]) - 1);
            text[sizeof(text) / sizeof(text[0]) - 1] = 0;
            pWndMain->m_activeTabView->m_strFind = text;
            return lr;
        }
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
          ::PostMessage(pWndMain->m_hWnd, WM_COMMAND, ID_SEARCH_REFRESH, 0);
            return 0; //if you don't want to pass it further to def proc
            //break;  //If not your key, skip to default:
        case VK_ESCAPE:
            pWndMain->SetFocus();
            return 0; 
        }
    default:
        return CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
    }
    return 0;
}
//////////////////////////////////////////////////////////////////////////////

void MainFrame::SetActiveTab(std::shared_ptr<TabView> tabView, bool bActive)
{
  tabView->SetActive(bActive);
  if (bActive)
  {
    m_activeTabView = tabView;
    UpdateSearchResults();
    m_searchbar.CheckButton(ID_SEARCH_MATCH_CASE, m_activeTabView->m_bMatchCase);
    m_searchbar.CheckButton(ID_SEARCH_MATCH_WHOLE_WORD, m_activeTabView->m_bMatchWholeWord);
    ::SetWindowTextW(m_searchedit.m_hWnd, m_activeTabView->m_strFind);
  }
}

LRESULT MainFrame::OnSwitchTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nNewSel = wID-ID_SWITCH_TAB_1;

	if (nNewSel >= m_TabCtrl.GetItemCount()) return 0;

	m_TabCtrl.SetCurSel(nNewSel);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnFileCloseTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTabViewTabItem* pCurSelTabItem = m_TabCtrl.GetItem(m_TabCtrl.GetCurSel());
	if (!pCurSelTabItem) return 0;

	switch(wID)
	{
	case ID_FILE_CLOSE_TAB:
		CloseTab(pCurSelTabItem);
		break;

	case ID_FILE_CLOSE_ALL_TABS_BUT_THIS:
	case ID_FILE_CLOSE_ALL_TABS_LEFT:
	case ID_FILE_CLOSE_ALL_TABS_RIGHT:
		{

			// close all to the left
			if( wID == ID_FILE_CLOSE_ALL_TABS_BUT_THIS ||
			    wID == ID_FILE_CLOSE_ALL_TABS_LEFT )
			{
				for(;;)
				{
					if( m_TabCtrl.GetItemCount() == 0 )
						break;

					CTabViewTabItem* pMostLeftTabItem = m_TabCtrl.GetItem(0);

					if( pMostLeftTabItem == pCurSelTabItem )
						break;

					CloseTab(pMostLeftTabItem->GetTabView());
				}
			}

			// close all to the right
			if( wID == ID_FILE_CLOSE_ALL_TABS_BUT_THIS ||
			    wID == ID_FILE_CLOSE_ALL_TABS_RIGHT )
			{
				for(;;)
				{
					if( m_TabCtrl.GetItemCount() == 0 )
						break;

					CTabViewTabItem* pMostRightTabItem = m_TabCtrl.GetItem(m_TabCtrl.GetItemCount() - 1);

					if( pMostRightTabItem == pCurSelTabItem )
						break;

					CloseTab(pMostRightTabItem->GetTabView());
				}
			}


				/*
				if( !g_settingsHandler->GetBehaviorSettings().closeSettings.bAllowClosingLastView )
				if (m_tabs.size() <= 1) return;
				*/
		}
		break;
	}

	return 0;
}


LRESULT MainFrame::OnNextTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nCurSel = m_TabCtrl.GetCurSel();

	if (++nCurSel >= m_TabCtrl.GetItemCount()) nCurSel = 0;
	m_TabCtrl.SetCurSel(nCurSel);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnPrevTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nCurSel = m_TabCtrl.GetCurSel();

	if (--nCurSel < 0) nCurSel = m_TabCtrl.GetItemCount() - 1;
	m_TabCtrl.SetCurSel(nCurSel);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnMoveTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nCurSel = m_TabCtrl.GetCurSel();

	switch( wID )
	{
	case ID_MOVE_TAB_LEFT:
		if( nCurSel > 0 )
			m_TabCtrl.MoveItem(nCurSel, nCurSel - 1);
		break;

	case ID_MOVE_TAB_RIGHT:
		if( nCurSel < (m_TabCtrl.GetItemCount() - 1) )
			m_TabCtrl.MoveItem(nCurSel, nCurSel + 1);
		break;
	}

	return 0;
}




LRESULT MainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT MainFrame::OnPuttyCommand(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (m_activeTabView)
    m_activeTabView->SendCommandToConsoles(wID);
  return 0;
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnEditRenameTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (!m_activeTabView) return 0;

  SetActiveWindow();
  DlgRenameTab dlg(CString(m_activeTabView->GetTitle().c_str()));

  if (dlg.DoModal() == IDOK)
  {
    m_activeTabView->SetTitle(tstring(dlg.m_strTabName));
    UpdateTabText(*m_activeTabView, tstring(dlg.m_strTabName).c_str());
  }

  return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

LRESULT MainFrame::OnEditSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!m_activeTabView) return 0;
    // TODO
	return 0;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void MainFrame::AdjustWindowRect(CRect& rect)
{
	AdjustWindowRectEx(&rect, GetWindowLong(GWL_STYLE), FALSE, GetWindowLong(GWL_EXSTYLE));

	// adjust for the toolbar height
	CReBarCtrl	rebar(m_hWndToolBar);
	rect.bottom	+= rebar.GetBarHeight() - 4;

	rect.bottom	+= GetTabAreaHeight(); //+0
}

//////////////////////////////////////////////////////////////////////////////

bool MainFrame::CreateNewTab(ConsoleOptions& consoleOptions)
{
    if (consoleOptions.strTitle.empty())
    {
        TCHAR szTitle[32];
        _stprintf_s(szTitle, _countof(szTitle), _T("Console %d"), ++m_dwNextTabNum);
        consoleOptions.strTitle = szTitle;
    }

  consoleOptions.mainWnd = m_hWnd;
	std::shared_ptr<TabView> tabView(new TabView(consoleOptions));

  HWND hwndTabView = tabView->m_hWnd;

	if (hwndTabView == NULL)
	{
		return false;
	}

	m_tabs.insert(TabViewMap::value_type(hwndTabView, tabView));

	CString cstrTabTitle;
	tabView->GetWindowText(cstrTabTitle);
	if( !consoleOptions.strTitle.empty() )
	{
		cstrTabTitle = consoleOptions.strTitle.c_str();
		tabView->SetTitle(tstring(cstrTabTitle));
	}

  AddTab(hwndTabView, cstrTabTitle);

	DisplayTab(hwndTabView, FALSE);
	::SetForegroundWindow(m_hWnd);

	UpdateUI();

	return true;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void MainFrame::CloseTab(CTabViewTabItem* pTabItem)
{
  if (!pTabItem) return;
  // TODO if( !g_settingsHandler->GetBehaviorSettings().closeSettings.bAllowClosingLastView )    if (m_tabs.size() <= 1) return;
  CloseTab(pTabItem->GetTabView());
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void MainFrame::CloseTab(HWND hwndTabView)
{
  TabViewMap::iterator it = m_tabs.find(hwndTabView);
  if (it == m_tabs.end()) return;

  RemoveTab(hwndTabView);
  if (m_activeTabView == it->second) m_activeTabView.reset();
  it->second->DestroyWindow();
  m_tabs.erase(it);

  UpdateUI();

  if (m_tabs.empty()) PostMessage(WM_CLOSE);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void MainFrame::SetWindowStyles(void)
{

  DWORD	dwStyle   = GetWindowLong(GWL_STYLE);
  DWORD	dwExStyle = GetWindowLong(GWL_EXSTYLE);

  DWORD	dwOldStyle   = dwStyle;
  DWORD	dwOldExStyle = dwExStyle;

  dwStyle |= WS_MAXIMIZEBOX;
  dwStyle |= WS_CAPTION;
  dwStyle |= WS_THICKFRAME;
  dwStyle |= WS_BORDER; /* WS_CAPTION = WS_BORDER | WS_DLGFRAME  */
  dwStyle |= WS_MINIMIZEBOX;
  dwExStyle |= WS_EX_APPWINDOW;

  if( m_bOnCreateDone )
  {
    if( dwExStyle != dwOldExStyle )
    {
      this->ShowWindow(SW_HIDE);
      SetWindowLong(GWL_EXSTYLE, dwExStyle);
      this->ShowWindow(SW_SHOW);
    }

    if( dwStyle != dwOldStyle )
    {
      SetWindowLong(GWL_STYLE, dwStyle);
      this->SetWindowPos(
        nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
  }
  else
  {
    SetWindowLong(GWL_STYLE, dwStyle);
    SetWindowLong(GWL_EXSTYLE, dwExStyle);
  }
}


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////


HWND MainFrame::GetDesktopWindow()
{
	// pinned to the desktop, Program Manager is the parent
	// TODO: support more shells/automatic shell detection
    return ::FindWindow(_T("Progman"), _T("Program Manager"));
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK MainFrame::MonitorEnumProc(HMONITOR /*hMonitor*/, HDC /*hdcMonitor*/, LPRECT lprcMonitor, LPARAM lpData)
{
  std::vector<CRect> * pvMonitors = reinterpret_cast<std::vector<CRect> *>(lpData);

  pvMonitors->push_back(lprcMonitor);

  return TRUE;
}


void MainFrame::AdjustWindowSize(ADJUSTSIZE as)
{
	// update window width and height
	CRect rectWindow;

	GetWindowRect(&rectWindow);
	m_dwWindowWidth	= rectWindow.Width();
	m_dwWindowHeight= rectWindow.Height();
	SetMargins();
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

void MainFrame::SetMargins(void)
{
    if (!this->m_TabCtrl.m_hWnd)
        return;

  CReBarCtrl rebar(m_hWndToolBar);
  DWORD dwStyle = this->m_TabCtrl.GetStyle();

  m_Margins.cyTopHeight = rebar.GetBarHeight();
  m_Margins.cyBottomHeight = 0;

  if (CTCS_BOTTOM == (dwStyle & CTCS_BOTTOM))
  {
      m_Margins.cyBottomHeight += m_nTabAreaHeight;
  }
  else
  {
      m_Margins.cyTopHeight += m_nTabAreaHeight;
  }
}

void MainFrame::SetActiveConsole(HWND hwndTabView, HWND hwndConsoleView)
{
  // find the tab
  auto it = m_tabs.find(hwndTabView);
  if( it != m_tabs.end() )
  {
    if( m_activeTabView != it->second )
    {
      int nCount = m_TabCtrl.GetItemCount();
      for(int i = 0; i < nCount; ++i)
      {
        if( m_TabCtrl.GetItem(i)->GetTabView() == hwndTabView )
        {
          m_TabCtrl.SetCurSel(i);
          break;
        }
      }
    }

  }
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

void MainFrame::PostMessageToConsoles(UINT Msg, WPARAM wParam, LPARAM lParam)
{
  for (TabViewMap::iterator it = m_tabs.begin(); it != m_tabs.end(); ++it)
  {
    it->second->PostMessageToConsoles(Msg, wParam, lParam);
  }
}


LRESULT MainFrame::OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
  // TODO if( g_settingsHandler->GetAppearanceSettings().stylesSettings.bCaption ) return 0;

  LPMINMAXINFO lpMMI = reinterpret_cast<LPMINMAXINFO>(lParam);
  /*
  For systems with multiple monitors, the ptMaxSize and ptMaxPosition members describe the maximized size
  and position of the window on the primary monitor, even if the window ultimately maximizes onto a
  secondary monitor. In that case, the window manager adjusts these values to compensate for differences
  between the primary monitor and the monitor that displays the window. Thus, if the user leaves ptMaxSize
  untouched, a window on a monitor larger than the primary monitor maximizes to the size of the larger monitor.
  */

  CRect rectCurrentWorkArea;
  CRect rectCurrentMonitor;

  if (Helpers::GetDesktopRect(m_hWnd, rectCurrentWorkArea) &&
    Helpers::GetMonitorRect(m_hWnd, rectCurrentMonitor) &&
    rectCurrentWorkArea != rectCurrentMonitor) // there is a taskbar ...
  {

    lpMMI->ptMaxPosition.x = rectCurrentWorkArea.left - rectCurrentMonitor.left;
    lpMMI->ptMaxPosition.y = rectCurrentWorkArea.top - rectCurrentMonitor.top;
    lpMMI->ptMaxSize.x = rectCurrentWorkArea.right - rectCurrentWorkArea.left;
    lpMMI->ptMaxSize.y = rectCurrentWorkArea.bottom - rectCurrentWorkArea.top;
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

void MainFrame::UpdateUI()
{

    UIEnable(ID_FILE_CLOSE_TAB, TRUE);

	UIEnable(ID_FILE_CLOSE_ALL_TABS_BUT_THIS, m_tabs.size() > 1);
	UIEnable(ID_FILE_CLOSE_ALL_TABS_LEFT, m_TabCtrl.GetCurSel() > 0);
	UIEnable(ID_FILE_CLOSE_ALL_TABS_RIGHT, m_TabCtrl.GetCurSel() < (m_TabCtrl.GetItemCount() - 1));
}

/////////////////////////////////////////////////////////////////////////////

void MainFrame::BrodcastUdpCommand(char* cmd)
{
  if (0 == strcmp(cmd, "clean"))
    ::PostMessage(HWND_BROADCAST, m_uUdpCommand, time(0), ID_EDIT_CLEAR_ALL);
  else if (0 == strcmp(cmd, "save"))
    ::PostMessage(HWND_BROADCAST, m_uUdpCommand, time(0), ID_EDIT_COPY_TO_FILE);
}

void MainFrame::OnUdpCommand(WPARAM wParam, LPARAM lParam)
{
  static time_t t = 0;
  static LPARAM cmd = 0;
  if (t == wParam && cmd == lParam)
    return;// we are receivig UdpCommand twice :(
  t = wParam;
  cmd = lParam;

  int nCount = m_TabCtrl.GetItemCount();
  for (int i = 0; i < nCount; ++i)
  {
    TabViewMap::iterator it = m_tabs.find(m_TabCtrl.GetItem(i)->GetTabView());
    if (it == m_tabs.end()) continue;

    if (ID_EDIT_COPY_TO_FILE == lParam)
    {
      TCHAR* szOutDir = _T("D:\\temp\\MultiPuTTyOut");
      if (!(CreateDirectory(szOutDir, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
        break;
      tm *pTime = localtime(&t);
      TCHAR file_path[128];
      _stprintf(file_path, _T("%s\\%02d.%02d.%02d.%02d"), szOutDir, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
      if (!(CreateDirectory(file_path, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
        break;

      COPYDATASTRUCT cds;
      cds.dwData = MAKEWPARAM(ID_EDIT_COPY_TO_FILE, 0);
      tstring file_name = file_path;
      file_name += _T("\\");
      file_name += it->second->GetTitle();
      file_name += _T(".txt");
#ifdef UNICODE
      const char* szFile = w2m(file_name.c_str());
#else
      const char* szFile = file_name.c_str();
#endif //UNICODE
      cds.cbData = strlen(szFile) + 1;
      cds.lpData = (void*)szFile;
      if (::IsWindow(it->second->m_hwndConsoleView) )
        SendMessage(it->second->m_hwndConsoleView, WM_COPYDATA, (WPARAM)(HWND)it->second->m_hWnd, (LPARAM)(LPVOID)&cds);
#ifdef UNICODE
      delete szFile;
#endif //UNICODE
    }
    else
    {
      it->second->SendCommandToConsoles(lParam);
    }
  }
}
