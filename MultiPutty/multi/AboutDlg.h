// aboutdlg.h : interface of the CAboutDlg class
//
// 

#pragma once

class CAboutDlg :
public CDialogImpl<CAboutDlg>
{
public:
  CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    MESSAGE_HANDLER(WM_DWMNCRENDERINGCHANGED, BringToTop)
	END_MSG_MAP() 
  //break; default:  break; } stdlog("message = %x %d %d\n", uMsg, wParam, lParam); return FALSE;  }

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT BringToTop(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  bool m_isOnTop;
};
