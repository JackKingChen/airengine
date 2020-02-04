#include "stdafx.h"
#include "AISDeviceObject.h"
#include "AISInputManager.h"
#include "AISFactoryCreator.h"
#include <sstream>
#include <algorithm>

//Bring in correct Header / InputManager for current build platform
#if defined AIS_WIN32_PLATFORM
#  include "win32/Win32InputManager.h"
#elif defined AIS_LINUX_PLATFORM
#  include "linux/LinuxInputManager.h"
#elif defined AIS_APPLE_PLATFORM
#  ifdef __OBJC__
#    include "mac/CocoaInputManager.h"
#  endif
#  include "mac/MacInputManager.h"
#elif defined AIS_IPHONE_PLATFORM
#  include "ios/iPhoneInputManager.h"
#elif defined AIS_ANDROID_PLATFORM
#  include "android/AndroidInputManager.h"
#endif


using namespace AIS;

//----------------------------------------------------------------------------//
InputManager::InputManager(const std::string& name)
	: mVersionName(AIS_VERSION_NAME)
	, mInputSystemName(name)
{
    mFactories.clear();
    mFactoryObjects.clear();
}

//----------------------------------------------------------------------------//
InputManager::~InputManager()
{
}

//----------------------------------------------------------------------------//
unsigned int InputManager::getVersionNumber()
{
	return AIS_VERSION;
}

//----------------------------------------------------------------------------//
const std::string &InputManager::getVersionName()
{
	return mVersionName;
}

//----------------------------------------------------------------------------//
InputManager* InputManager::createInputManager( std::size_t windowhandle )
{
	ParamList pl;
	std::ostringstream wnd;
	wnd << windowhandle;
	pl.insert(std::make_pair( std::string("WINDOW"), wnd.str() ));

	return createInputManager( pl );
}

//----------------------------------------------------------------------------//
InputManager* InputManager::createInputManager( ParamList &paramList )
{
	InputManager* im = 0;

#if defined AIS_WIN32_PLATFORM
	im = new Win32InputManager();
#elif defined AIS_LINUX_PLATFORM
	im = new LinuxInputManager();
#elif defined AIS_APPLE_PLATFORM
	ParamList::iterator i = paramList.find("WINDOW");
	if(i != paramList.end())
    {
#ifndef __LP64__
#pragma message("at X86 architeture create MacInputManager")
        if(!im)
            im = new MacInputManager();
#endif
#ifdef __OBJC__
#pragma message("at X64 architeture create CocoaInputManager")
        if(!im)
        {
            id obj = (id)strtoul(i->second.c_str(), 0, 10);
            if(obj && [obj isKindOfClass:[NSWindow class]])
            {
                im = new CocoaInputManager();
            }
        }
#endif
    }
#elif defined AIS_IPHONE_PLATFORM
	im = new iPhoneInputManager();
#elif defined AIS_ANDROID_PLATFORM
	im = new AndroidInputManager();
#else
	AIS_EXCEPT(E_General, "No platform library.. check build platform defines!");
#endif 

	try
	{
		im->_initialize(paramList);
	}
	catch(...)
	{
		delete im;
		throw; //rethrow
	}

	return im;
}

//----------------------------------------------------------------------------//
void InputManager::destroyInputManager(InputManager* manager)
{
	if( manager == 0 )
		return;

	//Cleanup before deleting...
	for( FactoryCreatedObject::iterator i = manager->mFactoryObjects.begin(); 
		i != manager->mFactoryObjects.end(); ++i )
	{
		i->second->destroyObject( i->first );
	}

	manager->mFactoryObjects.clear();
	delete manager;
}

//--------------------------------------------------------------------------------//
const std::string& InputManager::inputManagerName()
{
	return mInputSystemName;
}

//--------------------------------------------------------------------------------//
int InputManager::getNumberOfDevices( AISDeviceType iType )
{
	//Count up all the factories devices
	int factoyObjects = 0;
	FactoryList::iterator i = mFactories.begin(), e = mFactories.end();
	for( ; i != e; ++i )
		factoyObjects += (*i)->totalDevices(iType);

	return factoyObjects;
}

//----------------------------------------------------------------------------//
DeviceList InputManager::listFreeDevices()
{
	DeviceList list;
	FactoryList::iterator i = mFactories.begin(), e = mFactories.end();
	for( ; i != e; ++i )
	{
		DeviceList temp = (*i)->freeDeviceList();
		list.insert(temp.begin(), temp.end());
	}

	return list;
}

//----------------------------------------------------------------------------//
DeviceObject* InputManager::createInputObject( AISDeviceType iType, bool bufferMode, const std::string &vendor )
{
	DeviceObject* obj = 0;
	FactoryList::iterator i = mFactories.begin(), e = mFactories.end();
	for( ; i != e; ++i)
	{
		if( (*i)->freeDevices(iType) > 0 )
		{
			if( vendor == "" || (*i)->vendorExist(iType, vendor) )
			{
				obj = (*i)->createObject(this, iType, bufferMode, vendor);
				mFactoryObjects[obj] = (*i);
				break;
			}
		}
	}

	if(!obj)
	{
		AIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");
	}

	if ( obj )
	{
		try
		{	
			//Intialize device
			obj->_initialize();
		}
		catch(...)
		{	//Somekind of error, cleanup and rethrow
			destroyInputObject(obj);
			throw;
		}
	}

	return obj;
}

//----------------------------------------------------------------------------//
void InputManager::destroyInputObject( DeviceObject* obj )
{
	if( obj == 0 )
		return;

	FactoryCreatedObject::iterator i = mFactoryObjects.find(obj);
	if( i != mFactoryObjects.end() )
	{
		i->second->destroyObject(obj);
		mFactoryObjects.erase(i);
	}
	else
	{
		AIS_EXCEPT(E_General, "Object creator not known.");
	}
}

//----------------------------------------------------------------------------//
void InputManager::addFactoryCreator( FactoryCreator* factory )
{
	if(factory != 0)
		mFactories.push_back(factory);
}

//----------------------------------------------------------------------------//
void InputManager::removeFactoryCreator( FactoryCreator* factory )
{
	if(factory != 0)
	{
		//First, destroy all devices created with the factory
		for( FactoryCreatedObject::iterator i = mFactoryObjects.begin(); i != mFactoryObjects.end(); ++i )
		{
			if( i->second == factory )
			{
				i->second->destroyObject(i->first);
				mFactoryObjects.erase(i++);
			}
		}

		//Now, remove the factory itself
		FactoryList::iterator fact = std::find(mFactories.begin(), mFactories.end(), factory);
		if( fact != mFactories.end() )
			mFactories.erase(fact);
	}
}
