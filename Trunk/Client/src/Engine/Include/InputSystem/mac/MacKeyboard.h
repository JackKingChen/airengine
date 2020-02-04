
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISKeyboard.h"
#include "mac/MacHelpers.h"
#include "mac/MacPrereqs.h"

#include <Carbon/Carbon.h>

namespace AIS
{
    
    class MacKeyboard : public Keyboard
    {
    public:
        MacKeyboard( InputManager* creator, bool buffered, bool repeat );
        virtual ~MacKeyboard();
        
        virtual void setBuffered( bool buffered );
        
        virtual bool isKeyDown( KeyCode key ) const;
        
        virtual void capture();
        
        virtual void copyKeyStates( char keys[256] ) const;
        
        virtual std::string& getAsString( KeyCode key );
        
        virtual Interface* queryInterface( Interface::IType type ) { return 0; }
        
        
        virtual void _initialize();
        
        void _keyDownCallback( EventRef theEvent );
        void _keyUpCallback( EventRef theEvent );
        void _modChangeCallback( EventRef theEvent );
        

    protected:
        // just to get this out of the way
        void populateKeyConversion();
        
        // updates the keybuffer and optionally the eventStack
        void injectEvent(KeyCode kc, unsigned int time, MacEventType type, unsigned int txt = 0 );
                
	protected:
        typedef std::map<UInt32, KeyCode> VirtualtoAIS_KeyMap;
        VirtualtoAIS_KeyMap keyConversion;
        
        std::string		getString;
        
        char KeyBuffer[256];
        UInt32			prevModMask;        
        
        // "universal procedure pointers" - required reference for callbacks
        EventHandlerUPP keyDownUPP;
        EventHandlerUPP keyUpUPP;
        EventHandlerUPP keyModUPP;
        
        // so we can delete the handlers on destruction
        EventHandlerRef keyDownEventRef;
        EventHandlerRef keyUpEventRef;
        EventHandlerRef keyModEventRef;
        
        // buffered events, fifo stack
        typedef std::list<MacKeyStackEvent> eventStack;
        eventStack		pendingEvents;        
        bool			useRepeat;
        
    };
}

#endif //!AIS_APPLE_PLATFORM
