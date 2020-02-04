
#pragma once

#ifdef AIS_WIN32_PLATFORM

#include "AISInputManager.h"
#include "AISFactoryCreator.h"
#include "win32/Win32Prereqs.h"

namespace AIS
{

	/**	Win32InputManager specialization - Using DirectInput8 */
	class Win32InputManager : public InputManager, public FactoryCreator
	{
	public:
		Win32InputManager();
		virtual ~Win32InputManager();
		void _initialize( ParamList &paramList );

		DeviceList freeDeviceList();

		int totalDevices(AISDeviceType iType);

		int freeDevices(AISDeviceType iType);

		bool vendorExist(AISDeviceType iType, const std::string & vendor);

		DeviceObject* createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor = "");

		void destroyObject(DeviceObject* obj);

		void _setKeyboardUsed(bool used) {keyboardUsed = used; }

		void _setMouseUsed(bool used) { mouseUsed = used; }

		void _returnJoyStick(const JoyStickInfo& joystick);

		HWND getWindowHandle() { return hWnd; }

	protected:
		void _parseConfigSettings( ParamList &paramList );
		void _enumerateDevices();
		static BOOL CALLBACK _DIEnumDevCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	protected:
		JoyStickInfoList	unusedJoyStickList;
		HWND				hWnd;
		IDirectInput8		*mDirectInput;
		DWORD				kbSettings;
		DWORD				mouseSettings;
		DWORD				joySettings;
		char				joySticks;
		bool				keyboardUsed;
		bool				mouseUsed;
	};

}

#endif //! AIS_WIN32_PLATFORM
