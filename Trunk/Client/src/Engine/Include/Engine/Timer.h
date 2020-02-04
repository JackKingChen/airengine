#pragma once

#ifndef __Timer_H__
#define __Timer_H__

//#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include "EngineConfig.h"
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"
#include "AIRTypes.h"

typedef void* (*TimerCallBackFunc)(void*);


class EngineExport TimerObject //: public EngineAllocator(TimerObject)
{
	EngineAllocator(TimerObject)
public:
	TimerObject( void );

	~TimerObject();

	// 设置初始回调函数
	void	SetStartFunc( void* Func, void* parm );

	// 设置结束回调函数
	void	SetEndFunc( void* Func, void* parm );

	// 设置时间段回调函数
	void	SetTimeSegmentFunc( void* Func, void* parm );

	//************************************
	// Method:    Set
	// Returns:   void
	// Qualifier: 设置计时器对象的参数
	// Parameter: double timeStart - 起始时间
	// Parameter: double timePermanent - 持续时间
	// Parameter: double timeSegment - 触发的时间段长度
	// Parameter: bool hasTimeEnd - 是否到时结束
	//************************************
	void	Set( double timeStart, double timePermanent, double timeSegment, bool hasTimeEnd);

	// 执行初始回调函数
	void*	DoStartFunc( void );

	// 执行结束回调函数
	void*	DoEndFunc( void );

	// 执行时间段回调函数
	void*	DoTimeSegmentFunc( void );

	// 计时器对象的帧更新
	void	Update( double curTime );

	// 获得当前是否可以销毁计时器对象
	bool	GetDelTimerObject( void );


private:
	TimerCallBackFunc	m_FuncStart;		// 计时器的开始处理函数
	void*				m_ParmStart;		// 计时器的开始处理函数的传入参数
	TimerCallBackFunc	m_FuncEnd;			// 计时器的结束处理函数
	void*				m_ParmEnd;			// 计时器的结束处理函数的传入参数
	TimerCallBackFunc	m_FuncTimeSegment;	// 计时器的定时处理函数
	void*				m_ParmTimeSegment;	// 计时器的定时处理函数的传入参数
	double				m_TimeStart;		// 计时器的起始时间
	double				m_TimePermanent;	// 计时器的持续时间
	double				m_TimeSegment;		// 脉冲时间间隔
	bool				m_HasTimeEnd;		// 计时器是否有时间限制
	bool				m_DelTimerObject;	// 当前是否可以删除计时器自己
	double				m_TimeCount;		// 累计时间
	bool				m_Done;				// 是否已处理了开始函数
};



typedef AIRList< TimerObject* > LIST_TIMER_OBJECT;



class EngineExport Timer
{

public:
	Timer();
	~Timer();

    // 创建一个脉冲触发对象
    TimerObject*	CreateTimerObject( void );

	// 更新计时器
	void			UpdateTimer( void );

	// 获得当前帧与上一帧的时间差
	double			GetTimeSinceLastFrame( void ) const
    {
        return m_TimeSinceLastFrame;
    }

	// 获得帧率
	double			GetFPS( void ) const
    {
        return 1.0 / m_TimeSinceLastFrame;
    }

	// 当前时间
	double			GetCurTime( void ) const
    {
        return m_CurTime;
    }

	// 注册一个脉冲触发对象
	void			RegistorTimerObject( TimerObject* obj );

	static double   GetTime();
	static AIR::u32	GetTick();

protected:
	// 更新脉冲触发对象容器
	void			UpdateAllTimerObject( double curTime );


private:
	//LARGE_INTEGER		m_CPUFrequency;			// cpu频率
	//LARGE_INTEGER		m_StartTime;			// 起始时间
	double				m_CurTime;				// 当前时间
	double				m_TimeSinceLastFrame;	// 当前帧与上一帧的时间差
	double				m_FPS;					// 帧速率
	LIST_TIMER_OBJECT	m_lstTimerObject;		// 脉冲触发对象容器

};


#endif //! __Timer_H__