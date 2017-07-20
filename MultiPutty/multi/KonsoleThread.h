#pragma once
#include "TabView.h"

#include "WorkerThread.h"

struct ConsoleOptions;

class KonsoleThread :
    public WorkerThread
{
public:
  void Work(LPVOID pWorkParam);
};

