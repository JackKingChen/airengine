
/****************************************************************************
*                                                                           *
*	SSSSSSSSSSSSSSSSSSSS      EEEEEEEEEEEEEEEEEEEE                          * 
*   SSSSSSSSSSSSSSSSSSSS      EEEEEEEEEEEEEEEEEEEE                          *
*   SSS              SSS      EEE                                           *
*   SSS              SSS      EEE                                           *
*   SSS                       EEE                  VV      VV  2222222222   *
*   SSSSSSSSSSSSSSSSSSSS      EEEEEEEEEEEEEEEEEE   VV      VV  22      22   *
*   SSSSSSSSSSSSSSSSSSSS      EEEEEEEEEEEEEEEEEE   VV      VV          22   *
*                    SSS      EEE                  VV      VV  2222222222   *
*   SSS              SSS      EEE                   VV    VV   2222222222   *
*   SSS              SSS      EEE                    VV  VV    22           *
*   SSSSSSSSSSSSSSSSSSSS      EEEEEEEEEEEEEEEEEEEE    VVVV     22      22   *
*   SSSSSSSSSSSSSSSSSSSS      EEEEEEEEEEEEEEEEEEEE     VV      2222222222   *
*                                                                           *
*****************************************************************************
*	作者	:   JACK                                                        *
*	日期    :   1/28/2008                                                   *
*   QQ      :   280877280                                                   *
*   These files were parts of the work done by JACK. You are free to use    *
*   the codes in any way you like, modified, unmodified or copy'n'pasted    *
*   into your own work. However, I expect you to respect these points:      *
*   @ If you use this file and its contents unmodified, or use a major      *
*     part of this file, please credit the author and leave this note.      *
*   @ For use in anything commercial, please request my approval.           *
*   @ Share your work and ideas too as much as you can.                     *
*   最后修改：梁蔼然                                                        *
*****************************************************************************
*	SEFps.h						                                            *
*	说明	:	声明时间类												    *
****************************************************************************/

//#ifndef __SE_FPS_INCLUDE__
//#define __SE_FPS_INCLUDE__
#pragma once 

//#include <windows.h>
#include "EngineConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

class EngineExport Fps
{
public:
	int			m_iFps;          // 帧速
	//u32		m_dwSysTime;     // 系统当前时间
	int			m_iCounter;      // 用于计算帧速
	float		m_fLastTime;     // 上次获取的时间
	float		m_fFirstTime;    // 程序开始时记录的系统时间
	float		m_fTimeLapse;    // 程序从开始到目前所经历的时间
	float		m_fTimePerFrame; // 绘制一帧所用的时间
    float       m_fTimeSinceLastFrame;   //上一帧时间
    float       m_fLastRenderTime;       //上一次渲染的时间
	SYSTEMTIME	m_SystemTime;

public:
	

	

	~Fps(){}

	float		GetTime(void);            // 获取系统时间
	bool		ProgTimeChkStart(void);   // 程序计时器开始计时
	float		GetTimeLapse(void);       // 获取程序目前运行过的时间
    int			GetFps(void) { return m_iFps; };             // 计算帧速
	float		GetTimePerFrame();		  // 绘制一帧所用的时间
    float       GetElapseSinceLastFrame() { return m_fTimeSinceLastFrame; }  //获取当前时间与上一帧时间之差
    void        Update();             // 更新帧数，每一帧只能调用一次！
	//SYSTEMTIME	GetSysTime();			  // 获取系统时间

    static  Fps* GetInstance()
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new Fps;
        }
        return m_pInstance;
    }

private:
    Fps(int t)
    { 
        m_iFps = t; 
        m_fTimeSinceLastFrame = 0.0f;
    }

    Fps()
    { 
        m_iFps			= 72;
        m_iCounter		= 0;
        m_fLastTime		= 0.0f;
        m_fFirstTime	= 0.0f;
        m_fTimeLapse	= 0.0f;
        m_fTimePerFrame	= 0.0f;
        m_fTimeSinceLastFrame = 0.0f;
        m_fLastRenderTime = GetTime();
        ProgTimeChkStart();
    }
	__int64 now;

    static Fps* m_pInstance;

};

