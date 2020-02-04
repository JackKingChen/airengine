
#pragma once

#include "AISPrereqs.h"
#include "AISDeviceObject.h"

namespace AIS
{

    
	//! 触摸事件类型
	enum MultiTypeEventTypeID
	{
		MT_None = 0,    // 未定义
        MT_Pressed,     // 按下
        MT_Released,    // 释放
        MT_Moved,       // 移动
        MT_Cancelled    // 取消
	};

    //! 点坐标
    class Point
    {
    public:
        Point() : mX(0.0f), mY(0.0f) {}
        void Set( float x, float y ) { mX = x; mY = y; }
        void operator = (const Point& other) { mX = other.mX; mY = other.mY; }
		float DistTo( const Point& other )
		{
			float dx = mX - other.mX;
			float dy = mY - other.mY;
			return sqrtf( dx*dx + dy*dy );
		}
        
    public:
        float mX;
        float mY;
    };

    //! 触摸记录
    class TouchRecord
    {
    public:
        TouchRecord() : m_TouchObjID(0), m_touchID(-1), m_touchType(MT_None) {}
        
        inline bool touchIsType( MultiTypeEventTypeID touch ) const
		{
			return ((m_touchType & ( 1L << touch )) == 0) ? false : true;
		}
        
    public:
        int    m_TouchObjID;   // 触摸对象指针
        Point  m_point;        // 当前触摸点位置
        Point  m_lastPt;       // 上次触摸点的位置
        int    m_touchID;      // 触控点id
        int    m_touchType;    // 触控类型 (MultiTypeEventTypeID)
    };

    typedef std::vector<TouchRecord*> MultiTouchStateVec;

	//! 多点触摸事件
	class AIR_INPUT_API MultiTouchEvent : public EventBase
	{
	public:
		MultiTouchEvent( unsigned int evtType, DeviceObject *obj, const MultiTouchStateVec& states ) : EventBase(evtType, obj)
        {
            mStates.insert( mStates.begin(), states.begin(), states.end() );
        }
		virtual ~MultiTouchEvent()
        {
            mStates.clear();
        }

        // 记录多个触摸点的列表
        MultiTouchStateVec mStates;
	};
    
	//!触摸监听
	class AIR_INPUT_API MultiTouchListener
	{
	public:
		virtual ~MultiTouchListener() {}
        virtual bool touchesMoved( const MultiTouchEvent &arg ) = 0;
		virtual bool touchesPressed( const MultiTouchEvent &arg ) = 0;
		virtual bool touchesReleased( const MultiTouchEvent &arg ) = 0;
		virtual bool touchesCancelled( const MultiTouchEvent &arg ) = 0;
	};

    //! 定义最大的触摸点数
    #define MAX_TOUCHES 11


	//! 触摸屏设备
	class AIR_INPUT_API MultiTouch : public DeviceObject
	{
	public:
		virtual ~MultiTouch() {}

		virtual void setEventCallback( MultiTouchListener *touchListener ) {mListener = touchListener;}

		MultiTouchListener* getEventCallback() {return mListener;}
        
        virtual void SetViewSize( int w, int h );        
        virtual void GetViewSize( int& w, int &h );
        
        //处理手指按下
         bool handleTouchesBegan( int touchCound, int ids[], float posxs[], float posys[] );
        //处理手指移动
         bool handleTouchesMoved( int touchCound, int ids[], float posxs[], float posys[] );
        //处理手指离开
         bool handleTouchesEnded( int touchCound, int ids[], float posxs[], float posys[] );
        //处理取消触摸
         bool handleTouchesCancelled( int touchCound, int ids[], float posxs[], float posys[] );
        
         void resetTouchRecords();

		 void GetValidTouches(MultiTouchStateVec& validTouches);
        
    protected:
        //取得当前空闲的触摸记录结构体
        TouchRecord* GetUnusedTouch();
        //是否已存在该触摸记录
        TouchRecord* IsTouchExist( int touchObjID );
        
	protected:
		MultiTouch(const std::string &vendor, bool buffered, int devID, InputManager* creator);

		MultiTouchListener	*mListener;
        TouchRecord			mValidTouchList[MAX_TOUCHES];        
        int					mWidth;
        int					mHeight;
	};


}
