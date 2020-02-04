
#pragma once

#include "AISDeviceObject.h"

namespace AIS
{

    //荧屏朝向
	enum SensorOrientation
	{
		SO_UNDEFINED = 0,   // 未知
		SO_PORTRAIT = 1,    // 荧屏直立
		SO_LANDSCAPE = 2,   // 荧屏横置
		SO_SQUARE = 3       // 荧屏平躺
	};

    //荧屏旋转角度
	enum SensorRotate
	{
		SR_0 = 0,
		SR_90 = 1,
		SR_180 = 2,
		SR_270 = 3
	};

    //感应器状态
	class AIR_INPUT_API SensorState
	{
	public:
		SensorState() { clear(); }
		void clear();

	public:
		Vector3             mGravity;       // 重力向量
		SensorOrientation   mOrientation;   // 荧屏朝向
		SensorRotate        mRotateAngle;   // 荧屏旋转角度类型
	};

    //感应器事件
	class AIR_INPUT_API SensorEvent : public EventBase
	{
	public:
		SensorEvent( unsigned int evtType, DeviceObject* device, const SensorState& st )
			: EventBase( evtType, device ), state( st )
		{}
		virtual ~SensorEvent() {}

		const SensorState &state;
	};

    //感应器监听
	class AIR_INPUT_API SensorListener
	{
	public:
		virtual ~SensorListener() {}

		virtual bool orientationChanged( const SensorEvent& arg ) = 0;
		virtual bool gravityChanged( const SensorEvent& arg ) = 0;
	};

    //感应器设备
	class AIR_INPUT_API Sensor : public DeviceObject
	{
	public:
		virtual ~Sensor() {}

		virtual void setEventCallback( SensorListener* pListener );

		SensorListener* getEventCallback() const;

		const SensorState& getAccelerometerState() const { return mState; }

	protected:
		Sensor(const std::string &vendor, bool buffered, int devID, InputManager* creator);

		SensorListener	*mListener;
		SensorState		mState;
	};


}
