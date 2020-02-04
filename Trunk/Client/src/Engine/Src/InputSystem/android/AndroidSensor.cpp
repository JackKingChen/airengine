#include "stdafx.h"
#include "android/AndroidSensor.h"
#include "android/AndroidInputManager.h"
#include "android/AndroidJniHelper.h"

using namespace AIS;

//-------------------------------------------------------------------//
AndroidSensor::AndroidSensor( InputManager* creator, bool buffered )
: Sensor(creator->inputManagerName(), buffered, 0, creator)
{
	AndroidInputManager *man = static_cast<AndroidInputManager*>(mCreator);

	man->_setSensorUsed(true);
	man->_getDelegate()->mAcceleromaterObject = this;
	mUpdateInterval = 60.0f;
}

AndroidSensor::~AndroidSensor()
{
	AndroidInputManager *man = static_cast<AndroidInputManager*>(mCreator);

	man->_setSensorUsed(false);
	man->_getDelegate()->mAcceleromaterObject = NULL;
}

void AndroidSensor::_initialize()
{
	// Clear old joy state
	mState.clear();

	// Set the update interval
	setUpdateInterval(1.0f / mUpdateInterval);
}

void AndroidSensor::setUpdateInterval(float interval)
{
	mUpdateInterval = interval;
#ifdef AIS_ANDROID_PLATFORM
	AndroidJniHelper::setAcclerometerInterval( interval );
#endif
}

void AndroidSensor::setBuffered( bool buffered )
{
	mBuffered = buffered;
}

void AndroidSensor::didAccelerate(SensorParam& param)
{
	float tmp = param.mX;
	if ((param.mSO == SO_LANDSCAPE) && (param.mSR != SR_0)) 
	{
		param.mX = -param.mY;
		param.mY = tmp;
	} 
	else if ((param.mSO == SO_PORTRAIT) && (param.mSR != SR_0)) 
	{
		param.mX = param.mY;
		param.mY = -tmp;
	} 
	bool bOrientationChanged = (param.mSO != mState.mOrientation || param.mSR != mState.mRotateAngle);
	bool bGravityChanged = (param.mX != mState.mGravity.x || param.mY != mState.mGravity.y || param.mZ != mState.mGravity.z );
	mState.mGravity.Set( param.mX, param.mY, param.mZ );
	mState.mOrientation = param.mSO;
	mState.mRotateAngle = param.mSR;
	if ( mListener && mBuffered )
	{
		if ( bOrientationChanged )
		{
			mListener->orientationChanged( SensorEvent(ET_SensorOrientationChanged, this, mState ) );
		}
		if ( bGravityChanged )
		{
			mListener->gravityChanged( SensorEvent(ET_SensorGravityChanged, this, mState ) );
		}
	}
}

void AndroidSensor::capture()
{

}
