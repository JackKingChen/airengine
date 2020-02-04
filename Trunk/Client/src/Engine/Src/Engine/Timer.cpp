#include "stdafx.h"
#include "Timer.h"
#include "Util.h"
//#include "DetectMemLeak.h"

#include <time.h>

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
#   include <sys/time.h>
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
#   include <mach/mach_time.h>
#endif


double Timer::GetTime()
{
	s64 time;

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&time);

#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
	time = mach_absolute_time();

#else
	struct timeval t_current;
	gettimeofday( &t_current, NULL );
	time = ((long)t_current.tv_sec) * 1000000 + (long)t_current.tv_usec;
#endif

	static double counterFreq = 0;
	static s64 initialTickCount = 0;
	if (counterFreq == 0)
	{
		initialTickCount = time;

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		s64 freq;
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		counterFreq = 1.0 / double(freq);

#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_MAC || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
		mach_timebase_info_data_t    sTimebaseInfo;
		mach_timebase_info(&sTimebaseInfo);
		counterFreq = double(sTimebaseInfo.numer)/sTimebaseInfo.denom/1000000000;

#else
		counterFreq = 1.0 / 1000000.0;//1.0;

#endif

	}
	time = time-initialTickCount;

	double seconds = double(time) * counterFreq;
	return seconds;
}

AIR::u32 Timer::GetTick()
{
	u32 tick = u32(GetTime() * 1000.0);
	return tick;
}

TimerObject::TimerObject( void )
{
	m_FuncStart = NULL;
	m_FuncEnd = NULL;
	m_FuncTimeSegment = NULL;
	m_ParmStart = NULL;
	m_ParmEnd = NULL;
	m_ParmTimeSegment = NULL;
	m_TimeStart = 0.0;
	m_TimePermanent = 0.0;
	m_TimeSegment = 1.0;
	m_HasTimeEnd = false;
	m_DelTimerObject = false;
	m_TimeCount = 0.0;
	m_Done = false;
}

TimerObject::~TimerObject()
{
	m_FuncStart = NULL;
	m_FuncEnd = NULL;
	m_FuncTimeSegment = NULL;
	m_ParmStart = NULL;
	m_ParmEnd = NULL;
	m_ParmTimeSegment = NULL;
}

void TimerObject::SetStartFunc( void* Func, void* parm )
{
	m_FuncStart = (TimerCallBackFunc)Func;
	m_ParmStart = parm;
}

void TimerObject::SetEndFunc( void* Func, void* parm )
{
	m_FuncEnd = (TimerCallBackFunc)Func;
	m_ParmEnd = parm;
}

void TimerObject::SetTimeSegmentFunc( void* Func, void* parm )
{
	m_FuncTimeSegment = (TimerCallBackFunc)Func;
	m_ParmTimeSegment = parm;
}

void TimerObject::Set( double timeStart, double timePermanent, double timeSegment, bool hasTimeEnd)
{
	m_TimeStart = timeStart;
	m_TimePermanent = timePermanent;
	m_TimeSegment = timeSegment;
	m_HasTimeEnd = hasTimeEnd;
	m_TimeCount = m_TimeStart;
}

void* TimerObject::DoStartFunc( void )
{
    // 如果已经运行了StartFunc,就返回
	if ( m_Done == true )
	{
		return NULL;
	}
	if ( m_FuncStart )
	{
        // 执行m_FuncStart
		m_Done = true;
		return m_FuncStart( m_ParmStart );
	}
	return NULL;
}

void* TimerObject::DoEndFunc( void )
{
    // 执行m_FuncEnd
	if ( m_FuncEnd )
	{
		return m_FuncEnd( m_ParmEnd );
	}
	return NULL;
}

void* TimerObject::DoTimeSegmentFunc( void )
{
    // 执行m_FuncTimeSegment
	if ( m_FuncTimeSegment )
	{
		return m_FuncTimeSegment( m_ParmTimeSegment );
	}
	return NULL;
}

void TimerObject::Update( double curTime )
{
    // 先执行m_FuncStart
	DoStartFunc();
    // 如果当前时间减去时间累计大于等于时间片
	if ( curTime - m_TimeCount >= m_TimeSegment )
	{
        // 执行时间片函数
		DoTimeSegmentFunc();
		m_TimeCount = curTime;
	}

    // 检查是否已经到达计时器结束时间
	if ( m_HasTimeEnd )
	{
		if ( curTime - m_TimeStart >= m_TimePermanent )
		{
			DoEndFunc();
			m_DelTimerObject = true;
		}
	}
}

bool TimerObject::GetDelTimerObject( void )
{
	return m_DelTimerObject;
}








Timer::Timer()
{
	//QueryPerformanceFrequency( &m_CPUFrequency );
	//QueryPerformanceCounter( &m_StartTime );
	m_CurTime = 0.0;
}

Timer::~Timer()
{
	LIST_TIMER_OBJECT::iterator it = m_lstTimerObject.begin();
	for ( ; it != m_lstTimerObject.end(); ++it )
	{
        TimerObject* pObject = *it;
		SafeDelete( pObject );
	}
	m_lstTimerObject.clear();
}

void Timer::UpdateTimer( void )
{
	//LARGE_INTEGER t;
	//QueryPerformanceCounter(&t);
	//double dt = double(t.QuadPart - m_StartTime.QuadPart) / double( m_CPUFrequency.QuadPart );
	double dt = GetTime();
	m_TimeSinceLastFrame = dt - m_CurTime;
	m_CurTime = dt;
	UpdateAllTimerObject( m_CurTime );
}

void Timer::RegistorTimerObject( TimerObject* obj )
{
	m_lstTimerObject.push_back( obj );
}

void Timer::UpdateAllTimerObject( double curTime )
{
	LIST_TIMER_OBJECT::iterator it = m_lstTimerObject.begin(), tem_it;
	for ( ; it != m_lstTimerObject.end();  )
	{
		if ( (*it)->GetDelTimerObject() )
		{
            SafeDelete(*it);
			tem_it = it;
			++tem_it;
			m_lstTimerObject.erase( it );
			it = tem_it;
		}
		else
		{
			(*it)->Update( curTime );
			++it;
		}
	}
}

// 创建一个脉冲触发对象
TimerObject* Timer::CreateTimerObject( void )
{
    return new TimerObject;
}