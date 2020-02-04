
#pragma once

#include "AISPrereqs.h"

namespace AIS
{

	class AIR_INPUT_API FactoryCreator
	{
	public:
		virtual ~FactoryCreator() {};

		virtual DeviceList freeDeviceList() = 0;

		virtual int totalDevices(AISDeviceType iType) = 0;

		virtual int freeDevices(AISDeviceType iType) = 0;

		virtual bool vendorExist(AISDeviceType iType, const std::string & vendor) = 0;

		virtual DeviceObject* createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor = "") = 0;

		virtual void destroyObject(DeviceObject* obj) = 0;
	};


}
