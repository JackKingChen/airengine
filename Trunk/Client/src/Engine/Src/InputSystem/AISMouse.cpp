#include "stdafx.h"
#include "AISMouse.h"

#include <time.h>
#include <iostream>

#ifdef AIS_WIN32_PLATFORM
#include <Windows.h>
#elif defined AIS_APPLE_PLATFORM
#include <mach/mach_time.h>
#else
#include <sys/time.h>
#endif

typedef unsigned int		Tick;
typedef double				Time;

// define uint64 type
#ifdef AIS_MSVC_COMPILER
typedef unsigned __int64    uint64;
typedef __int64             int64;
#else
typedef unsigned long long  uint64;
typedef long long           int64;
#endif


using namespace AIS;

//! Button down test
bool MouseState::buttonDown( MouseButtonID button ) const
{
	return ((buttons & ( 1L << button )) == 0) ? false : true;
}

//! Clear all the values
void MouseState::clear()
{
	X.clear();
	Y.clear();
	Z.clear();
	buttons = 0;
}

//--------------------------------------------------------------------------------------------------

/// 取得系统计时器时间(秒)
Time AISTimeMachine::GetTime()
{
	int64 time;

#ifdef AIS_WIN32_PLATFORM
	QueryPerformanceCounter((LARGE_INTEGER*)&time);
#elif defined AIS_APPLE_PLATFORM
	time = mach_absolute_time();
#else
	struct timeval t_current;
	gettimeofday( &t_current, NULL );
	time = ((long)t_current.tv_sec) * 1000000 + (long)t_current.tv_usec;
#endif

	static Time counterFreq = 0;
	static int64 initialTickCount = 0;
	if (counterFreq == 0)
	{
		initialTickCount = time;

#ifdef AIS_WIN32_PLATFORM
		int64 freq;
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		counterFreq = 1.0 / Time(freq);
#elif defined AIS_APPLE_PLATFORM
		mach_timebase_info_data_t    sTimebaseInfo;
		mach_timebase_info(&sTimebaseInfo);
		counterFreq = Time(sTimebaseInfo.numer)/sTimebaseInfo.denom/1000000000;
#else
		counterFreq = 1.0 / 1000000.0;//1.0;
#endif
	}
	time = time-initialTickCount;

	Time seconds = Time(time) * counterFreq;
//	std::cout<< "curtime is : "<<seconds<<"\n";
	return seconds;
}

/// Time转为Tick
Tick AISTimeMachine::TimeToTick(Time t)
{
	return (Tick)(t * 1000 + 0.5);
}

/// Tick转为Time
Time AISTimeMachine::TickToTime(Tick t)
{
	return (Time)(t * 0.001);
}

/// 取得系统计时器时间(毫秒)
Tick AISTimeMachine::GetTick()
{
	return TimeToTick(GetTime());
}

//--------------------------------------------------------------------------------------------------

MouseDBClickSimulater::MouseDBClickSimulater()
{
	unsigned int curTime = AISTimeMachine::GetTick();
	for ( int i = 0; i < 8; ++i )
	{
		mButtonClickTimes[i] = curTime;
		mButtonDBClickFlags[i] = false;
	}
}

MouseDBClickSimulater::~MouseDBClickSimulater()
{

}

bool MouseDBClickSimulater::IsDBClickWhenPress( int buttonID )
{
	unsigned int curTime = AISTimeMachine::GetTick();
	bool bIsDBClick = false;
	if ( mButtonDBClickFlags[buttonID] )
	{
		mButtonDBClickFlags[buttonID] = false;			
	}
	else
	{			
		unsigned int dt = curTime - mButtonClickTimes[buttonID];
		if ( dt < 300 )
		{
			mButtonDBClickFlags[buttonID] = true;
			bIsDBClick = true;
		}
	}
	mButtonClickTimes[buttonID] = curTime;
	return bIsDBClick;
}

//--------------------------------------------------------------------------------------------------

bool Mouse::isDBClickButton( int buttonID )
{
	return mDBClickSimulater.IsDBClickWhenPress(buttonID);
}

bool Mouse::isButtonDown( MouseButtonID button )
{
	return mState.buttonDown(button);
}

void Mouse::getMousePos( int& x, int& y, int&z, bool abs )
{
	x = abs ? mState.X.m_AbsVal : mState.X.m_RelVal;
	y = abs ? mState.Y.m_AbsVal : mState.Y.m_RelVal;
	z = abs ? mState.Z.m_AbsVal : mState.Z.m_RelVal;
}