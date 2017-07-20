#include "StdAfx.h"
#include "Helpers.h"

//////////////////////////////////////////////////////////////////////////////


char* w2m(const wchar_t *w)
{
  int cw = wcslen(w);
  int l = WideCharToMultiByte(CP_ACP, 0, w, cw, 0, 0, NULL, NULL);
  char *c = new char[l + 1];
  WideCharToMultiByte(CP_ACP, 0, w, cw, c, l, NULL, NULL);
  c[l] = 0;
  return c;
}

wchar_t* m2w(const char *pc)
{
    char *c;
    if (pc == 0)
        c = "";
    else
        c = (char*)pc;

  int cc = strlen(c);
  int l = MultiByteToWideChar(CP_ACP, 0, c, cc, 0, 0);
  wchar_t *w = new wchar_t[l + 1];
  MultiByteToWideChar(CP_ACP, 0, c, cc, w, l);
  w[l] = 0;
  return w;
}

tstring Helpers::GetModuleFileName(HINSTANCE hInstance)
{
	TCHAR szModulePath[MAX_PATH] = _T("");

	::GetModuleFileName(hInstance, szModulePath, MAX_PATH);

	tstring strPath(szModulePath);

	return strPath;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

tstring Helpers::GetCurrentDirectory(void)
{
    TCHAR szCD[MAX_PATH] = _T("");

	if( ::GetCurrentDirectory(MAX_PATH, szCD) == 0 )
		return tstring();
	else
		return tstring(szCD);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

tstring Helpers::EscapeCommandLineArg(const tstring& str)
{
	tstring result(_T("\""));
	result += str;
    if (str.back() == _T('\''))
        result += _T("\"");
    result += _T("\"");

	return result;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


const TCHAR * Helpers::GetEnvironmentVariable(const TCHAR * envb, const TCHAR * str, size_t len /*= SIZE_MAX*/)
{
	const TCHAR * ptr = envb;

	if( len == SIZE_MAX ) len = _tcslen(str);

	while ((ptr[0] != _T('\x00')) && !(_tcsnicmp(ptr, str, len) == 0 && ptr[len] == _T('='))) ptr += _tcslen(ptr)+1;

	if( ptr[0] != _T('\x00') )
		return ptr + len + 1;

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

tstring Helpers::GetComputerName(void)
{
	tstring strComputerName;

	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD   dwComputerNameLen = ARRAYSIZE(szComputerName);
	if(::GetComputerName(szComputerName, &dwComputerNameLen))
		strComputerName = szComputerName;

	return strComputerName;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

bool Helpers::GetMonitorRect(HWND hWnd, CRect& rectMonitor)
{
	HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	return GetMonitorRect(hMonitor, true, rectMonitor);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

bool Helpers::GetDesktopRect(HWND hWnd, CRect& rectDesktop)
{
	HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	return GetMonitorRect(hMonitor, false, rectDesktop);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

bool Helpers::GetDesktopRect(const CPoint& point, CRect& rectDesktop)
{
	HMONITOR hMonitor = ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
	return GetMonitorRect(hMonitor, false, rectDesktop);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

bool Helpers::GetMonitorRect(HMONITOR hMonitor, bool bIgnoreTaskbar, CRect& rectDesktop)
{
  ::ZeroMemory(&rectDesktop, sizeof(CRect));

  MONITORINFO		mi;

  ::ZeroMemory(&mi, sizeof(MONITORINFO));
  mi.cbSize = sizeof(MONITORINFO);

  if( ::GetMonitorInfo(hMonitor, &mi) )
  {
    rectDesktop = bIgnoreTaskbar? mi.rcMonitor : mi.rcWork;
    return true;
  }
  else
  {
    return false;
  }
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

HBITMAP Helpers::CreateBitmap(HDC dc, DWORD dwWidth, DWORD dwHeight, CBitmap& bitmap)
{
//	HBITMAP hBmp = bitmap.CreateCompatibleBitmap(dc, dwWidth, dwHeight);
//	if (hBmp != NULL) return hBmp;

	// failed to create compatible bitmap, fall back to DIB section...
	BITMAPINFO	bmpInfo;
	void*		pBits = NULL;
	
	::ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));

//	DWORD dwBytesPerLine =   (((32 * bkImage->dwImageWidth) + 31) / 32 * 4); 
	bmpInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth		= dwWidth;
	bmpInfo.bmiHeader.biHeight		= dwHeight;
	bmpInfo.bmiHeader.biPlanes		= static_cast<WORD>(::GetDeviceCaps(dc, PLANES));
	bmpInfo.bmiHeader.biBitCount	= static_cast<WORD>(::GetDeviceCaps(dc, BITSPIXEL));
	bmpInfo.bmiHeader.biCompression	= BI_RGB;
	bmpInfo.bmiHeader.biSizeImage	= 0;//dwBytesPerLine*bkImage->dwImageHeight;


	return bitmap.CreateDIBSection(dc, &bmpInfo, DIB_RGB_COLORS, &pBits, NULL, 0);
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/*
tstring Helpers::LoadString(UINT uID)
{
	CAtlString localized;

	if(localized.LoadStringW(uID))
		return tstring(localized);
	else
        return tstring(_T("LoadString failed"));
}
*/
/*
tstring Helpers::LoadFileFilter(UINT uID)
{
	// The OPENFILENAME struct (used by the CFileDialog) expects
	// the filter string components to be delimited using '\0' chars.
	// Unfortunately, strings containing \0 can't be embed in applications string table,
	// so a pipe symbol is used instead
	tstring str = Helpers::LoadStringW(uID);

    std::replace(str.begin(), str.end(), _T('|'), _T('\0'));

	return str;
}
*/
/*
void Helpers::LoadCombo(CComboBox& cb, UINT uID)
{
	tstring combo = Helpers::LoadStringW(uID);
	std::vector<tstring> tok;
    boost::algorithm::split(tok, combo, std::bind2nd(std::equal_to<TCHAR>(), _T(';')));

	for(auto tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter)
	{
		if(!tok_iter->empty())
			cb.AddString(tok_iter->c_str());
	}
}
*/
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/*
HICON Helpers::LoadTabIcon(bool bBigIcon, bool bUseDefaultIcon, const string& strIcon, const string& strShell)
{
  if (bUseDefaultIcon)
  {
    if ( !strShell.empty() )
    {
      string strCommandLine = Helpers::ExpandEnvironmentStrings(strShell);
      int argc = 0;
      std::unique_ptr<LPWSTR[], LocalFreeHelper> argv(::CommandLineToArgvW(strCommandLine.c_str(), &argc));

      if ( argv && argc > 0 )
      {
        SHFILEINFO info;
        memset(&info, 0, sizeof(info));
        if( ::SHGetFileInfo(
          argv[0],
          0,
          &info,
          sizeof(info),
          SHGFI_ICON | (( bBigIcon )? SHGFI_LARGEICON : SHGFI_SMALLICON)) != 0 )
        {
          return info.hIcon;
        }
      }
    }
  }
  else
  {
    if (!strIcon.empty())
    {
      int index = 0;

      // check strIcon ends with ,<integer>
      bool ok = false;

      size_t pos = strIcon.find_last_of(_T(','));
      if( pos != string::npos )
      {
        bool negative = false;
        size_t i = pos + 1;
        if (i < strIcon.length() && strIcon.at(i) == _T('-'))
        {
          i ++;
          negative = true;
        }
        for(; i < strIcon.length(); ++i)
        {
            if (strIcon.at(i) >= _T('0') && strIcon.at(i) <= _T('9'))
          {
            ok = true;
            index = index * 10 + (strIcon.at(i) - _T('0'));
          }
          else
          {
            ok = false;
            break;
          }
        }
        if( negative )
          index = -index;
      }

      string strIconPath = ok ? strIcon.substr(0, pos) : strIcon;

      HICON hIcon = nullptr;

      if ( bBigIcon )
      {
        ::ExtractIconEx(
          Helpers::ExpandEnvironmentStrings(strIconPath).c_str(),
          index,
          &hIcon,
          nullptr,
          1);
      }
      else
      {
        ::ExtractIconEx(
          Helpers::ExpandEnvironmentStrings(strIconPath).c_str(),
          index,
          nullptr,
          &hIcon,
          1);
      }

      if( hIcon )
        return hIcon;
    }
  }


  if ( bBigIcon )
  {
    return static_cast<HICON>(
      ::LoadImage(
        ::GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDR_MAINFRAME),
        IMAGE_ICON,
		::GetSystemMetrics(SM_CXICON),
		::GetSystemMetrics(SM_CYICON),
        LR_DEFAULTCOLOR));
  }
  else
  {
    return static_cast<HICON>(
      ::LoadImage(
        ::GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDR_MAINFRAME),
        IMAGE_ICON,
		::GetSystemMetrics(SM_CXSMICON),
		::GetSystemMetrics(SM_CYSMICON),
        LR_DEFAULTCOLOR));
  }
}
*/
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/*
bool Helpers::IsElevated(void)
{
	std::unique_ptr<void, CloseHandleHelper> hToken(nullptr);

	{
		HANDLE _hToken = nullptr;

		if ( !::OpenProcessToken(
			::GetCurrentProcess(),
			TOKEN_QUERY,
			&_hToken) )
		{
			Win32Exception::ThrowFromLastError("OpenProcessToken");
		}

		hToken.reset(_hToken);
	}

	TOKEN_ELEVATION_TYPE tet;
	DWORD dwReturnLength = 0;

	if ( !::GetTokenInformation(
		hToken.get(),
		TokenElevationType,
		&tet,
		sizeof(TOKEN_ELEVATION_TYPE),
		&dwReturnLength ) )
	{
		Win32Exception::ThrowFromLastError("GetTokenInformation");
	}

	return tet == TokenElevationTypeFull;
}
*/
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

bool Helpers::CheckOSVersion(DWORD dwMinMajorVersion, DWORD dwMinMinorVersion)
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	BYTE op = VER_GREATER_EQUAL;

	// Initialize the OSVERSIONINFOEX structure.

	::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = dwMinMajorVersion;
	osvi.dwMinorVersion = dwMinMinorVersion;

	// Initialize the condition mask.

	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );

	// Perform the test.

	if( ::VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		dwlConditionMask) )
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

int Helpers::GetHighDefinitionResourceId(int nId)
{
	switch( ::GetSystemMetrics(SM_CYSMICON) )
	{
	case 16: return nId;
	case 20: return nId + 10;
	case 24: return nId + 20;
	default: return nId + 30;
	}
}

void Helpers::sendAltTab()
{
  INPUT Input = { 0 };
  // shift key down
  Input.type = INPUT_KEYBOARD;
  Input.ki.dwFlags = 0;
  Input.ki.wVk = VK_LMENU;
  SendInput(1, &Input, sizeof(INPUT));
  // shift key release
  Input.type = INPUT_KEYBOARD;
  Input.ki.dwFlags = KEYEVENTF_KEYUP;
  Input.ki.wVk = VK_LMENU;
  SendInput(1, &Input, sizeof(INPUT));

  // tab key down
  Input.type = INPUT_KEYBOARD;
  Input.ki.dwFlags = 0;
  Input.ki.wVk = VK_TAB;
  SendInput(1, &Input, sizeof(INPUT));
  // tab key release
  Input.type = INPUT_KEYBOARD;
  Input.ki.dwFlags = KEYEVENTF_KEYUP;
  Input.ki.wVk = VK_TAB;
  SendInput(1, &Input, sizeof(INPUT));
}
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/*
tstring Helpers::ToUtf8(const tstring& text)
{
	tstring result;
	int rc = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		text.c_str(), static_cast<int>(text.length()),
		nullptr, 0,
		nullptr, nullptr);

	if(rc > 0)
	{
		result.resize(rc);
		::WideCharToMultiByte(
			CP_UTF8,
			0,
			text.c_str(), static_cast<int>(text.length()),
			&result[0], rc,
			nullptr, nullptr);
	}

	return result;
}
*/
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/*
void Helpers::WriteLine(HANDLE hFile, const tstring& text)
{
	tstring utf8 = Helpers::ToUtf8(text);

	DWORD dwNumberOfBytesWritten;

	if(!::WriteFile(
		hFile,
		utf8.data(),
		static_cast<DWORD>(utf8.size()),
		&dwNumberOfBytesWritten, // This parameter can be NULL only when the lpOverlapped parameter is not NULL. 
		NULL))
		Win32Exception::ThrowFromLastError("WriteFile");

	if(!::WriteFile(
		hFile,
		"\r\n",
		2,
		&dwNumberOfBytesWritten, // This parameter can be NULL only when the lpOverlapped parameter is not NULL.
		NULL))
		Win32Exception::ThrowFromLastError("WriteFile");
}
*/
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/*
tstring Helpers::GetUACPrefix(void)
{
	tstring result;

	// read the prefix in default language
    std::unique_ptr<HINSTANCE__, FreeMUILibraryHelper> mui(::LoadMUILibrary(_T("cmd.exe"), MUI_LANGUAGE_ID, LOCALE_USER_DEFAULT));
	if(mui.get())
	{
		LPWSTR lpBuffer = nullptr;
		if(FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_HMODULE |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			mui.get(),
			1073751880,
			0,
			reinterpret_cast<LPWSTR>(&lpBuffer),
			0,
			nullptr))
		{
			result = lpBuffer;
			::LocalFree(lpBuffer);
		}
	}

	return result;
}
*/
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////