#pragma once

#include "resource.h"
#include "CommandThread.h"

// TEMPORARY const
#define PROG_NAME _T("MultiPutty")

//////////////////////////////////////////////////////////////////////////////

#define ID_NEW_TAB_FIRST		1000

// Timer that will force a call to ResizeWindow (called from WM_EXITSIZEMOVE handler
// when the MultuPutty window is resized using a mouse)
// External utilities that might resize MultuPutty window usually don't send WM_EXITSIZEMOVE
// message after resizing a window.
#define	TIMER_SIZING			42
#define	TIMER_SIZING_INTERVAL	100

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class TabView;

typedef map<HWND, std::shared_ptr<TabView> >	TabViewMap;

//////////////////////////////////////////////////////////////////////////////

#define DEFAULT_CONSOLE_COMMAND _T("bash")

struct ConsoleOptions
{
    ConsoleOptions()
        : strTitle()
        , strInitialCmd()
        , strPuttyConfig()
        , pTabView()
        , mainWnd()
    {
    }
    TabView* pTabView;
    HWND mainWnd;
    tstring strTitle;
    tstring strInitialCmd;
    tstring strPuttyConfig;
};

struct CommandLineOptions
{
    CommandLineOptions()
        : strWindowTitle()
        , startupTabs()
        , startupTabTitles()
        , startupDirs()
        , startupCmds()
        , basePriorities()
        , nMultiStartSleep(0)
        , strWorkingDir()
        , strEnvironment()
    {
    }

    tstring strWindowTitle;
    std::vector<tstring> startupTabs;
    std::vector<tstring> startupTabTitles;
    std::vector<tstring> startupDirs;
    std::vector<tstring> startupCmds;
    std::vector<DWORD> basePriorities;
    int nMultiStartSleep;
    tstring strWorkingDir;
    tstring strEnvironment;
};

//////////////////////////////////////////////////////////////////////////////

class MainFrame
    : public CTabbedFrameImpl<MainFrame>
    , public CUpdateUI<MainFrame>
    , public CMessageFilter
    , public CIdleHandler
{
public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    CCommandBarCtrl m_CmdBar;

    MainFrame
        (
        LPCTSTR lpstrCmdLine
        );
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnIdle();

    BEGIN_UPDATE_UI_MAP(MainFrame)
        UPDATE_ELEMENT(ID_FILE_CLOSE_TAB, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_FILE_CLOSE_ALL_TABS_BUT_THIS, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_FILE_CLOSE_ALL_TABS_LEFT, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_FILE_CLOSE_ALL_TABS_RIGHT, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_COPY, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_COPY_WITH_TIMESTAMP, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_SELECT_ALL, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_COPY_ALL, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_COPY_ALL_WITH_TIMESTAMP, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_CLEAR_SCROLLBACK, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_RESET_TERMINAL, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_CLEAR_ALL, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_EDIT_PASTE, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_PASTE_SELECTION, UPDUI_MENUPOPUP)
        UPDATE_ELEMENT(ID_SEARCH_MATCH_CASE, UPDUI_TOOLBAR)
        UPDATE_ELEMENT(ID_SEARCH_MATCH_WHOLE_WORD, UPDUI_TOOLBAR)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(MainFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_SIZING, OnSizing)
        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)

#ifndef _USING_V110_SDK71_


