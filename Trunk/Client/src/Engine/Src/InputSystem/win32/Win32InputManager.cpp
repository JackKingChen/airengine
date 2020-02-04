#include "stdafx.h"
#include "win32/Win32InputManager.h"
#include "win32/Win32KeyBoard.h"
#include "win32/Win32Mouse.h"
#include "win32/Win32JoyStick.h"

#ifdef AIS_WIN32_PLATFORM


using namespace AIS;

//--------------------------------------------------------------------------------//
Win32InputManager::Win32InputManager() : InputManager("Win32InputManager")
{
	hWnd = 0;
	mDirectInput = 0;

	kbSettings    = 0;
	mouseSettings = 0;
	joySettings   = 0;

	joySticks = 0;
	keyboardUsed = mouseUsed = false;

	//Setup our internal factories
	mFactories.push_back(this);
}

//--------------------------------------------------------------------------------//
Win32InputManager::~Win32InputManager()
{
	if( mDirectInput )
	{
		mDirectInput->Release();
		mDirectInput = 0;
	}
}

//--------------------------------------------------------------------------------//
void Win32InputManager::_initialize( ParamList &paramList )
{
	HINSTANCE hInst = 0;
	HRESULT hr;


	//First of all, get the Windows Handle and Instance
	ParamList::iterator i = paramList.find("WINDOW");
	if( i == paramList.end() ) 
		AIS_EXCEPT( E_InvalidParam, "Win32InputManager::Win32InputManager >> No HWND found!" );

	// Get number as 64 bit and then convert. Handles the case of 32 or 64 bit HWND
	unsigned long handle = atol(i->second.c_str());
//#ifdef _MSC_VER
//	unsigned __int64 handle = _strtoui64(i->second.c_str(), 0, 10);
//#else
//    unsigned __int64 handle = strtoull(i->second.c_str(), 0, 10);
//#endif
	hWnd  = (HWND)handle;

	if( IsWindow(hWnd) == 0 )
		AIS_EXCEPT( E_General, "Win32InputManager::Win32InputManager >> The sent HWND is not valid!");

	hInst = GetModuleHandle(0);

	//Create the device
	hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&mDirectInput, NULL );
    if (FAILED(hr))	
		AIS_EXCEPT( E_General, "Win32InputManager::Win32InputManager >> Not able to init DirectX8 Input!");

	//Ok, now we have DirectInput, parse whatever extra settings were sent to us
	_parseConfigSettings( paramList );

	// Enumerate devices ...
	_enumerateDevices();
}

//--------------------------------------------------------------------------------//
void Win32InputManager::_parseConfigSettings( ParamList &paramList )
{
	//Here we pick up settings such as a device's cooperation mode
	std::map<std::string, DWORD> temp;
	temp["DISCL_BACKGROUND"]	= DISCL_BACKGROUND;
	temp["DISCL_EXCLUSIVE"]		= DISCL_EXCLUSIVE;
	temp["DISCL_FOREGROUND"]	= DISCL_FOREGROUND;
	temp["DISCL_NONEXCLUSIVE"]	= DISCL_NONEXCLUSIVE;
	temp["DISCL_NOWINKEY"]		= DISCL_NOWINKEY;

	//Check for pairs: ie. ("w32_keyboard","DISCL_NOWINKEY")("w32_keyboard","DISCL_FOREGROUND")
	ParamList::iterator i = paramList.begin(), e = paramList.end();
	for( ; i != e; ++i ) 
	{
		if( i->first == "w32_keyboard" )
				kbSettings |= temp[i->second];
		else if( i->first == "w32_mouse" )
				mouseSettings |= temp[i->second];
		else if( i->first == "w32_joystick" )
				joySettings |= temp[i->second];
	}
	if( kbSettings == 0 ) kbSettings = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY;
	if( mouseSettings == 0 ) mouseSettings = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
	if( joySettings == 0 ) joySettings = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
}

//--------------------------------------------------------------------------------//
void Win32InputManager::_enumerateDevices()
{
	//Enumerate all attached devices
	mDirectInput->EnumDevices(NULL, _DIEnumDevCallback, this, DIEDFL_ATTACHEDONLY);

#ifdef AIS_WIN32_XINPUT_SUPPORT
	//let's check how many possible XInput devices we may have (max 4)... 
	for(int i = 0; i < 3; ++i)
	{
		XINPUT_STATE state;
		if(XInputGetState(i, &state) != ERROR_DEVICE_NOT_CONNECTED)
		{	//Once we found 1, just check our whole list against devices
			Win32JoyStick::CheckXInputDevices(unusedJoyStickList);
			break;
		}
	}
#endif
}

