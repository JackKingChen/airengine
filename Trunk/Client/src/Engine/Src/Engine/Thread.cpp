#include "stdafx.h"
#include "Thread.h"
#include "EngineResource.h"
#include "_Mutex.h"
#include "DebugTools.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	DWORD WINAPI ThreadProc(LPVOID lpParameter) 
	{
		//	BT_SetTerminate(); // set_terminate() must be called from every thread
		Thread* thread = (Thread*)lpParameter;
		thread->DoRun();
		return 0;
	}
#else
	void* ThreadProc(void* lpParameter) 
	{
		//	BT_SetTerminate(); // set_terminate() must be called from every thread
		Thread* thread = (Thread*)lpParameter;
		thread->DoRun();
		return 0;
	}
#endif


IOThread::IOThread()
{
    DT_TO_DBGSTR(LT_NONE, "��̨IO�߳�����....\n");
    m_Event = new SyncEvent();
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	m_Handle = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
#else
    pthread_create(m_Handle, 0, ThreadProc, this);
#endif
    
    
}


IOThread::~IOThread()
{
    if (m_Handle)
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        ::CloseHandle(m_Handle);
#else
		pthread_exit(m_Handle);
#endif
    }
    SafeDelete(m_Event);
}

void IOThread::DoRun()
{
    while (1)
    {
        //��̨�߳����ڹ���
        if (m_Event->Lock(AIR_INFINITE))
        {
            //DT_TO_DBGSTR(LT_NONE, "��̨�߳����ڹ�����\n");
            RESOURCE_MANAGER->LoadAllResources();
        }
    }
}

void IOThread::SetEvent()
{
    m_Event->SetEvent();
}