#define FPS Fps::GetInstance()

#endif

//#endif

/*
高分辨率计数器和定时器应用 
代码优化时需要用到精确的计时器。常用的有GetTickCount函数，可以达到毫秒级的精度。但还是很不够的，这时可以采用提高循环次数的办法。另外，还有一个精度更高的定时——“高分辨率性能计数器”（high-resolution performance counter），它提供了两个API函数，取得计数器频率的QueryPerformanceFrequency和取得计数器数值的QueryPerformanceCounter。实现原理是利用计算机中的8253,8254可编程时间间隔定时器芯片实现的。在计算机内部有三个独立的16位计数器。

　　计数器可以以二进制或二—十进制(BDC)计数。计数器每秒产生n次脉冲，每次脉冲使计数器的数字减一，产生频率是可变的,用QueryPerformanceFrequency可以得到,比如我的电脑（P4 2.8G）返回的是2793050000，P4 3G则返回2992530000。QueryPerformance Counter可以得到当前的计数器值。所以只要你的计算机够快，理论上精度可以达到1/2793050000秒或者更高。

先看高分辨率计数器的类
public class Counter
{
      protected Int64 m_freq;
      protected bool m_usePerf = true;      //高分辨率计数器是否可用

      public Counter()
      {
            if (QueryPerformanceFrequency(ref m_freq) == 0)
            {
                  m_freq = 1000;
                  m_usePerf = false;
            }
      }      

      // 获取当前的Tick
      public Int64 CurrentTick()
      {
            if (m_usePerf)
            {
                  Int64 curTick = 0;
                  QueryPerformanceCounter(ref curTick);
                  return curTick;
            }
            
            return (Int64)Environment.TickCount;
      }

      // 获取两个时间内的滴答数
      private Int64 GetSafeDelta(Int64 curTime, Int64 prevTime, Int64 maxValue)
      {
            if (curTime < prevTime)
                  return curTime + maxValue - prevTime;

            return curTime - prevTime;
      }
      
      // 获取两个Tick间的时间（毫秒）
      public Int64 DeltaTime_ms(Int64 curTick, Int64 prevTick)
      {
            if (m_usePerf)
                  return 1000 * GetSafeDelta(curTick, prevTick, Int64.MaxValue) / m_freq;

            return GetSafeDelta(curTick, prevTick, Int32.MaxValue);
      }

      

      [System.Runtime.InteropServices.DllImport("KERNEL32.dll")]
      internal static extern int QueryPerformanceFrequency(ref Int64 lpFrequency);

      [System.Runtime.InteropServices.DllImport("KERNEL32.dll")]
      internal static extern int QueryPerformanceCounter(ref Int64 lpPerformanceCount);
}


应用：
Counter counter = new Counter();

flag = true;
while(flag)
{
      Int64 startTick = counter.CurrentTick();
      TickCount++;

      if(TickCount % FramesPerSecond == 0)
            label9.Text = (TickCount / FramesPerSecond).ToString();

      // 按帧更新屏幕
      label3.Text = TickCount.ToString();

      Int64 deltaMs = counter.DeltaTime_ms(counter.CurrentTick(), startTick);
      Int64 targetMs = (Int64)(1000.0F * SecondsPerFrame);      // 如果FramesPerSecond不能被1整除，那么这里会有走时误差

    dt = dt.AddMilliseconds(targetMs);
      label1.Text = dt.ToString() + " " + dt.Millisecond;
      
      if (deltaMs <= targetMs)
      {
            label4.Text = counter.CurrentTick().ToString();

            // 在时间帧到达之前空循环
            while (counter.DeltaTime_ms(counter.CurrentTick(), startTick) < targetMs)
            {
                  Thread.Sleep(0);
                  Application.DoEvents();
            }
      }
}

*/