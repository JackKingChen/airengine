
#pragma once

#include "AISPrereqs.h"

namespace AIS
{

	class AIR_INPUT_API InputManager
	{
	public:
		static unsigned int getVersionNumber();

		const std::string &getVersionName();

		static InputManager* createInputManager( std::size_t winHandle );

		static InputManager* createInputManager( ParamList &paramList );

		static void destroyInputManager(InputManager* manager);

		const std::string& inputManagerName();

		int getNumberOfDevices( AISDeviceType iType );

		DeviceList listFreeDevices();

		DeviceObject* createInputObject( AISDeviceType iType, bool bufferMode, const std::string &vendor = "");

		void destroyInputObject( DeviceObject* obj );

		void addFactoryCreator( FactoryCreator* factory );

		void removeFactoryCreator( FactoryCreator* factory );

		enum AddOnFactories
		{
			AddOn_All = 0,		//All Devices
			AddOn_LIRC = 1,		//PC Linux Infrared Remote Control
			AddOn_WiiMote = 2	//PC WiiMote Support
		};

	protected:
		// Called from createInputSystem, gives derived input class a chance to setup after it is created
		virtual void _initialize(ParamList &paramList) = 0;

		InputManager(const std::string& name);

		virtual ~InputManager();

        
    protected:
		const std::string		mVersionName;
		FactoryList				mFactories;
		FactoryCreatedObject	mFactoryObjects;
		const std::string		mInputSystemName;

	};


}
