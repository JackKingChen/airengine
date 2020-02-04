
#pragma once

#ifdef AIS_WIN32_PLATFORM

#include "AISKeyboard.h"
#include "win32/Win32Prereqs.h"

namespace AIS
{

	class Win32Keyboard : public Keyboard
	{
	public:
		Win32Keyboard(InputManager* creator, IDirectInput8* pDI, bool buffered, DWORD coopSettings);
		virtual ~Win32Keyboard();

		virtual bool isKeyDown(KeyCode key) const;
		
		virtual const std::string& getAsString(KeyCode kc);

		virtual void copyKeyStates(char keys[256]) const;

		virtual void setBuffered(bool buffered);
		
		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}
		
		virtual void _initialize();

	protected:
		void _readBuffered();
		void _read();
		int _translateText( KeyCode kc );

		IDirectInput8		*mDirectInput;
		IDirectInputDevice8	*mKeyboard;
		DWORD				mCoopSetting;
		unsigned char		mKeyBuffer[256];
		WCHAR				mDeadKey;
		std::string			mGetString;
	};

}

#endif //! AIS_WIN32_PLATFORM
