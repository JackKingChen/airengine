#include "stdafx.h"
//#include "stdafx.h"

#include <math.h>
#include <cmath>
#include "SEFps.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

Fps* Fps::m_pInstance = NULL;

float Fps::GetTime()
{
	static bool init = false;
	static bool hires = false;
	static __int64 freq = 1;
	if(!init)
	{
		hires = QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
		if(!hires)
		    freq = 1000;
		init = true;
	}

    

    if(hires)
	{
		QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        QueryPerformanceCounter((LARGE_INTEGER *)&now);
	}
    else
        now = GetTickCount();

  return (float)((double)now / (double)freq);
}

bool Fps::ProgTimeChkStart()
{
	m_fFirstTime = GetTime();
	return true;
}

float Fps::GetTimeLapse()
{
	m_fTimeLapse = GetTime() - m_fFirstTime;
	return m_fTimeLapse;
}


/*
int Fps::GetFps()
{
	if( m_iFps == 0 ) 
		m_iFps = 72;

    m_fTimeSinceLastFrame = GetTime() - m_fLastTime;  //add by liangairan

	if( m_fTimeSinceLastFrame >= 1.0f )
	{
		m_iFps      = m_iCounter;
		m_iCounter  = 0;
		m_fLastTime = GetTime();
	}
	m_iCounter++;

	//if (m_iCounter > 100)
	//{
	//	m_iFps = m_iCounter;
	//}
	return m_iFps;
}
*/

void Fps::Update()
{
    if( m_iFps == 0 ) 
        m_iFps = 72;

    m_fTimeSinceLastFrame = GetTime() - m_fLastRenderTime;  //add by liangairan
    m_fLastRenderTime = GetTime();

    if( GetTime() - m_fLastTime >= 1.0f )
    {
        m_iFps      = m_iCounter;
        m_iCounter  = 0;
        m_fLastTime = GetTime();
    }
    m_iCounter++;
}

float Fps::GetTimePerFrame()
{
    if (m_iFps == 0)
    {
        return 1.0 / 72.0;
    }
	return 1.0 / (float)m_iFps;
}


#endif