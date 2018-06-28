#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <psapi.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment( lib, "Psapi.lib" )

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "stdlog.h"

//#ifdef _DEBUG
//	#define _STDLOG
//#endif