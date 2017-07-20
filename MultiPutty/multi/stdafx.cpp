// stdafx.cpp : source file that includes just the standard includes
// MultiPutty.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)


//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Memory allocation tracking

#ifdef _DEBUG

void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
{
    return ::_malloc_dbg(nSize, 1, lpszFileName, nLine);
}

void __cdecl operator delete(void* pData, LPCSTR /* lpszFileName */, int /* nLine */)
{
    ::operator delete(pData);
}

#endif

