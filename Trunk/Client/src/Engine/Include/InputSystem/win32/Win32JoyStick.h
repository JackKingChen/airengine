
#pragma once

#ifdef AIS_WIN32_PLATFORM

#include "AISJoyStick.h"
#include "win32/Win32Prereqs.h"

namespace AIS
{

	class Win32JoyStick : public JoyStick
	{
	public:
		Win32JoyStick( InputManager* creator, IDirectInput8* pDI, bool buffered, DWORD coopSettings, const JoyStickInfo &info );
		virtual ~Win32JoyStick();
		
		virtual void setBuffered(bool buffered);
		
		virtual void capture();

		void captureXInput();

		virtual Interface* queryInterface(Interface::IType type);

		virtual void _initialize();

#ifdef AIS_WIN32_XINPUT_SUPPORT

		static void CheckXInputDevices(JoyStickInfoList &joys);
#endif

	protected:
		void _enumerate();
		static BOOL CALLBACK DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
		static BOOL CALLBACK DIEnumEffectsCallback(LPCDIEFFECTINFO pdei, LPVOID pvRef);

		bool _doButtonClick( int button, DIDEVICEOBJECTDATA& di );
		bool _changePOV( int pov, DIDEVICEOBJECTDATA& di );

	protected:
		IDirectInput8		*mDirectInput;
		IDirectInputDevice8	*mJoyStick;
		DIDEVCAPS			mDIJoyCaps;
		DWORD				mCoopSetting;
        JoyStickInfo		mJoyInfo;
		Win32ForceFeedback	*mFfDevice;
		int					mAxisNumber;
	};

}

#endif //! AIS_WIN32_PLATFORM
