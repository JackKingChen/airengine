
#pragma once

#include "AISJoystick.h"
#include "AISSensor.h"

namespace AIS
{
	typedef struct _SensorParam
	{
		float mX;
		float mY;
		float mZ;
		SensorOrientation mSO;
		SensorRotate mSR;
		double mTimeStamp;
	}
	SensorParam;

	class AndroidSensor : public Sensor
	{
	public:
		AndroidSensor( InputManager* creator, bool buffered );
		virtual ~AndroidSensor();
	
		virtual void setBuffered(bool buffered);

		void setUpdateInterval(float interval);

		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		virtual void _initialize();

		void didAccelerate(SensorParam& param);

	protected:
		float	mUpdateInterval;
		double	mTimeStamp;
	};

};
