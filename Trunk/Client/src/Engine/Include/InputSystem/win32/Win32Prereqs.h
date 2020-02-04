
#pragma once

#ifdef AIS_WIN32_PLATFORM

#include <cstddef>
#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>

#ifdef AIS_WIN32_XINPUT_SUPPORT
#	include <XInput.h>
#endif

//Max number of elements to collect from buffered DirectInput
#define KEYBOARD_DX_BUFFERSIZE	17
#define MOUSE_DX_BUFFERSIZE		128
#define JOYSTICK_DX_BUFFERSIZE	129

//MinGW defines
#if defined(AIS_MINGW_COMPILER)
#	undef FIELD_OFFSET
#	define FIELD_OFFSET			offsetof
#endif

namespace AIS
{

	class Win32InputManager;
	class Win32Keyboard;
	class Win32JoyStick;
	class Win32Mouse;
	class Win32ForceFeedback;

	class JoyStickInfo
	{
	public:
		int			devId;
		GUID		deviceID;
		GUID		productGuid;
		std::string vendor;
		bool		isXInput;
		int			xInputDev;
	};

	typedef std::vector<JoyStickInfo> JoyStickInfoList;

}

#endif //! AIS_WIN32_PLATFORM
