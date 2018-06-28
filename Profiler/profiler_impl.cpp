#include "pch.h"
#include "util.h"
#include "profiler_impl.h"

#ifdef _PROFILER

int ready = 0;
static CRITICAL_SECTION CriticalSection;

static char* serverIP = "127.0.0.1";
static char* serverPort = "8888";

typedef void**(*SkipTraceListType)();

static CmdNextModule* pDbgModules; //{"Reader.dll", "\\ExtraReader\\", 0}
DWORD cDbgModules = 0;

FUNC_ADDR* pFuncAddr = NULL;
BYTE* pFuncAttr = NULL;
DWORD cFuncAddr = 0;
DWORD64 testAddr = 0;

bool EnumModules()
{
	DWORD processID;
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;

	processID = GetCurrentProcessId();// GetProcessId(GetCurrentProcess());

	// Get a handle to the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (NULL == hProcess)
		return false;

	bool bRet = true;
	// Get process path
	char szAppFullName[MAX_PATH];
	if (GetModuleFileNameExA(hProcess, NULL, szAppFullName, sizeof(szAppFullName) / sizeof(char)))
	{
		char szAppPath[MAX_PATH];
		char szAppName[MAX_PATH];
		//wchar_t *szThisFile = __FILE__;
		_splitpath_s(szAppFullName, NULL, 0, szAppPath, MAX_PATH, szAppName, MAX_PATH, NULL, 0);

		// Get a list of all the modules in this process.
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for (size_t i = 0; i < (cbNeeded / sizeof(HMODULE)) && bRet; i++)
			{
				CmdNextModule cmd;
				if (GetModuleFileNameExA(hProcess, hMods[i], cmd.szModName, sizeof(cmd.szModName) / sizeof(cmd.szModName[0])))
				{
					for (DWORD j = 0; j < cDbgModules && bRet; j++)
					{
						char* szModule = strrchr(cmd.szModName, '\\');
						if (szModule)
							szModule++;
						else
							szModule = cmd.szModName;
						char* szDbgModName = strrchr(pDbgModules[j].szModName, '\\');
						if (szDbgModName)
							szDbgModName++;
						else
							szDbgModName = pDbgModules[j].szModName;
						if (0 == _stricmp(szModule, szDbgModName) && pDbgModules[j].BaseOfDll == NULL)
						{
							MODULEINFO modinfo;
							if (GetModuleInformation(hProcess, hMods[i], &modinfo, sizeof(modinfo)))
							{
								cmd.BaseOfDll = (DWORD64)modinfo.lpBaseOfDll;
								szModule = strrchr(cmd.szModName, '\\');
								szDbgModName = strrchr(pDbgModules[j].szModName, '\\');
								if (szModule && szDbgModName)
								{
									szModule++;
									*szModule = 0;
									strcat_s(cmd.szModName, pDbgModules[j].szModName);
								}
								strcpy_s(cmd.szProjectName, pDbgModules[j].szProjectName);

								//dbgModules[j].SkipTraceListPtr = (SkipTraceListType)GetProcAddress(hMods[i], "SkipTraceList");
								if (!profiler::Send(&cmd, sizeof(cmd)))
								{
									Log("Send failed on %s %p", cmd.szModName, modinfo.lpBaseOfDll);
									bRet = false;
								}
								else
								{
									pDbgModules[j].BaseOfDll = (DWORD64)modinfo.lpBaseOfDll;
									Log("%s %p", cmd.szModName, modinfo.lpBaseOfDll);
								}
							}
						}
					}
				}
			}
		}
	}

	CloseHandle(hProcess);

	return bRet;
}

bool init_addresses()
{
	Cmd cmd(CMD_PREP_INFO);
	if (!profiler::Send(&cmd, sizeof(cmd)))
		return false;
	CmdInfoSize cmdInfoSize;
	if (!profiler::Recv(&cmdInfoSize, sizeof(cmdInfoSize)))
		return false;
	cFuncAddr = cmdInfoSize.infoSize;
	pFuncAddr = new FUNC_ADDR[cFuncAddr];
	pFuncAttr = new BYTE[cFuncAddr];
	ZeroMemory(pFuncAttr, cFuncAddr);
	FUNC_ADDR* p = pFuncAddr;
	while (cmdInfoSize.infoSize)
	{
		if (!profiler::Recv(p, sizeof(FUNC_ADDR)))
			return false;
		p++;
		cmdInfoSize.infoSize--;
	}

	return true;
}

//void lock()
//{
//	EnterCriticalSection(&CriticalSection);
//}

//void unlock()
//{
//	LeaveCriticalSection(&CriticalSection);
//}

void init_globals()
{
	InitializeCriticalSectionAndSpinCount(&CriticalSection, 0x00000400);
	if (!profiler::WsaInit())
		return;
	if (!profiler::ConnectToServer(serverIP, serverPort))
		return;
	Cmd cmd(CMD_START);
	if (!profiler::Send(&cmd, sizeof(cmd)))
		return;
	CmdInfoSize cmdInfoSize;
	if (!profiler::Recv(&cmdInfoSize, sizeof(cmdInfoSize)))
		return;
	cDbgModules = cmdInfoSize.infoSize;
	pDbgModules = new CmdNextModule[cDbgModules];
	for (DWORD i = 0; i < cDbgModules; i++)
	{
		if(!profiler::Recv(&pDbgModules[i], sizeof(CmdNextModule)))
			return;
	}

	for (DWORD j = 0; j < cDbgModules; j++)
	{
		char* szDbgModName = strrchr(pDbgModules[j].szModName, '\\');
		if (szDbgModName)
			szDbgModName++;
		else
			szDbgModName = pDbgModules[j].szModName;
		char* szExt = strrchr(szDbgModName, '.');
		if (szExt && _stricmp(szExt, ".exe") == 0)
			continue;
		wchar_t wsz_moduleName[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szDbgModName, -1, wsz_moduleName, MAX_PATH);
		HMODULE hmodule = LoadPackagedLibrary(wsz_moduleName, 0);
		if (hmodule == NULL)
		{
			Log("LoadLibraryA failed on %s %p", pDbgModules[j].szModName);
			return;
		}
	}

	ready = 1;
}

extern "C" void init_dbg_info()
{
	int i = __LINE__;
	char* p = __FUNCDNAME__;

	//lock();
	if (ready == 1)
	{
		if (EnumModules())
		{
			bool bAllModulesLoaded = true;
			for (DWORD j = 0; j < cDbgModules; j++)
			{
				if (pDbgModules[j].BaseOfDll == NULL)
				{
					bAllModulesLoaded = false;
				}
			}
			if (bAllModulesLoaded)
			{
				if (!init_addresses())
					ready = 3;
				else
					ready = 2;
			}
		}
	}
	//unlock();
}

extern "C" bool dbg_info_initialized()
{
	return ready > 1;
}

void SendLog(void* p, int cb)
{
	if (!profiler::Send(p, cb))
		ready = 3;
}

bool SendTrace(void* p, int cb)
{
	if (ready != 2)
		return false;
	if (!profiler::Send(p, cb))
	{
		ready = 3;
		return false;
	}
	return true;

}
#endif