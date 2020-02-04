
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISKeyboard.h"
#include "AISMouse.h"

// This is needed for keeping an event stack for keyboard and mouse
namespace AIS
{
    
    // used in the eventStack to store the type
    enum Mac_EventType 
	{ 
		MAC_KEYUP = 0,
		MAC_KEYDOWN = 1,
		MAC_KEYREPEAT,
		MAC_MOUSEDOWN,
		MAC_MOUSEUP,
		MAC_MOUSEMOVED,
		MAC_MOUSESCROLL
	};
    typedef enum Mac_EventType MacEventType;
}

#endif //!AIS_APPLE_PLATFORM
