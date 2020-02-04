
#pragma once

#include "AISDeviceObject.h"

namespace AIS
{

	// default sensitivity for vector3 component of joystick
	#define AIS_JOYSTICK_VECTOR3_DEFAULT 2.28f

	// POV / HAT Joystick component
	class AIR_INPUT_API Pov : public Component
	{
	public:
		Pov() : Component(AIS_POV), direction(0) {}

		static const int Centered  = 0x00000000;
		static const int North     = 0x00000001;
		static const int South     = 0x00000010;
		static const int East      = 0x00000100;
		static const int West      = 0x00001000;
		static const int NorthEast = 0x00000101;
		static const int SouthEast = 0x00000110;
		static const int NorthWest = 0x00001001;
		static const int SouthWest = 0x00001010;

		int direction;
	};

	// A sliding axis - only used in Win32 Right Now
	class AIR_INPUT_API Slider : public Component
	{
	public:
		Slider() : Component(AIS_Slider), abX(0), abY(0) {};

		int abX;
		int abY;
	};


	class AIR_INPUT_API JoyStickState
	{
	public:
		JoyStickState() { clear(); }
		void clear();

	public:
		std::vector<bool>		mButtons;
		std::vector<Axis>		mAxes;
		Pov						mPOV[4];
		Slider					mSliders[4];
		std::vector<Vector3>	mVectors;
	};

	class AIR_INPUT_API JoyStickEvent : public EventBase
	{
	public:
		JoyStickEvent( unsigned int evtType, DeviceObject* obj, const JoyStickState &st, int flag ) : EventBase(evtType, obj), state(st), mButtonAxisPovIndex(flag) {}
		virtual ~JoyStickEvent() {}

		const JoyStickState &state;
		int mButtonAxisPovIndex;
	};


	class AIR_INPUT_API JoyStickListener
	{
	public:
		virtual ~JoyStickListener() {}
		virtual bool buttonPressed( const JoyStickEvent &arg ) = 0;
		virtual bool buttonReleased( const JoyStickEvent &arg ) = 0;
		virtual bool axisMoved( const JoyStickEvent &arg ) = 0;
		virtual bool sliderMoved( const JoyStickEvent &arg ) {return true;}
		virtual bool povMoved( const JoyStickEvent &arg ) {return true;}
		virtual bool vector3Moved( const JoyStickEvent &arg ) {return true;}
	};


	class AIR_INPUT_API JoyStick : public DeviceObject
	{
	public:
		virtual ~JoyStick() {}

		int getNumberOfComponents(ComponentType cType) const;

		void setVector3Sensitivity(float degrees = AIS_JOYSTICK_VECTOR3_DEFAULT);

		float getVector3Sensitivity() const;

		virtual void setEventCallback( JoyStickListener *joyListener );

		JoyStickListener* getEventCallback() const;

		const JoyStickState& getJoyStickState() const { return mState; }


	protected:
		JoyStick(const std::string &vendor, bool buffered, int devID, InputManager* creator);

	protected:
		int					mSliders;
		int					mPOVs;
		JoyStickState		mState;
		JoyStickListener	*mListener;
		float				mVector3Sensitivity;

	public:
		static const int	MIN_AXIS = -32768;
		static const int	MAX_AXIS = 32767;
	};


}