//--------------------------------------------------------------------------------//
BOOL CALLBACK Win32InputManager::_DIEnumDevCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	Win32InputManager *_this_ = static_cast<Win32InputManager*>(pvRef);

	// Register only game devices (keyboard and mouse are managed differently).
	if( GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_JOYSTICK ||
		GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_GAMEPAD ||
		GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_1STPERSON ||
		GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_DRIVING ||
		GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_FLIGHT)
	{
		JoyStickInfo jsInfo;
		jsInfo.isXInput = false;
		jsInfo.productGuid = lpddi->guidProduct;
		jsInfo.deviceID = lpddi->guidInstance;
		jsInfo.vendor = lpddi->tszInstanceName;
		jsInfo.devId = _this_->joySticks;

		_this_->joySticks++;
		
		_this_->unusedJoyStickList.push_back( jsInfo );
	}

	return DIENUM_CONTINUE;
}

//----------------------------------------------------------------------------//
void Win32InputManager::_returnJoyStick(const JoyStickInfo& joystick)
{
	unusedJoyStickList.push_back(joystick);
}

//----------------------------------------------------------------------------//
DeviceList Win32InputManager::freeDeviceList()
{
	DeviceList ret;

	if( keyboardUsed == false )
		ret.insert(std::make_pair(AISKeyboard, mInputSystemName));

	if( mouseUsed == false )
		ret.insert(std::make_pair(AISMouse, mInputSystemName));

	for(JoyStickInfoList::iterator i = unusedJoyStickList.begin(); i != unusedJoyStickList.end(); ++i)
		ret.insert(std::make_pair(AISJoyStick, i->vendor));

	return ret;
}

//----------------------------------------------------------------------------//
int Win32InputManager::totalDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISKeyboard: return 1;
	case AISMouse: return 1;
	case AISJoyStick: return joySticks;
	default: return 0;
	}
}

//----------------------------------------------------------------------------//
int Win32InputManager::freeDevices(AISDeviceType iType)
{
	switch(iType)
	{
	case AISKeyboard: return keyboardUsed ? 0 : 1;
	case AISMouse: return mouseUsed ? 0 : 1;
	case AISJoyStick: return (int)unusedJoyStickList.size();
	default: return 0;
	}
}

//----------------------------------------------------------------------------//
bool Win32InputManager::vendorExist(AISDeviceType iType, const std::string & vendor)
{
	if( (iType == AISKeyboard || iType == AISMouse) && vendor == mInputSystemName )
	{
		return true;
	}
	else if( iType == AISJoyStick )
	{
		for(JoyStickInfoList::iterator i = unusedJoyStickList.begin(); i != unusedJoyStickList.end(); ++i)
			if(i->vendor == vendor)
				return true;
	}

	return false;
}

//----------------------------------------------------------------------------//
DeviceObject* Win32InputManager::createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor)
{
	DeviceObject *obj = 0;

	switch(iType)
	{
	case AISKeyboard: 
	{
		if( keyboardUsed == false )
			obj = new Win32Keyboard(this, mDirectInput, bufferMode, kbSettings);
		break;
	}
	case AISMouse:
	{
		if( mouseUsed == false )
			obj = new Win32Mouse(this, mDirectInput, bufferMode, mouseSettings);
		break;
	}
	case AISJoyStick:
	{
		for(JoyStickInfoList::iterator i = unusedJoyStickList.begin(); i != unusedJoyStickList.end(); ++i)
		{
			if(vendor == "" || i->vendor == vendor)
			{
				obj = new Win32JoyStick(this, mDirectInput, bufferMode, joySettings, *i);
				unusedJoyStickList.erase(i);
				break;
			}
		}
		break;
	}
	default:
		break;
	}

	if( obj == 0 )
		AIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");

	return obj;
}

//----------------------------------------------------------------------------//
void Win32InputManager::destroyObject(DeviceObject* obj)
{
	delete obj;
}


#endif //! AIS_WIN32_PLATFORM