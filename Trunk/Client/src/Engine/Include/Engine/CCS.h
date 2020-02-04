#pragma once

#ifndef __CCS_H__
#define __CCS_H__

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
// 共享锁
class CCS 
{ 
public: 
	CCS() 
	{ 
		InitializeCriticalSection(&m_cs); 
	} 

	~CCS() 
	{ 
		DeleteCriticalSection(&m_cs); 
	} 

	void Lock() 
	{ 
		EnterCriticalSection(&m_cs); 
	} 

	void Unlock() 
	{ 
		LeaveCriticalSection(&m_cs); 
	} 

	CRITICAL_SECTION m_cs; 
};


typedef CCS MyLock;


// 自动加锁解锁器
class AutoLock
{
public:
	AutoLock(MyLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->Lock();
	}
	~AutoLock()
	{
		m_pLock->Unlock();
	}
private:
	AutoLock();
	MyLock* m_pLock;
};

#endif

#endif //! __CCS_H__


