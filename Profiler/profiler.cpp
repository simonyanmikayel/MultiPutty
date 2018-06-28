#include "pch.h"
#include "profiler.h"

#include "profiler_impl.h"
#include <intrin.h>  

extern "C" void  dummy_func(void)
{
}

#ifdef _PROFILER

__forceinline DWORD findAddr(DWORD64 addr)
{
	DWORD l = 0, u = cFuncAddr, idx, funcId = INFINITE;
	while (l < u)
	{
		idx = (l + u) / 2;
		if (addr < pFuncAddr[idx].addrStart) //comparison < 0
			u = idx;
		else if (addr > pFuncAddr[idx].addrEnd) //comparison > 0
			l = idx + 1;
		else {
			funcId = idx; //found 
			break;
		}
	}
	return funcId;
}

#define _CHECK_ATTR
void on_enter(void* paddr) {
//Log("on_enter %p", paddr);
  if (ready != 2)
    return;

  DWORD tid = GetCurrentThreadId();
  THREAD_INFO *pThreadInfo = (THREAD_INFO *)TlsGetValue(dwTlsIndex);
  if (pThreadInfo == NULL)
  {
	  pThreadInfo = new THREAD_INFO();
	  //Log("Attached -> tid: %d (%p)", tid, pThreadInfo);
	  if (!TlsSetValue(dwTlsIndex, pThreadInfo))
	  {
		  Log("Faile to set Tls");
	  }
	  else if(pThreadInfo != (THREAD_INFO *)TlsGetValue(dwTlsIndex))
	  {
		  Log("Faile 2 to set Tls");
	  }
  }
#ifdef _CHECK_ATTR
  if (pThreadInfo && pThreadInfo->blocked)
  {
	  //Log("blocked %d", pThreadInfo->blocked);
	  pThreadInfo->blocked++;
	  return;
  }
#endif
  CmdFlow cmd;
  cmd.funcId = findAddr((DWORD64)paddr);

  if (cmd.funcId == INFINITE)
    return;

  if (pThreadInfo && pFuncAddr[cmd.funcId].attr)
  {
	  //Log("Locking func %p(id %d) (attr=%d)", paddr, cmd.funcId, pFuncAddr[cmd.funcId].attr);
	  pThreadInfo->blocked = 1;
	  pThreadInfo->attr = pFuncAddr[cmd.funcId].attr;
#ifdef _CHECK_ATTR
	  if (pThreadInfo->attr & LOG_ATTR_SKIP_LOG)
		  return;
#endif
  }

  cmd.enter = 1;
  cmd.callAddr = 0;
  cmd.tid = tid;
  cmd.tickCount = (DWORD)GetTickCount64();
  //void* call_addr = 0;
  CaptureStackBackTrace(3, 1, (void**)(&cmd.callAddr), 0);
  SendLog(&cmd, sizeof(cmd));
} 

void on_exit(void* paddr) {
  if (ready != 2)
    return;

  DWORD tid = GetCurrentThreadId();
  THREAD_INFO *pThreadInfo = (THREAD_INFO *)TlsGetValue(dwTlsIndex);
  if (pThreadInfo == NULL)
  {
	  Log("Faile to get Tls");
  }

  if (pThreadInfo && pThreadInfo->blocked)
  {
	  //Log("blocked %d", pThreadInfo->blocked);
	  pThreadInfo->blocked--;
	  if (pThreadInfo->blocked == 0)
	  {
		  //Log("Unlocked func %p (attr=%d)", paddr, pThreadInfo->attr);
#ifdef _CHECK_ATTR
		  if (pThreadInfo->attr & LOG_ATTR_SKIP_LOG)
			  return;
#endif
	  }
	  else
	  {
		  return;
	  }
  }

  CmdFlow cmd;
  cmd.funcId = findAddr((DWORD64)paddr);

  if (cmd.funcId == INFINITE)
    return;

  cmd.enter = 0;
  cmd.callAddr = 0;
  cmd.tid = tid;
  cmd.tickCount = (DWORD)GetTickCount64();
  SendLog(&cmd, sizeof(cmd));
}

#ifdef _WIN64
//extern "C" void _penter();
//extern "C" void _pexit();
#else
extern "C" void __declspec(naked) _cdecl _penter(void) {
	_asm
	{
		//Prolog instructions
		pushad
		//calculate the pointer to the return address by adding 4*8 bytes 
		//(8 register values are pushed onto stack which must be removed)
		mov  eax, esp
		add  eax, 32
		// retrieve return address from stack
		mov  eax, dword ptr[eax]
		// subtract 5 bytes as instruction for call _penter
		// is 5 bytes long on 32-bit machines, e.g. E8 <00 00 00 00>
		sub  eax, 5
		// provide return address to recordFunctionCall
		push eax
		call on_enter
		pop eax

		//Epilog instructions
		popad
		ret
	}
}
extern "C" void __declspec(naked) _cdecl _pexit(void) {
	_asm
	{
		//Prolog instructions
		pushad
		//calculate the pointer to the return address by adding 4*8 bytes 
		//(8 register values are pushed onto stack which must be removed)
		mov  eax, esp
		add  eax, 32
		// retrieve return address from stack
		mov  eax, dword ptr[eax]
		// subtract 5 bytes as instruction for call _penter
		// is 5 bytes long on 32-bit machines, e.g. E8 <00 00 00 00>
		sub  eax, 5
		// provide return address to recordFunctionCall
		push eax
		call on_exit
		pop eax

		//Epilog instructions
		popad
		ret
	}
}
#endif

#endif //_PROFILER