#endif

        MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseButtonUp)
        MESSAGE_HANDLER(WM_RBUTTONUP, OnMouseButtonUp)
        MESSAGE_HANDLER(WM_MBUTTONUP, OnMouseButtonUp)
        MESSAGE_HANDLER(WM_XBUTTONUP, OnMouseButtonUp)
        MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(UM_CONSOLE_RESIZED, OnConsoleResized)
        MESSAGE_HANDLER(UM_START_MOUSE_DRAG, OnStartMouseDrag)

        NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnTabChanged)
        NOTIFY_CODE_HANDLER(CTCN_CLOSE, OnTabClose)
        NOTIFY_CODE_HANDLER(CTCN_MCLICK, OnTabMiddleClick);
        NOTIFY_CODE_HANDLER(NM_RCLICK, OnTabRightClick);

        NOTIFY_CODE_HANDLER(RBN_HEIGHTCHANGE, OnRebarHeightChanged)

            CHAIN_MSG_MAP(CUpdateUI<MainFrame>)

            COMMAND_ID_HANDLER(ID_SEARCH, OnSearchStart)
            COMMAND_ID_HANDLER(ID_SEARCH_PREV, OnSearch)
            COMMAND_ID_HANDLER(ID_SEARCH_NEXT, OnSearch)
            COMMAND_ID_HANDLER(ID_SEARCH_FIRST, OnSearch)
            COMMAND_ID_HANDLER(ID_SEARCH_LAST, OnSearch)
            COMMAND_ID_HANDLER(ID_SEARCH_CLEAR, OnSearch)
            COMMAND_ID_HANDLER(ID_SEARCH_REFRESH, OnSearch)
            COMMAND_ID_HANDLER(ID_FILE_NEW_TAB, OnFileNewTab)
            COMMAND_RANGE_HANDLER(ID_SWITCH_TAB_1, ID_SWITCH_TAB_1 + 9, OnSwitchTab)
            COMMAND_ID_HANDLER(ID_NEXT_TAB, OnNextTab)
            COMMAND_ID_HANDLER(ID_PREV_TAB, OnPrevTab)
            COMMAND_ID_HANDLER(ID_MOVE_TAB_LEFT, OnMoveTab)
            COMMAND_ID_HANDLER(ID_MOVE_TAB_RIGHT, OnMoveTab)
            COMMAND_ID_HANDLER(ID_SEARCH_MATCH_CASE, OnSearchSettings)
            COMMAND_ID_HANDLER(ID_SEARCH_MATCH_WHOLE_WORD, OnSearchSettings)

            COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
            COMMAND_ID_HANDLER(ID_FILE_CLOSE_TAB, OnFileCloseTab)
            COMMAND_ID_HANDLER(ID_FILE_CLOSE_ALL_TABS_BUT_THIS, OnFileCloseTab)
            COMMAND_ID_HANDLER(ID_FILE_CLOSE_ALL_TABS_LEFT, OnFileCloseTab)
            COMMAND_ID_HANDLER(ID_FILE_CLOSE_ALL_TABS_RIGHT, OnFileCloseTab)
            COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
            COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_COPY, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_COPY_WITH_TIMESTAMP, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_COPY_ALL, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_COPY_ALL_WITH_TIMESTAMP, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_CLEAR_SCROLLBACK, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_RESET_TERMINAL, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_CLEAR_ALL, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_SCROLL_UP, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_SCROLL_DOWN, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_SCROLL_PAGE_UP, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_SCROLL_PAGE_DOWN, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_SCROLL_TOP, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_SCROLL_BOTTOM, OnPuttyCommand)
            COMMAND_ID_HANDLER(ID_EDIT_SETTINGS, OnPuttyCommand)

            COMMAND_ID_HANDLER(ID_EDIT_RENAME_TAB, OnEditRenameTab)
            COMMAND_ID_HANDLER(ID_EDIT_SETTINGS, OnEditSettings)

            CHAIN_MSG_MAP(CTabbedFrameImpl<MainFrame>)
            REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    //		Handler prototypes (uncomment arguments if needed):
    //		LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //		LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //		LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSizing(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnExitSizeMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);


    LRESULT OnConsoleResized(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /* bHandled */);
    LRESULT OnStartMouseDrag(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnTabChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnTabClose(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /* bHandled */);
    LRESULT OnTabMiddleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /* bHandled */);
    LRESULT OnTabRightClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /* bHandled */);
    LRESULT OnRebarHeightChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSearchStart(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSearchSettings(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFileNewTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSwitchTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnFileCloseTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnNextTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPrevTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnMoveTab(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnPuttyCommand(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnEditRenameTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnEditSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnZoom(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSendCtrlEvent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    void OnUdpCommand(WPARAM wParam, LPARAM lParam);
    void OnSaveConsoles();



public:

    void AdjustWindowRect(CRect& rect);
    void AdjustWindowSize(ADJUSTSIZE as);
    void CloseTab(HWND hwndTabView);
    void SetActiveConsole(HWND hwndTabView, HWND hwndConsoleView);
    void PostMessageToConsoles(UINT Msg, WPARAM wParam, LPARAM lParam);
    void SetProgress(unsigned long long ullProgressCompleted, unsigned long long ullProgressTotal);
    void UpdateSearchResults();
    void ShowPopupMenu();
    void BrodcastUdpCommand(char* cmd);

private:

    bool CreateNewTab(ConsoleOptions& consoleOptions);
    void CloseTab(CTabViewTabItem* pTabItem);
    void SetActiveTab(std::shared_ptr<TabView> tabView, bool bActive);

    void SetWindowStyles(void);
    HWND GetDesktopWindow();

    void UpdateUI();

    void ResizeWindow();
    void SetMargins();
    void UpdateSerchList(WCHAR* szText);

    static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/, LPARAM lpData);

public:
    LRESULT CreateInitialTabs
        (
        const CommandLineOptions& commandLineOptions
        );

    bool					m_bOnCreateDone;

    std::shared_ptr<TabView>	m_activeTabView;
    CEdit               m_searchedit;
private:
    CommandLineOptions m_commandLineOptions;
    CommandThread* m_pCommandThread;


    DWORD m_dwNextTabNum;
    bool m_bMenuVisible;
    bool m_bMenuChecked;

    TabViewMap	m_tabs;

    CMenu			m_tabsMenu;
    CMenu			m_tabsRPopupMenu;

    CIcon			m_icon;
    CIcon			m_smallIcon;

    tstring m_strWindowTitle;

    DWORD			m_dwWindowWidth;
    DWORD			m_dwWindowHeight;
    DWORD			m_dwResizeWindowEdge;

    bool			m_bShowingHidingWindow;
    bool			m_bRestoringWindow;
    CRect			m_rectWndNotFS;

    UINT			m_uUdpCommand;
    CMenuHandle m_contextMenu;
    CToolBarCtrl        m_searchbar;
    CComboBox           m_searchbox;
    CComboBoxEx         m_cb;
    CEdit             m_searchResult;

    MARGINS m_Margins;

    HWND    m_hwndPreviousForeground;

    CComPtr<ITaskbarList3> m_pTaskbarList;
};

extern MainFrame*                  pWndMain;

//////////////////////////////////////////////////////////////////////////////
