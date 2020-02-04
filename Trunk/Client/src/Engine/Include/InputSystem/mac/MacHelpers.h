
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "mac/MacPrereqs.h"
#include "AISKeyboard.h"
#include "AISMouse.h"

#include <Carbon/Carbon.h>

// This is a hack needed to get the event handler working. 
// The carbon lib expects a "OSStatus (*)(EventHandlerCallRef, EventRef, void*)",
// so I cannot give it a class member function (unless it is static which is pointless)
// Instead, I just pass the class* through the last paramter that gets passed to the
// callback every time an event occurs. Then I dereference it and call the member function.
OSStatus KeyDownWrapper( EventHandlerCallRef nextHandler, EventRef theEvent, void* callClass );

OSStatus KeyUpWrapper( EventHandlerCallRef nextHandler, EventRef theEvent, void* callClass );

OSStatus KeyModWrapper( EventHandlerCallRef nextHandler, EventRef theEvent, void* callClass );

OSStatus MouseWrapper( EventHandlerCallRef nextHandler, EventRef theEvent, void* callClass );


// This is needed for keeping an event stack for keyboard and mouse
namespace AIS
{
    
    // used in the eventStack to store the type
    enum Mac_EventType { MAC_KEYUP = 0,
                         MAC_KEYDOWN = 1,
                         MAC_KEYREPEAT,
                         MAC_MOUSEDOWN,
                         MAC_MOUSEUP,
                         MAC_MOUSEMOVED,
                         MAC_MOUSESCROLL};
    typedef enum Mac_EventType MacEventType;

    // only used by MacKeyboard
    typedef class Mac_KeyStackEvent
    {
        friend class MacKeyboard;

    private:
        Mac_KeyStackEvent( KeyEvent event, MacEventType type ) : Type(type), Event(event) {}
        
        MacEventType Type;
        KeyEvent Event;
    }
    MacKeyStackEvent;
}

#endif //!AIS_APPLE_PLATFORM
