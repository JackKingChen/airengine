
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISPrereqs.h"
#include "mac/MacPrereqs.h"
#include "AISFactoryCreator.h"

#import <CoreFoundation/CFString.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFPlugIn.h>
#import <IOKit/hid/IOHIDLib.h>
#import <IOKit/hid/IOHIDKeys.h>
#import <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>

namespace AIS
{
	//Information needed to create Mac HID Devices
	class HidInfo
	{
	public:
		HidInfo() : type(AISUnknown), numButtons(0), numHats(0), numAxes(0), inUse(false), interface(0)
		{
		}

		//Useful tracking information
		AISDeviceType type;
		std::string vendor;
		std::string productKey;
		std::string combinedKey;

		//Retain some count information for recreating devices without having to reparse
		int numButtons;
		int numHats;
		int numAxes;
		bool inUse;

		//Used for opening a read/write/tracking interface to device
		IOHIDDeviceInterface **interface;
	};

	typedef std::vector<HidInfo*> HidInfoList;
		
	class MacHIDManager : public FactoryCreator
	{
	public:
		MacHIDManager();
		~MacHIDManager();

		void initialize();
		
		void iterateAndOpenDevices(io_iterator_t iterator);
		io_iterator_t lookUpDevices(int usage, int page);

		DeviceList freeDeviceList();

		int totalDevices(AISDeviceType iType);

		int freeDevices(AISDeviceType iType);

		bool vendorExist(AISDeviceType iType, const std::string & vendor);

		DeviceObject* createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor = "");

		void destroyObject(DeviceObject* obj);

	private:
		HidInfo*	enumerateDeviceProperties(CFMutableDictionaryRef propertyMap);
		void		parseDeviceProperties(CFDictionaryRef properties);
		void		parseDevicePropertiesGroup(CFDictionaryRef properties);

		HidInfoList mDeviceList;		
	};
}

#endif //!AIS_APPLE_PLATFORM

