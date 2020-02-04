
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISMouse.h"
#include "mac/MacHelpers.h"
#include "mac/MacPrereqs.h"

#include <Carbon/Carbon.h>

namespace AIS
{
	class MacMouse : public Mouse
    {
	public:
		MacMouse( InputManager* creator, bool buffered );
		virtual ~MacMouse();
		
		virtual void setBuffered(bool buffered);

		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		virtual void _initialize();
        
	public:
        void _mouseCallback( EventRef theEvent );
        
        virtual void setMouseShow( bool bShow );

		virtual void getClientRect( int& x, int& y, int& width, int& height );
		virtual void setMousePos( int x, int y );

	protected:
		static OSStatus WindowFocusChanged(EventHandlerCallRef nextHandler, EventRef event, void* macMouse);
        
        // "universal procedure pointers" - required reference for callbacks
		EventHandlerUPP mouseUPP;
		EventHandlerRef mouseEventRef;
		
		EventHandlerUPP mWindowFocusListener;
		EventHandlerRef mWindowFocusHandler;
		
		bool mNeedsToRegainFocus;
		bool mMouseWarped;
	};
}

#endif //!AIS_APPLE_PLATFORM
