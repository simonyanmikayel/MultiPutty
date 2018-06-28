#pragma once

#include "profiler_ipc.h"

#ifdef _PROFILER

void init_globals();
extern "C" void init_dbg_info();
extern "C" bool dbg_info_initialized();
//inline void lock();
//inline void unlock();
//inline DWORD findAddr(DWORD64 addr);

extern int ready; // 0 - not initialized, 1 - initializing, 2 - initialized, 3 - failed
extern FUNC_ADDR* pFuncAddr;
extern DWORD cFuncAddr;
//extern BYTE* pFuncAttr;
//extern DWORD64 testAddr;

extern DWORD dwTlsIndex;
struct THREAD_INFO
{
	THREAD_INFO() { ZeroMemory(this, sizeof(*this)); }
	DWORD blocked;
	BYTE  attr;
};

void SendLog(void* p, int cb);
bool SendTrace(void* p, int cb);

#endif //_PROFILER

