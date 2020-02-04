#include "stdafx.h"
#include "android/AndroidInputManager.h"
#include "android/AndroidMultiTouch.h"
#include "android/AndroidSensor.h"

using namespace AIS;

AndroidInputManager::AndroidInputManager()
: InputManager("Android Input Manager")
, mIsContentScalingSupported(false)
, mContentScalingFactor(1.0)
{
	mHideMouse = true;
	bSensorUsed = bMultiTouchUsed = false;
	mFactories.push_back(this);
	mDelegate = new InputDelegate;
}

AndroidInputManager::~AndroidInputManager()
{
	delete mDelegate;
	mDelegate = NULL;
}

void AndroidInputManager::_initialize( ParamList &paramList )
{
	_parseConfigSettings( paramList );
}

DeviceList AndroidInputManager::freeDeviceList()
{
	DeviceList ret;

	if( bSensorUsed == false )
		ret.insert(std::make_pair(AISSensor, mInputSystemName));

	if( bMultiTouchUsed == false )
		ret.insert(std::make_pair(AISMultiTouch, mInputSystemName));

	return ret;
}

int AndroidInputManager::totalDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISSensor: return 1;
	case AISMultiTouch: return 1;
	default: return 0;
	}
}

int AndroidInputManager::freeDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISSensor: return bSensorUsed ? 0 : 1;
	case AISMultiTouch: return bMultiTouchUsed ? 0 : 1;
	default: return 0;
	}
}

bool AndroidInputManager::vendorExist(AISDeviceType iType, const std::string & vendor)
{
	if( ( iType == AISMultiTouch || iType == AISSensor ) && vendor == mInputSystemName )
		return true;

	return false;
}

DeviceObject* AndroidInputManager::createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor /*= ""*/)
{
	DeviceObject *obj = 0;

	switch(iType)
	{
	case AISSensor:
		{
			if( bSensorUsed == false )
				obj = new AndroidSensor(this, bufferMode);
			break;
		}
	case AISMultiTouch:
		{
			if( bMultiTouchUsed == false )
				obj = new AndroidMultiTouch(this, bufferMode);
			break;
		}
	default:
		break;
	}

	if( obj == 0 )
	{
		AIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");
	}

	return obj;
}

void AndroidInputManager::destroyObject(DeviceObject* obj)
{
	delete obj;
	obj = NULL;
}
      
void AndroidInputManager::_parseConfigSettings( ParamList& paramList )
{

}

