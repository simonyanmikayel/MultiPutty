#include "pch.h"
#include "profiler_impl.h"

DWORD dwTlsIndex; // address of shared memory

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#ifdef _PROFILER
	{
		if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
		{
			//Log("Faile to alloc Tls");
			return FALSE;
		}
		init_globals();
		init_dbg_info();
	}
#endif
	// No break: Initialize the index for first thread.
    case DLL_THREAD_ATTACH:
	{
		//DWORD tid = GetCurrentThreadId();
		//THREAD_INFO *pThreadInfo = new THREAD_INFO();
		//Log("Attached -> tid: %X (%p)", tid, pThreadInfo);
		//if (!TlsSetValue(dwTlsIndex, pThreadInfo))
		//	Log("Faile to set Tls");
	}
	break;
	case DLL_THREAD_DETACH:
#ifdef _PROFILER
	{
		DWORD tid = GetCurrentThreadId();
		THREAD_INFO *pThreadInfo = (THREAD_INFO *)TlsGetValue(dwTlsIndex);
		//Log("Detached <- tid: %X (%p)", tid, pThreadInfo);
		if (pThreadInfo != NULL)
			delete pThreadInfo;
	}
#endif
	break;
	case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
