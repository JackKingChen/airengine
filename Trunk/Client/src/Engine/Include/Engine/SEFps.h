
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
*	����	:   JACK                                                        *
*	����    :   1/28/2008                                                   *
*   QQ      :   280877280                                                   *
*   These files were parts of the work done by JACK. You are free to use    *
*   the codes in any way you like, modified, unmodified or copy'n'pasted    *
*   into your own work. However, I expect you to respect these points:      *
*   @ If you use this file and its contents unmodified, or use a major      *
*     part of this file, please credit the author and leave this note.      *
*   @ For use in anything commercial, please request my approval.           *
*   @ Share your work and ideas too as much as you can.                     *
*   ����޸ģ�����Ȼ                                                        *
*****************************************************************************
*	SEFps.h						                                            *
*	˵��	:	����ʱ����												    *
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
	int			m_iFps;          // ֡��
	//u32		m_dwSysTime;     // ϵͳ��ǰʱ��
	int			m_iCounter;      // ���ڼ���֡��
	float		m_fLastTime;     // �ϴλ�ȡ��ʱ��
	float		m_fFirstTime;    // ����ʼʱ��¼��ϵͳʱ��
	float		m_fTimeLapse;    // ����ӿ�ʼ��Ŀǰ��������ʱ��
	float		m_fTimePerFrame; // ����һ֡���õ�ʱ��
    float       m_fTimeSinceLastFrame;   //��һ֡ʱ��
    float       m_fLastRenderTime;       //��һ����Ⱦ��ʱ��
	SYSTEMTIME	m_SystemTime;

public:
	

	

	~Fps(){}

	float		GetTime(void);            // ��ȡϵͳʱ��
	bool		ProgTimeChkStart(void);   // �����ʱ����ʼ��ʱ
	float		GetTimeLapse(void);       // ��ȡ����Ŀǰ���й���ʱ��
    int			GetFps(void) { return m_iFps; };             // ����֡��
	float		GetTimePerFrame();		  // ����һ֡���õ�ʱ��
    float       GetElapseSinceLastFrame() { return m_fTimeSinceLastFrame; }  //��ȡ��ǰʱ������һ֡ʱ��֮��
    void        Update();             // ����֡����ÿһֻ֡�ܵ���һ�Σ�
	//SYSTEMTIME	GetSysTime();			  // ��ȡϵͳʱ��

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
�߷ֱ��ʼ������Ͷ�ʱ��Ӧ�� 
�����Ż�ʱ��Ҫ�õ���ȷ�ļ�ʱ�������õ���GetTickCount���������Դﵽ���뼶�ľ��ȡ������Ǻܲ����ģ���ʱ���Բ������ѭ�������İ취�����⣬����һ�����ȸ��ߵĶ�ʱ�������߷ֱ������ܼ���������high-resolution performance counter�������ṩ������API������ȡ�ü�����Ƶ�ʵ�QueryPerformanceFrequency��ȡ�ü�������ֵ��QueryPerformanceCounter��ʵ��ԭ�������ü�����е�8253,8254�ɱ��ʱ������ʱ��оƬʵ�ֵġ��ڼ�����ڲ�������������16λ��������

���������������Զ����ƻ����ʮ����(BDC)������������ÿ�����n�����壬ÿ������ʹ�����������ּ�һ������Ƶ���ǿɱ��,��QueryPerformanceFrequency���Եõ�,�����ҵĵ��ԣ�P4 2.8G�����ص���2793050000��P4 3G�򷵻�2992530000��QueryPerformance Counter���Եõ���ǰ�ļ�����ֵ������ֻҪ��ļ�������죬�����Ͼ��ȿ��Դﵽ1/2793050000����߸��ߡ�

�ȿ��߷ֱ��ʼ���������
public class Counter
{
      protected Int64 m_freq;
      protected bool m_usePerf = true;      //�߷ֱ��ʼ������Ƿ����

      public Counter()
      {
            if (QueryPerformanceFrequency(ref m_freq) == 0)
            {
                  m_freq = 1000;
                  m_usePerf = false;
            }
      }      

      // ��ȡ��ǰ��Tick
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

      // ��ȡ����ʱ���ڵĵδ���
      private Int64 GetSafeDelta(Int64 curTime, Int64 prevTime, Int64 maxValue)
      {
            if (curTime < prevTime)
                  return curTime + maxValue - prevTime;

            return curTime - prevTime;
      }
      
      // ��ȡ����Tick���ʱ�䣨���룩
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


Ӧ�ã�
Counter counter = new Counter();

flag = true;
while(flag)
{
      Int64 startTick = counter.CurrentTick();
      TickCount++;

      if(TickCount % FramesPerSecond == 0)
            label9.Text = (TickCount / FramesPerSecond).ToString();

      // ��֡������Ļ
      label3.Text = TickCount.ToString();

      Int64 deltaMs = counter.DeltaTime_ms(counter.CurrentTick(), startTick);
      Int64 targetMs = (Int64)(1000.0F * SecondsPerFrame);      // ���FramesPerSecond���ܱ�1��������ô���������ʱ���

    dt = dt.AddMilliseconds(targetMs);
      label1.Text = dt.ToString() + " " + dt.Millisecond;
      
      if (deltaMs <= targetMs)
      {
            label4.Text = counter.CurrentTick().ToString();

            // ��ʱ��֡����֮ǰ��ѭ��
            while (counter.DeltaTime_ms(counter.CurrentTick(), startTick) < targetMs)
            {
                  Thread.Sleep(0);
                  Application.DoEvents();
            }
      }
}

*/