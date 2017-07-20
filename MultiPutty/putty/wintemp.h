/*
 * winstuff.h: Windows-specific inter-module stuff.
 */

#ifndef PUTTY_WINTEMP_H
#define PUTTY_WINTEMP_H

#pragma warning( disable : 4100 ) //4100: 'iprogress' : unreferenced formal parameter
#pragma warning( disable : 4996 ) //4996: This function or variable may be unsafe
#pragma warning( disable : 4127 ) //4127 : conditional expression is constant
#pragma warning( disable : 4244 ) //4244 : conversion from 'int' to 'char', possible loss of data
#pragma warning( disable : 4018 ) //4018: '<' : signed / unsigned mismatch
#pragma warning( disable : 4706 ) //4706: assignment within conditional expression
#pragma warning( disable : 4245 ) //4245: '=' : conversion from 'int' to 'unsigned int', signed / unsigned mismatch
#pragma warning( disable : 4389 ) //4389: '!=' : signed / unsigned mismatch
#pragma warning( disable : 4701 ) //4701: potentially uninitialized local variable
#pragma warning( disable : 4146 ) //4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning( disable : 4127 ) //4127: conditional expression is constant
#pragma warning( disable : 4703 ) //4703: potentially uninitialized local pointe
#pragma warning( disable : 4057 ) //4057: 'function' : 'char *' differs in indirection to slightly different base types from 'unsigned char [20]'
#pragma warning( disable : 4204 ) //4204: nonstandard extension used : non-constant aggregate initializer
#pragma warning( disable : 4100 ) //4100: unreferenced formal parameter
#pragma warning( disable : 4210 ) //4210: nonstandard extension used : function given file scope
#pragma warning( disable : 4700 ) //4700: uninitialized local variable used
#pragma warning( disable : 4146 ) //
#pragma warning( disable : 4146 ) //

// delete undernith
//#pragma warning( disable : 4005 ) //4005: 'SendMessage' : macro redefinition
//#define TCHAR char
//#define LPCTSTR const char*
//#define LPTSTR char*
//#define SendMessage SendMessageA
//#define CreateWindowEx CreateWindowExA
//#define GetTextExtentExPoint GetTextExtentExPointA
//#define PROGRESS_CLASS PROGRESS_CLASSA
//#define GetTextExtentPoint32 GetTextExtentPoint32A
//#define TextOut TextOutA
//#define SetDlgItemText SetDlgItemTextA
//#define OPENFILENAME OPENFILENAMEA
//#define GetDlgItemText GetDlgItemTextA
//#define	GetCurrentDirectory GetCurrentDirectoryA
//#define	GetSaveFileName GetSaveFileNameA
//#define	GetOpenFileName GetOpenFileNameA
//#define	SetCurrentDirectory SetCurrentDirectoryA
//#define	MSGBOXPARAMS MSGBOXPARAMSA
//#define	MessageBoxIndirect MessageBoxIndirectA
//#define	LOGFONT LOGFONTA
//#define	CHOOSEFONT CHOOSEFONTA
//#define	ChooseFont ChooseFontA
//#define	MessageBox MessageBoxA
//#define	CreateFont CreateFontA
//#define	SetWindowText SetWindowTextA
//#define	ShellExecute ShellExecuteA
//#define	WNDCLASS WNDCLASSA
//#define	RegisterClass RegisterClassA
//#define	CreateDialog CreateDialogA
//#define	TVINSERTSTRUCT TVINSERTSTRUCTA
//#define	CreateWindowEx CreateWindowExA
//#define	WC_TREEVIEW WC_TREEVIEWA
//#define	TVITEM TVITEMA
//#define	MAKEINTRESOURCE MAKEINTRESOURCEA
//#define	DialogBox DialogBoxA
//#define	LoadCursor LoadCursorA
//#define	LoadIcon LoadIconA
//#define	InsertMenu InsertMenuA
//#define	RegisterWindowMessage RegisterWindowMessageA
//#define	AppendMenu AppendMenuA
//#define	ExtTextOut ExtTextOutA
//#define	GetLocaleInfo GetLocaleInfoA
//#define	GetModuleFileName GetModuleFileNameA
//#define	CreateProcess CreateProcessA
//#define	STARTUPINFO STARTUPINFOA
//#define	ImmSetCompositionFont ImmSetCompositionFontA
//#define	PlaySound PlaySoundA
//#define	RegOpenKey RegOpenKeyA
//#define	RegQueryValueEx RegQueryValueExA
//#define	LoadLibrary LoadLibraryA
//#define	WinHelp WinHelpA
//#define	GetUserNameEx GetUserNameExA
//#define	GetSystemDirectory GetSystemDirectoryA
//#define	FormatMessage FormatMessageA
//#define	GetWindowsDirectory GetWindowsDirectoryA
//#define	FindFirstFile FindFirstFileA
//#define	WIN32_FIND_DATA WIN32_FIND_DATAA
//#define	FindNextFile FindNextFileA
//#define	CreateFile CreateFileA
//#define	WaitNamedPipe WaitNamedPipeA
//#define	CreateNamedPipe CreateNamedPipeA
//#define	DialogBoxParam DialogBoxParamA
//#define	GetWindowText GetWindowTextA
//#define	NOTIFYICONDATA NOTIFYICONDATAA
//#define	Shell_NotifyIcon Shell_NotifyIconA
//#define	RegEnumKey RegEnumKeyA
//#define	MENUITEMINFO MENUITEMINFOA
//#define	InsertMenuItem InsertMenuItemA
//#define	GetMenuItemInfo GetMenuItemInfoA
//#define	OpenFileMapping OpenFileMappingA
//#define	CreateWindow CreateWindowA
//#define	FindWindow FindWindowA
//#define	CreateFileMapping CreateFileMappingA
//#define	LPPRINTER_INFO_4 LPPRINTER_INFO_4A
//#define	LPPRINTER_INFO_5 LPPRINTER_INFO_5A
//#define	OpenPrinter OpenPrinterA
//#define	DOC_INFO_1 DOC_INFO_1A
//#define	GetFileAttributes GetFileAttributesA
//#define	CreateDirectory CreateDirectoryA
//#define	CreateMutex CreateMutexA
//#define	RegCreateKey RegCreateKeyA
//#define	GetTextExtentPoint32 GetTextExtentPoint32A
//#define	RegSetValueEx RegSetValueExA
//#define	RegDeleteKey RegDeleteKeyA
//#define	DeleteFile DeleteFileA
//#define	GetEnvironmentVariable GetEnvironmentVariableA
//#define	RegCreateKeyEx RegCreateKeyExA
//#define	RegDeleteValue RegDeleteValueA


/*
 strlen _tcsclen
 strspn _tcsspn

*/
#include <tchar.h>

#ifdef  UNICODE
#define tcmemchr wmemchr
#else
#define tcmemchr memchr
#endif




#endif
//incompatible types
//#undef PUTTY_WINTEMP_H
//#include "wintemp.h"
