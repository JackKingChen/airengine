//=====================================================================================
// FileName  ：_mutex.h
// CreateDate：2007-08-16
// Author    ：Jenifer
// Des       ：thread safe 
//-------------------------------------------------------------------------------------
#ifndef __ACE_CCRITICALSECTION_H_
#define __ACE_CCRITICALSECTION_H_

#include "EngineConfig.h"
#include "AIRTypes.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#include <process.h>
	#include <windows.h>
	#include <stdio.h>
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
	#include <sys/errno.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <assert.h>
	#include <errno.h>
#endif



// 定义CRITICIAL_SECTION_OBJECT，对应不同的平台，有不同的定义。
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#if defined _MSC_VER
		typedef CRITICAL_SECTION		CRITICAL_SECTION_OBJECT;
        #define AIR_INFINITE			INFINITE
	#endif
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		typedef pthread_mutex_t			CRITICAL_SECTION_OBJECT;
		#define AIR_INFINITE			0xFFFFFFFF
#endif

class _Mutex_Body_
{
public:

	_Mutex_Body_():m_init(false)
	{
		Init();
	};

	~_Mutex_Body_()
	{
		Destroy();
	};

	void Enter()
	{
		if( !m_init )
			Init();
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		EnterCriticalSection( &m_mutex );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_mutex_lock (&m_mutex);
#endif
	};

	void Exit()
	{
		if( !m_init )
			return;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		LeaveCriticalSection( &m_mutex );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_mutex_unlock (&m_mutex);
#endif
	};

private:

	void Init()
	{
		m_init = true;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		InitializeCriticalSection( &m_mutex );

#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_mutexattr_init(&attr);
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
#else
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_NP);
#endif
		pthread_mutex_init( &m_mutex, &attr );
#endif
	}

	void Destroy()
	{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		DeleteCriticalSection( &m_mutex );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_mutex_destroy( &m_mutex );
#endif
	}

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	CRITICAL_SECTION_OBJECT m_mutex;
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
	pthread_mutex_t			m_mutex;
	pthread_mutexattr_t		attr;
#endif

	bool					m_init;
};
	
class _Mutex_Help
{
public:

    _Mutex_Help(_Mutex_Body_* pc) : m__Mutex_Body_(pc)
	{
		if(m__Mutex_Body_)
            m__Mutex_Body_->Enter();
	};

	~_Mutex_Help()
	{
		if(m__Mutex_Body_)
            m__Mutex_Body_->Exit();
	};

private:

	_Mutex_Body_ *m__Mutex_Body_;
};

class SyncEvent 
{
public:
    SyncEvent()
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        m_object = ::CreateEvent(NULL, FALSE, FALSE, NULL);
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_cond_init(&m_hCond, 0);
		pthread_mutex_init(&m_hMutex,NULL);
		m_bManualReset = false;
		m_bSignal = false;
#endif
        
    }

    ~SyncEvent()
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        ::CloseHandle(m_object);
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_mutex_destroy(&m_hMutex);
		pthread_cond_destroy(&m_hCond);
#endif
    }

    bool Lock(unsigned int timeout)
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        DWORD dwRet = ::WaitForSingleObject(m_object, timeout);
        if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
            return TRUE;
        else
            return FALSE;
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		int tempResult;
		if(timeout == 0)
		{
			tempResult = pthread_mutex_trylock(&m_hMutex);
			if(tempResult == EBUSY)
			{
				return false;
			}
		}
		else
		{
			tempResult = pthread_mutex_lock(&m_hMutex);
		}

		assert(tempResult == 0);

		int result = UnlockedWaitForEvent(timeout);

		tempResult = pthread_mutex_unlock(&m_hMutex);
		assert(tempResult == 0);

		return (result==0) ? true : false;
#endif
    }

    bool Unlock()
    {
        return true;
    }

    bool SetEvent()
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        return ::SetEvent(m_object) ? true : false;
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_mutex_lock(&m_hMutex);
		m_bSignal = true;
		if(m_bManualReset)
		{
			pthread_mutex_unlock(&m_hMutex);
			pthread_cond_broadcast(&m_hCond);
		}
		else
		{
			pthread_mutex_unlock(&m_hMutex);
			pthread_cond_signal(&m_hCond);
		}
#endif
    }

    bool PulseEvent()
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        return ::PulseEvent(m_object) ? true : false;
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC

#endif
    }

    bool ResetEvent()
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        return ::ResetEvent(m_object) ? true : false;
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_LINUX || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS || AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC
		pthread_mutex_lock(&m_hMutex);
		m_bSignal = false;
		pthread_mutex_unlock(&m_hMutex);
#endif
    }

private:
    void*				m_object;

#if AIR_TARGET_PLATFORM != AIR_PLATFORM_WIN32
	int UnlockedWaitForEvent(unsigned int ms)
	{
		int result = 0;
		if(!m_bSignal)
		{
			if(ms == 0)
			{
				return ETIMEDOUT;
			}

			timespec tm;
			if (AIR_INFINITE != ms)
			{
				timeval tv;
				gettimeofday(&tv, NULL);
				AIR::u64 nanoseconds = ((AIR::u64) tv.tv_sec) * 1000 * 1000 * 1000 + ms* 1000 * 1000 + ((AIR::u64) tv.tv_usec) * 1000;
				tm.tv_sec = nanoseconds / 1000 / 1000 / 1000;
				tm.tv_nsec = (nanoseconds - ((AIR::u64) tm.tv_sec) * 1000 * 1000 * 1000);
			}

			do
			{
				if(ms != AIR_INFINITE)
				{
					result = pthread_cond_timedwait(&m_hCond, &m_hMutex, &tm);
				}
				else
				{
					result = pthread_cond_wait(&m_hCond, &m_hMutex);
				}
			} 
			while(result == 0 && !m_bSignal);

			if(result == 0 && (!m_bManualReset))
			{
				m_bSignal = false;
			}
		}
		else if(!m_bManualReset)
		{
			result = 0;
			m_bSignal = false;
		}

		return result;
	}

	pthread_mutex_t		m_hMutex;
	pthread_cond_t		m_hCond;
	bool				m_bSignal;
	bool				m_bManualReset;
#endif

};

#endif 
