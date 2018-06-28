#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <Wincon.h>
#include "stdlog.h"
#include "profiler_impl.h"

#pragma warning( disable : 4100 4702 )
#ifndef _STDLOG

void stdlog(const char* f, int l, const char* lpFormat, ...) { return; }

#else

void stdlog(const char* f, int l, const char* lpFormat, ...)
{
	static int nn = 0;
	static DWORD64 dw0 = GetTickCount64();

	char buf[1024];
#ifdef _PROFILER
	CmdTrace *pCmdTrace = (CmdTrace*)buf;
	int c = sizeof(buf) / sizeof(buf[0]) - 100 - sizeof(CmdTrace);
	int cOut = sizeof(CmdTrace);
#else
	int c = sizeof(buf) / sizeof(buf[0]) - 100;
	int cOut = 0;
#endif
	DWORD64 dw = GetTickCount64();
	DWORD64 dwElised = dw - dw0;
	DWORD tid = GetCurrentThreadId();
	dw0 = dw;

#ifdef _PROFILER
	cOut += (pCmdTrace->cb_fn_name = _snprintf_s(buf + cOut, c - cOut, c - cOut, "%s", f));
#else
	cOut += _snprintf_s(buf + cOut, c - cOut, c - cOut, "[%05d %04x %05lld][%s %d] ", ++nn, tid, dwElised, f, l);
#endif
	va_list vl;
	va_start(vl, lpFormat);
#ifdef _PROFILER
	cOut += (pCmdTrace->cb_trace = vsprintf_s(buf + cOut, c - cOut, lpFormat, vl));
#else
	cOut += vsprintf_s(buf + cOut, c - cOut, lpFormat, vl);
#endif
	va_end(vl);

#ifdef _PROFILER
	pCmdTrace->cb_trace++;
#endif
	buf[cOut++] = '\n';
	buf[cOut] = 0;

#ifdef _PROFILER
	pCmdTrace->cmd = CMD_TRACE;
	pCmdTrace->tid = tid;
	pCmdTrace->tickCount = (DWORD)dw;
	pCmdTrace->call_line = l;
	if (!SendTrace(pCmdTrace, cOut)) //included '\n'
		OutputDebugStringA(buf + sizeof(CmdTrace));
#else
	OutputDebugStringA(buf);
#endif
}

#endif