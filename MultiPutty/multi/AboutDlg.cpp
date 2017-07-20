// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "Helpers.h"

CAboutDlg::CAboutDlg()
{
  m_isOnTop = false;
}

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CString strMsg;
  strMsg.Format(_T("\nMultiPutty %i.%i.%i.%i"), VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_BUILD2);

  CWindow staticMessage(GetDlgItem(IDC_STATIC_VERSION));
  staticMessage.SetWindowText(strMsg);

	CenterWindow(GetParent());

	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CAboutDlg::BringToTop(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  //return 0;

  if (!m_isOnTop) {
    m_isOnTop = true; 
    Helpers::sendAltTab();
    // ShowWindow(SW_SHOWNORMAL);
    //::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
  return 0;

}

