#pragma once

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

char* w2m(const wchar_t *w);
wchar_t* m2w(const char *c);

class Helpers
{
	public:


		static tstring GetModuleFileName(HINSTANCE hInstance);

		static tstring GetCurrentDirectory(void);
		static tstring EscapeCommandLineArg(const tstring& str);

		static const TCHAR * GetEnvironmentVariable(const TCHAR * envb, const TCHAR * str, size_t len = SIZE_MAX);

		static tstring GetComputerName(void);

		static bool GetMonitorRect(HWND hWnd, CRect& rectMonitor);
		static bool GetDesktopRect(HWND hWnd, CRect& rectDesktop);
		static bool GetDesktopRect(const CPoint& point, CRect& rectDesktop);

		static HBITMAP CreateBitmap(HDC dc, DWORD dwWidth, DWORD dwHeight, CBitmap& bitmap);

		//static tstring LoadString(UINT uID);
		//static tstring LoadFileFilter(UINT uID);
		//static void LoadCombo(CComboBox& cb, UINT uID);

		//static HICON LoadTabIcon(bool bBigIcon, bool bUseDefaultIcon, const string& strIcon, const string& strShell);

		//static bool IsElevated(void);
		static bool CheckOSVersion(DWORD dwMinMajorVersion, DWORD dwMinMinorVersion);

		static int GetHighDefinitionResourceId(int nId);

		//static tstring ToUtf8(const tstring& text);
		//static void WriteLine(HANDLE hFile, const tstring& text);

		//static tstring GetUACPrefix(void);

    static void sendAltTab();

	private:

		static bool GetMonitorRect(HMONITOR hMonitor, bool bIgnoreTaskbar, CRect& rectDesktop);

};

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

class CriticalSection
{
	public:

		CriticalSection()
		{
			::InitializeCriticalSection(&m_cs);
		}

		~CriticalSection()
		{
			::DeleteCriticalSection(&m_cs);
		}

	public:

		_Acquires_lock_(this->m_cs) void Enter()
		{
			::EnterCriticalSection(&m_cs);			
		}
		
		_Releases_lock_(this->m_cs) void Leave()
		{
			::LeaveCriticalSection(&m_cs);			
		}

	private:

		CRITICAL_SECTION m_cs;
};

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

class CriticalSectionLock
{
	public:

		explicit CriticalSectionLock(CriticalSection& critSection)
		: m_critSection(critSection)
		{
			m_critSection.Enter();
		}

		~CriticalSectionLock()
		{
			m_critSection.Leave();
		}

	private:

		CriticalSection& m_critSection;
};

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

class Mutex
{
	public:
		Mutex(SECURITY_ATTRIBUTES* pSecAttribs, BOOL bInitialOwner,	LPCTSTR pszName)
		: m_mutex(::CreateMutex(pSecAttribs, bInitialOwner, pszName), ::CloseHandle)
		{
		}

		HANDLE get()
		{
			return static_cast<HANDLE>(m_mutex.get());
		}

	private:

		std::shared_ptr<void>	m_mutex;
};

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

class MutexLock
{
	public:

		explicit MutexLock(Mutex& mutex)
		: m_mutex(mutex)
		{
			::WaitForSingleObject(m_mutex.get(), INFINITE);
		}

		~MutexLock()
		{
			::ReleaseMutex(m_mutex.get());
		}

	private:

		Mutex&	m_mutex;
};



//////////////////////////////////////////////////////////////////////////////
