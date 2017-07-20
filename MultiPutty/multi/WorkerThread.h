#pragma once
class WorkerThread
{
public:
    WorkerThread(void);
    virtual ~WorkerThread(void);

    DWORD getTID(){ return m_dwTID; }
    bool IsWorking(){ return m_bWorking; }
    void StartWork(LPVOID pWorkParam = 0);
    void StopWork();
    virtual void Work(LPVOID pWorkParam) = 0;
    virtual void OnThreadReady();
private:
    LPVOID m_pWorkParam;
    bool m_bWorking;
    HANDLE m_hTreadEvent;
    HANDLE m_hThread;
    DWORD m_dwTID;
    static DWORD WINAPI ThreadFunk(LPVOID lpParameter);
};

