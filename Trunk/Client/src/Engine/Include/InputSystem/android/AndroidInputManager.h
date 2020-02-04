
#pragma once

#include "AISInputManager.h"
#include "AISFactoryCreator.h"


namespace AIS
{
	class AndroidSensor;
	class AndroidMultiTouch;

	class InputDelegate
	{
	public:
		AndroidSensor*		mAcceleromaterObject;
		AndroidMultiTouch*	mTouchObject;
	};

	class AndroidInputManager : public InputManager, public FactoryCreator
	{
	public:
		AndroidInputManager();
		virtual ~AndroidInputManager();	

		void _initialize( ParamList &paramList );

		DeviceList freeDeviceList();

		int totalDevices(AISDeviceType iType);

		int freeDevices(AISDeviceType iType);

		bool vendorExist(AISDeviceType iType, const std::string & vendor);

		DeviceObject* createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor = "");

		void destroyObject(DeviceObject* obj);

		void _setMultiTouchUsed(bool used) { bMultiTouchUsed = used; }

		void _setSensorUsed(bool used) { bSensorUsed = used; }

		InputDelegate * _getDelegate() { return mDelegate; }

	protected:        
		void _parseConfigSettings( ParamList& paramList );

		InputDelegate	*mDelegate;
		bool			mHideMouse;
		bool			bMultiTouchUsed;
		bool			bSensorUsed;
		bool			mIsContentScalingSupported;
		float			mContentScalingFactor;
	};

};
