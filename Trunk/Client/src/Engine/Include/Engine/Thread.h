/********************************************************************
	created:	16:3:2011   19:35
	filename: 	Thread.h
	author:		liangairan
	
	purpose:	线程类的封装
*********************************************************************/

#pragma once 

#include "EngineConfig.h"
#include "MemoryAllocatorConfig.h"
class SyncEvent;

class EngineExport Thread //: public EngineAllocator(Thread)
{
	EngineAllocator(Thread)
public:
    Thread() : m_Handle(0), m_Event(0) {}
    virtual ~Thread() {}

    virtual void DoRun() = 0;
protected:

    void* m_Handle;
    SyncEvent* m_Event;
private:
};


class EngineExport IOThread : public Thread
{
public:
    IOThread();
    virtual ~IOThread();

    virtual void DoRun();

    void SetEvent();
protected:
private:
};
