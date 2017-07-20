// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
/*
#define WINVER        0x0602
#define _WIN32_WINNT  0x0602
*/
#define _WIN32_IE     0x0501
#define _RICHEDIT_VER 0x0100

//PuTTy is not UNICODE, so we are set UNICODE here and not in project properies
#define UNICODE

//#pragma warning(push)
//#pragma warning(disable: 4091 4302 4458 4838 4996)

//////////////////////////////////////////////////////////////////////////////
#include <atlbase.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <atltypes.h>

#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atlmisc.h>

#include <atltheme.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>

#include <userenv.h>
#include <Lm.h>

#include <atlgdix.h>

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "CustomTabCtrl.h"
#include "DotNetTabCtrl.h"
#include "TabbedFrame.h"
//#include "multisplit.h"

#include <HtmlHelp.h>
#include <ShellApi.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <winuser.h>
#include <Muiload.h>
#if _MSC_VER >= 1900
/*
Muiload.lib(muiload.obj) : error LNK2019: unresolved external symbol _vsnwprintf referenced in function
"long __cdecl StringVPrintfWorkerW(unsigned short *,unsigned __int64,unsigned __int64 *,unsigned short const *,char *)" (?StringVPrintfWorkerW@@YAJPEAG_KPEA_KPEBGPEAD@Z)

The definitions of all of the printf and scanf functions have been moved inline into <stdio.h>, <conio.h>,
and other CRT headers.This is a breaking change that leads to a linker error(LNK2019, unresolved external
symbol) for any programs that declared these functions locally without including the appropriate CRT
headers.If possible, you should update the code to include the CRT headers(that is, add #include <stdio.h>)
and the inline functions, but if you do not want to modify your code to include these header files,
an alternative solution is to add an additional library to your linker input, legacy_stdio_definitions.lib.
*/
#pragma comment (lib, "legacy_stdio_definitions.lib")
#endif

//#include <shobjidl.h>

#ifndef _USING_V110_SDK71_
//#include <ShellScalingAPI.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <array>
#include <regex>
#include <chrono>
#include <memory>
using namespace std;

//////////////////////////////////////////////////////////////////////////////
// Version numbers

#include "version.h"

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// WinXP defines

#define SPI_GETFONTSMOOTHINGTYPE	0x200A
#define FE_FONTSMOOTHINGCLEARTYPE	0x0002

//////////////////////////////////////////////////////////////////////////////
// User-defined messages

#define UM_CONSOLE_RESIZED		WM_USER + 0x1001
#define UM_SHOW_POPUP_MENU		WM_USER + 0x1004
#define UM_START_MOUSE_DRAG		WM_USER + 0x1005

//////////////////////////////////////////////////////////////////////////////

typedef enum
{
    ADJUSTSIZE_NONE,
    ADJUSTSIZE_WINDOW,
    ADJUSTSIZE_FONT
}
ADJUSTSIZE;

//////////////////////////////////////////////////////////////////////////////

#define SEARCH_MRU_COUNT        10

/*
#define UM_UPDATE_CONSOLE_VIEW	WM_USER + 0x1000
#define UM_TRAY_NOTIFY			WM_USER + 0x1006

#define UPDATE_CONSOLE_RESIZE           0x0001
#define UPDATE_CONSOLE_TEXT_CHANGED     0x0002
#define UPDATE_CONSOLE_TITLE_CHANGED    0x0004
#define UPDATE_CONSOLE_CSBI_CHANGED     0x0008
#define UPDATE_CONSOLE_PROGRESS_CHANGED 0x0010

#define IDC_TRAY_ICON		0x0001

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Global hotkeys

#define IDC_GLOBAL_ACTIVATE	0xB000

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Windows' console window menu commands

#define SC_CONSOLE_COPY			0xFFF0
#define SC_CONSOLE_PASTE		0xFFF1
#define SC_CONSOLE_MARK			0xFFF2
#define SC_CONSOLE_SCROLL		0xFFF3
#define SC_CONSOLE_FIND			0xFFF4
#define SC_CONSOLE_SELECT_ALL	0xFFF5

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Console limits
#define MIN_BUFFER_ROWS     0
#define MAX_BUFFER_ROWS     32766
#define MIN_BUFFER_COLUMNS  0
#define MAX_BUFFER_COLUMNS  9999
#define MIN_WINDOW_ROWS     10
#define MAX_WINDOW_ROWS     200
#define MIN_WINDOW_COLUMNS  10
#define MAX_WINDOW_COLUMNS  300

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
#define EXTERNAL_COMMANDS_COUNT 10

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// pseudo SCROLL constant
#define SB_WHEEL 99

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

#define TM_INMENUMODE     0x0001
#define TM_POSTCHARBREAKS 0x0002

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
*/

//////////////////////////////////////////////////////////////////////////////
// Memory allocation tracking

#ifdef _DEBUG
#include <crtdbg.h>
void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
#define DEBUG_NEW new(__FILE__, __LINE__)

void __cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine);
#endif

//////////////////////////////////////////////////////////////////////////////
// trace function and TRACE macro

extern "C" {
#include "StdLog.h"
}

#ifdef _DEBUG

void Trace(const TCHAR* pszFormat, ...);

#define TRACE		::Trace
#define TRACE_KEY	__noop
#define TRACE_PERF	__noop

//#define MultuPutty_CHRONOS

#else

#define TRACE		__noop
#define TRACE_KEY	__noop
#define TRACE_PERF	__noop

#endif // _DEBUG

#ifdef  UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#undef UNICODE
extern "C" {
#include "putty.h"
}
#define UNICODE

//////////////////////////////////////////////////////////////////////////////
