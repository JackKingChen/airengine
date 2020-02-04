
#pragma once

#ifdef AIS_IPHONE_PLATFORM

#include "AISJoystick.h"
#include "AISSensor.h"
#import <UIKit/UIKit.h>

@class iPhoneAccelerometerDelegate;

namespace AIS
{
	class iPhoneSensor : public Sensor
    {
	public:
		iPhoneSensor(InputManager* creator, bool buffered);
		virtual ~iPhoneSensor();

		virtual void setBuffered(bool buffered);

        void setUpdateInterval(float interval)
        {
            mUpdateInterval = interval;
            [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0f / mUpdateInterval)];
        }

		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		virtual void _initialize();

        void didAccelerate(UIAcceleration *acceleration);

    protected:
        iPhoneAccelerometerDelegate *accelerometerDelegate;
        float mUpdateInterval;
        double mTimeStamp;
	};
}

#endif //! AIS_IPHONE_PLATFORM
