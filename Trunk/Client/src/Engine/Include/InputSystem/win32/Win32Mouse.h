
#pragma once

#ifdef AIS_WIN32_PLATFORM

#include "AISMouse.h"
#include "win32/Win32Prereqs.h"

namespace AIS
{

	class Win32Mouse : public Mouse
	{
	public:
		Win32Mouse( InputManager* creator, IDirectInput8* pDI, bool buffered, DWORD coopSettings );
		virtual ~Win32Mouse();

		virtual void setBuffered(bool buffered);

		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		virtual void _initialize();

		virtual void setMouseShow( bool bShow );

		virtual void getClientRect( int& x, int& y, int& width, int& height );
		virtual void setMousePos( int x, int y );

	protected:
		bool _doMouseClick( int mouseButton, DIDEVICEOBJECTDATA& di );

		IDirectInput8		*mDirectInput;
		IDirectInputDevice8	*mMouse;
		DWORD				mCoopSetting;
		HWND				mHwnd;
	};

}

#endif //! AIS_WIN32_PLATFORM
