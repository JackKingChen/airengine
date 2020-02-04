
#pragma once

#include "AISDeviceObject.h"

namespace AIS
{

	enum MouseButtonID
	{
		MB_Left = 0,    // 左键
        MB_Right,       // 右键
        MB_Middle,      // 中键
		MB_Button3,
        MB_Button4,
        MB_Button5,
        MB_Button6,
        MB_Button7
	};

	class AIR_INPUT_API MouseState
	{
	public:
		MouseState() : width(50), height(50), buttons(0) {};

		bool buttonDown( MouseButtonID button ) const;
		void clear();

	public:
		mutable int width, height;
		Axis X;
		Axis Y;
		Axis Z;
		//! represents all buttons - bit position indicates button down
		int buttons;
	};


	class AIR_INPUT_API MouseEvent : public EventBase
	{
	public:
		MouseEvent( unsigned int evtType, DeviceObject *obj, const MouseState &ms, int btnID ) : EventBase(evtType, obj), state(ms), buttonID(btnID) {}
		virtual ~MouseEvent() {}

		const MouseState &state;
		int buttonID;				// -1 means mouse move; >= 0 means some button press or release
	};


	class AIR_INPUT_API MouseListener
	{
	public:
		virtual ~MouseListener() {}
		virtual bool mouseMoved( const MouseEvent &arg ) = 0;
		virtual bool mousePressed( const MouseEvent &arg ) = 0;
		virtual bool mouseReleased( const MouseEvent &arg ) = 0;
		virtual bool mouseDBClicked( const MouseEvent &arg ) = 0;
	};


	class AIR_INPUT_API AISTimeMachine
	{
	public:
		// 取得当前时间(秒)
		static double		GetTime();
		// 取得当前时间(毫秒)
		static unsigned int GetTick();
		// 毫秒转秒
		static double		TickToTime( unsigned int tick );
		// 秒转毫秒
		static unsigned int TimeToTick( double time );
	};


	class AIR_INPUT_API MouseDBClickSimulater
	{
	public:
		MouseDBClickSimulater();
		~MouseDBClickSimulater();

		bool IsDBClickWhenPress( int buttonID );

	protected:
		unsigned int GetCurTime();

	public:
		unsigned int mButtonClickTimes[8];
		unsigned int mButtonDBClickFlags[8];
	};


	class AIR_INPUT_API Mouse : public DeviceObject
	{
	public:
		virtual ~Mouse() {}

		virtual void setEventCallback( MouseListener *mouseListener ) {mListener = mouseListener;}

		MouseListener* getEventCallback() const {return mListener;}

		const MouseState& getMouseState() const { return mState; }

		virtual void setMouseShow( bool bShow ) { mShowMouse = bShow; }
        
        bool isDBClickButton( int buttonID );
        
        bool isButtonDown( MouseButtonID button );

		virtual void getClientRect( int& x, int& y, int& width, int& height ) = 0;
		virtual void setMousePos( int x, int y ) = 0;

		void getMousePos( int& x, int& y, int&z, bool abs = true );

	protected:
		Mouse(const std::string &vendor, bool buffered, int devID, InputManager* creator)
			: DeviceObject(vendor, AISMouse, buffered, devID, creator), mListener(0), mShowMouse(false) {}

		MouseState				mState;
		MouseListener			*mListener;
		bool					mShowMouse;
		MouseDBClickSimulater	mDBClickSimulater;
	};


}
