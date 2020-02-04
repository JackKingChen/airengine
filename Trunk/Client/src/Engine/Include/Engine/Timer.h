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

	// ���ó�ʼ�ص�����
	void	SetStartFunc( void* Func, void* parm );

	// ���ý����ص�����
	void	SetEndFunc( void* Func, void* parm );

	// ����ʱ��λص�����
	void	SetTimeSegmentFunc( void* Func, void* parm );

	//************************************
	// Method:    Set
	// Returns:   void
	// Qualifier: ���ü�ʱ������Ĳ���
	// Parameter: double timeStart - ��ʼʱ��
	// Parameter: double timePermanent - ����ʱ��
	// Parameter: double timeSegment - ������ʱ��γ���
	// Parameter: bool hasTimeEnd - �Ƿ�ʱ����
	//************************************
	void	Set( double timeStart, double timePermanent, double timeSegment, bool hasTimeEnd);

	// ִ�г�ʼ�ص�����
	void*	DoStartFunc( void );

	// ִ�н����ص�����
	void*	DoEndFunc( void );

	// ִ��ʱ��λص�����
	void*	DoTimeSegmentFunc( void );

	// ��ʱ�������֡����
	void	Update( double curTime );

	// ��õ�ǰ�Ƿ�������ټ�ʱ������
	bool	GetDelTimerObject( void );


private:
	TimerCallBackFunc	m_FuncStart;		// ��ʱ���Ŀ�ʼ������
	void*				m_ParmStart;		// ��ʱ���Ŀ�ʼ�������Ĵ������
	TimerCallBackFunc	m_FuncEnd;			// ��ʱ���Ľ���������
	void*				m_ParmEnd;			// ��ʱ���Ľ����������Ĵ������
	TimerCallBackFunc	m_FuncTimeSegment;	// ��ʱ���Ķ�ʱ������
	void*				m_ParmTimeSegment;	// ��ʱ���Ķ�ʱ�������Ĵ������
	double				m_TimeStart;		// ��ʱ������ʼʱ��
	double				m_TimePermanent;	// ��ʱ���ĳ���ʱ��
	double				m_TimeSegment;		// ����ʱ����
	bool				m_HasTimeEnd;		// ��ʱ���Ƿ���ʱ������
	bool				m_DelTimerObject;	// ��ǰ�Ƿ����ɾ����ʱ���Լ�
	double				m_TimeCount;		// �ۼ�ʱ��
	bool				m_Done;				// �Ƿ��Ѵ����˿�ʼ����
};



typedef AIRList< TimerObject* > LIST_TIMER_OBJECT;



class EngineExport Timer
{

public:
	Timer();
	~Timer();

    // ����һ�����崥������
    TimerObject*	CreateTimerObject( void );

	// ���¼�ʱ��
	void			UpdateTimer( void );

	// ��õ�ǰ֡����һ֡��ʱ���
	double			GetTimeSinceLastFrame( void ) const
    {
        return m_TimeSinceLastFrame;
    }

	// ���֡��
	double			GetFPS( void ) const
    {
        return 1.0 / m_TimeSinceLastFrame;
    }

	// ��ǰʱ��
	double			GetCurTime( void ) const
    {
        return m_CurTime;
    }

	// ע��һ�����崥������
	void			RegistorTimerObject( TimerObject* obj );

	static double   GetTime();
	static AIR::u32	GetTick();

protected:
	// �������崥����������
	void			UpdateAllTimerObject( double curTime );


private:
	//LARGE_INTEGER		m_CPUFrequency;			// cpuƵ��
	//LARGE_INTEGER		m_StartTime;			// ��ʼʱ��
	double				m_CurTime;				// ��ǰʱ��
	double				m_TimeSinceLastFrame;	// ��ǰ֡����һ֡��ʱ���
	double				m_FPS;					// ֡����
	LIST_TIMER_OBJECT	m_lstTimerObject;		// ���崥����������

};


#endif //! __Timer_H__