#include "stdafx.h"

#ifdef AIS_APPLE_PLATFORM

#ifndef __LP64__

#include "mac/MacHelpers.h"
#include "mac/MacKeyboard.h"
#include "mac/MacMouse.h"

#include <Carbon/Carbon.h>

using namespace AIS;
    
//-------------------------------------------------------------------//
OSStatus KeyDownWrapper( EventHandlerCallRef nextHandler,
                        EventRef               theEvent,
                        void*                  callClass )
{
    // TODO find a better way. This cast isn't very safe
    if (callClass != NULL)
    {
        ((MacKeyboard*)callClass)->_keyDownCallback( theEvent );
        
        // propagate the event down the chain
        return CallNextEventHandler( nextHandler, theEvent );
    }
    else
    {
        AIS_EXCEPT(E_General, "KeyDownWrapper >> Being called by something other than our event handler!");
        return noErr;
    }
}


//-------------------------------------------------------------------//
OSStatus KeyUpWrapper( EventHandlerCallRef nextHandler,
                       EventRef               theEvent,
                       void*                  callClass )
{
    if (callClass != NULL)
    {
        ((MacKeyboard*)callClass)->_keyUpCallback( theEvent );
        
        // propagate the event down the chain
        return CallNextEventHandler( nextHandler, theEvent );
    }
    else
    {
        AIS_EXCEPT(E_General, "KeyUpWrapper >> Being called by something other than our event handler!");
        return noErr;
    }
}


//-------------------------------------------------------------------//
OSStatus KeyModWrapper( EventHandlerCallRef nextHandler,
                        EventRef               theEvent,
                        void*                  callClass )
{
    if (callClass != NULL)
    {
        ((MacKeyboard*)callClass)->_modChangeCallback( theEvent );
        
        // propagate the event down the chain
        return CallNextEventHandler( nextHandler, theEvent );
        
    }
    else
    {
        AIS_EXCEPT(E_General, "KeyModWrapper >> Being called by something other than our event handler!");
        return noErr;
    }
}

/*
//-------------------------------------------------------------------//
OSStatus MouseMoveWrapper( EventHandlerCallRef nextHandler,
                           EventRef            theEvent,
                           void*               callClass )
{
    if (callClass != NULL) 
    {
        ((MacMouse*)callClass)->_mouseMoveCallback( theEvent );
        
        // propagate the event down the chain
        return CallNextEventHandler( nextHandler, theEvent );
        
    }
    else 
    {
        AIS_EXCEPT(E_General, "MouseMoveWrapper >> Being called by something other than our event handler!");
        return noErr;
    }
}


//-------------------------------------------------------------------//
OSStatus MouseScrollWrapper( EventHandlerCallRef nextHandler,
                             EventRef            theEvent,
                             void*               callClass )
{
    if (callClass != NULL) 
    {
        ((MacMouse*)callClass)->_mouseScrollCallback( theEvent );
        
        // propagate the event down the chain
        return CallNextEventHandler( nextHandler, theEvent );
        
    }
    else 
    {
        AIS_EXCEPT(E_General, "MouseScrollWrapper >> Being called by something other than our event handler!");
        return noErr;
    }
}


//-------------------------------------------------------------------//
OSStatus MouseButtonWrapper( EventHandlerCallRef nextHandler,
                             EventRef            theEvent,
                             void*               callClass )
{
    if (callClass != NULL) 
    {
        ((MacMouse*)callClass)->_mouseButtonCallback( theEvent );
        
        // propagate the event down the chain
        return CallNextEventHandler( nextHandler, theEvent );
        
    }
    else 
    {
        AIS_EXCEPT(E_General, "MouseButtonWrapper >> Being called by something other than our event handler!");
        return noErr;
    }
}
*/

//-------------------------------------------------------------------//
OSStatus MouseWrapper( EventHandlerCallRef nextHandler, EventRef theEvent, void* callClass )
{
    if (callClass != NULL)
	{
        ((MacMouse*)callClass)->_mouseCallback( theEvent );

        // propagate the event down the chain
        return CallNextEventHandler( nextHandler, theEvent );        
    }
    else
        AIS_EXCEPT(E_General, "MouseWrapper >> Being called by something other than our event handler!");
}

#endif

#endif //! AIS_APPLE_PLATFORM