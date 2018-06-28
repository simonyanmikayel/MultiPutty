#pragma once

#ifdef _PROFILER

#define MAX_TRCAE_LEN (1024*2)
#define MAX_FUNC_NAME_LEN (1024)

namespace profiler
{
	bool WsaInit();
	bool ConnectToServer(char* addr, char* port);
	bool Send(void* sendbuf, int cb);
	bool Recv(void* recvbuf, int cb);
};

enum PROFILER_CMD { CMD_START, CMD_NEXT_MODULE, CMD_PREP_INFO, CMD_INFO_SIZE, CMD_FLOW, CMD_TRACE, CMD_LAST };
#define LOG_ATTR_SKIP_CHILD 1
#define LOG_ATTR_SKIP_LOG 2

#pragma pack(push,1)
struct Cmd
{
	Cmd(PROFILER_CMD c) { cmd = c; }
	BYTE cmd;
};
struct CmdNextModule : Cmd
{
	CmdNextModule() : Cmd(CMD_NEXT_MODULE) {}
	DWORD64 BaseOfDll;
	char szModName[MAX_PATH];
  char szProjectName[MAX_PATH];
};
struct CmdInfoSize : Cmd
{
	CmdInfoSize() : Cmd(CMD_INFO_SIZE) {}
	DWORD infoSize;
};
struct CmdLog : Cmd
{
  CmdLog(PROFILER_CMD c) : Cmd(c) {}
  DWORD tid;
  DWORD tickCount;
};
struct CmdFlow : CmdLog
{
  CmdFlow() : CmdLog(CMD_FLOW) {}
  BYTE enter;
  DWORD funcId;
  DWORD64 callAddr;
};
struct CmdTrace : CmdLog
{
  CmdTrace() : CmdLog(CMD_TRACE) {}
  int call_line;
  int cb_fn_name;
  int cb_trace;
};
struct FUNC_ADDR
{
  DWORD64 addrStart;
  DWORD64 addrEnd;
  BYTE    attr;
};
#pragma pack(pop)

#endif //_PROFILER
