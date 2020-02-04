#include "stdafx.h"

#ifdef AIS_APPLE_PLATFORM

#include "mac/CocoaInputManager.h"
#include "mac/CocoaKeyboard.h"
#include "mac/CocoaMouse.h"
#include "mac/MacHIDManager.h"


using namespace std;
using namespace AIS;

//--------------------------------------------------------------------------------//
CocoaInputManager::CocoaInputManager() : InputManager("Mac OS X Cocoa Input Manager")
{
    mHideMouse = true;
    mUseRepeat = false;
	mWindow = nil;

	keyboardUsed = mouseUsed = false;

	//Setup our internal factories
	mFactories.push_back(this);

	mHIDManager = new MacHIDManager();
	mFactories.push_back(mHIDManager);
}

//--------------------------------------------------------------------------------//
CocoaInputManager::~CocoaInputManager()
{
	delete mHIDManager;
}

//--------------------------------------------------------------------------------//
void CocoaInputManager::_initialize( ParamList &paramList )
{
	_parseConfigSettings( paramList );
    
	//Enumerate all devices attached
	_enumerateDevices();
	
	mHIDManager->initialize();
}

//--------------------------------------------------------------------------------//
void CocoaInputManager::_parseConfigSettings( ParamList &paramList )
{
    // Some carbon apps are running in a window, however full screen apps
	// do not have a window, so we need to account for that too.
	ParamList::iterator i = paramList.find("WINDOW");
	if(i != paramList.end())
	{
		mWindow = (NSWindow *)strtoul(i->second.c_str(), 0, 10);
		if(mWindow == 0)
		{
			mWindow = nil;
		}
    }
	else
	{
		// else get the main active window.. user might not have access to it through some
		// graphics libraries, if that fails then try at the application level.
		mWindow = [[NSApplication sharedApplication] keyWindow];
	}
	
	if(mWindow == nil)
		AIS_EXCEPT( E_General, "CocoaInputManager::_parseConfigSettings >> Unable to find a window or event target" );
    
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
void CocoaInputManager::_enumerateDevices()
{
}

//--------------------------------------------------------------------------------//
DeviceList CocoaInputManager::freeDeviceList()
{
	DeviceList ret;

	if( keyboardUsed == false )
		ret.insert(std::make_pair(AISKeyboard, mInputSystemName));

	if( mouseUsed == false )
		ret.insert(std::make_pair(AISMouse, mInputSystemName));

	return ret;
}

//--------------------------------------------------------------------------------//
int CocoaInputManager::totalDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISKeyboard: return 1;
	case AISMouse: return 1;
	default: return 0;
	}
}

//--------------------------------------------------------------------------------//
int CocoaInputManager::freeDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISKeyboard: return keyboardUsed ? 0 : 1;
	case AISMouse: return mouseUsed ? 0 : 1;
	default: return 0;
	}
}

//--------------------------------------------------------------------------------//
bool CocoaInputManager::vendorExist(AISDeviceType iType, const std::string & vendor)
{
	if( (iType == AISKeyboard || iType == AISMouse) && vendor == mInputSystemName )
		return true;

	return false;
}

//--------------------------------------------------------------------------------//
DeviceObject* CocoaInputManager::createObject(InputManager* creator, AISDeviceType iType, bool bufferMode,
									  const std::string & vendor)
{
	DeviceObject *obj = 0;

	switch(iType)
	{
        case AISKeyboard: 
        {
            if( keyboardUsed == false )
                obj = new CocoaKeyboard(this, bufferMode, mUseRepeat);
            break;
        }
        case AISMouse:
        {
            if( mouseUsed == false )
                obj = new CocoaMouse(this, bufferMode);
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
void CocoaInputManager::destroyObject(DeviceObject* obj)
{
	delete obj;
}



#endif //! AIS_APPLE_PLATFORM