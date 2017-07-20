#pragma once
#include "WorkerThread.h"
class CommandThread :
  public WorkerThread
{
public:
  CommandThread();
  void Terminate();
  void Work(LPVOID pWorkParam);
private:
  SOCKET s;
};

