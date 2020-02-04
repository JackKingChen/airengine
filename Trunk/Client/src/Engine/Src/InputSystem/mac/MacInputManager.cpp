#include "stdafx.h"

#ifdef AIS_APPLE_PLATFORM

#ifndef __LP64__

#include "mac/MacInputManager.h"
#include "mac/MacKeyboard.h"
#include "mac/MacMouse.h"
#include "mac/MacHIDManager.h"

#include <Carbon/Carbon.h>

#include <iostream>
using namespace std;

using namespace AIS;

//--------------------------------------------------------------------------------//
MacInputManager::MacInputManager() : InputManager("Mac OS X Input Manager")
{
    mHideMouse = true;
    mUseRepeat = false;
    mEventTargetRef = NULL;
	mWindow = NULL;

	keyboardUsed = mouseUsed = false;

	//Setup our internal factories
	mFactories.push_back(this);

	mHIDManager = new MacHIDManager();
	mFactories.push_back(mHIDManager);
}

//--------------------------------------------------------------------------------//
MacInputManager::~MacInputManager()
{
	delete mHIDManager;
}

//--------------------------------------------------------------------------------//
void MacInputManager::_initialize( ParamList &paramList )
{
	_parseConfigSettings( paramList );
    
	//Enumerate all devices attached
	_enumerateDevices();
	
	mHIDManager->initialize();
}

//--------------------------------------------------------------------------------//
void MacInputManager::_parseConfigSettings( ParamList &paramList )
{
    // Some carbon apps are running in a window, however full screen apps
	// do not have a window, so we need to account for that too.
	ParamList::iterator i = paramList.find("WINDOW");
	if(i != paramList.end())
	{
		mWindow = (WindowRef)strtoul(i->second.c_str(), 0, 10);
		if(mWindow == 0)
		{
			mWindow = NULL;
			mEventTargetRef = GetApplicationEventTarget();
		}
		else
		{
			//mEventTargetRef = GetWindowEventTarget(mWindow);
			mEventTargetRef = GetApplicationEventTarget();
		}
    }
	else
	{
		// else get the main active window.. user might not have access to it through some
		// graphics libraries, if that fails then try at the application level.
		mWindow = ActiveNonFloatingWindow();
		if(mWindow == NULL)
		{
			mEventTargetRef = GetApplicationEventTarget();
		}
		else
		{
			//mEventTargetRef = GetWindowEventTarget(mWindow);
			mEventTargetRef = GetApplicationEventTarget();
		}
	}
	
	if(mEventTargetRef == NULL)
		AIS_EXCEPT( E_General, "MacInputManager::_parseConfigSettings >> Unable to find a window or event target" );
    
    // Keyboard
    if(paramList.find("MacAutoRepeatOn") != paramList.end())
	{
        if(paramList.find("MacAutoRepeatOn")->second == "true")
		{
            mUseRepeat = true;
        }
    }
}

//--------------------------------------------------------------------------------//
void MacInputManager::_enumerateDevices()
{
}

//--------------------------------------------------------------------------------//
DeviceList MacInputManager::freeDeviceList()
{
	DeviceList ret;

	if( keyboardUsed == false )
		ret.insert(std::make_pair(AISKeyboard, mInputSystemName));

	if( mouseUsed == false )
		ret.insert(std::make_pair(AISMouse, mInputSystemName));

	return ret;
}

//--------------------------------------------------------------------------------//
int MacInputManager::totalDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISKeyboard: return 1;
	case AISMouse: return 1;
	default: return 0;
	}
}

//--------------------------------------------------------------------------------//
int MacInputManager::freeDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISKeyboard: return keyboardUsed ? 0 : 1;
	case AISMouse: return mouseUsed ? 0 : 1;
	default: return 0;
	}
}

//--------------------------------------------------------------------------------//
bool MacInputManager::vendorExist(AISDeviceType iType, const std::string & vendor)
{
	if( (iType == AISKeyboard || iType == AISMouse) && vendor == mInputSystemName )
		return true;

	return false;
}

//--------------------------------------------------------------------------------//
DeviceObject* MacInputManager::createObject(InputManager* creator, AISDeviceType iType, bool bufferMode,
									  const std::string & vendor)
{
	DeviceObject *obj = 0;

	switch(iType)
	{
	case AISKeyboard: 
	{
		if( keyboardUsed == false )
			obj = new MacKeyboard(this, bufferMode, mUseRepeat);
		break;
	}
	case AISMouse:
	{
		if( mouseUsed == false )
			obj = new MacMouse(this, bufferMode);
		break;
	}
	default:
	{
		obj = mHIDManager->createObject(creator, iType, bufferMode, vendor);
		break;
	}
	}

	if( obj == 0 )
		AIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");

	return obj;
}

//--------------------------------------------------------------------------------//
void MacInputManager::destroyObject(DeviceObject* obj)
{
	delete obj;
}

#endif

#endif //! AIS_APPLE_PLATFORM