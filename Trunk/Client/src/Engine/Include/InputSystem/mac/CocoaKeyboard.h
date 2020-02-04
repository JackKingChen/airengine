
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISKeyboard.h"
#include "mac/CocoaHelpers.h"

#include <list>
#include <Cocoa/Cocoa.h>

@class CocoaKeyboardView;

using namespace AIS;
using namespace std;

namespace AIS
{
    typedef class Cocoa_KeyStackEvent
    {
        friend class CocoaKeyboard;
        
    public:
        Cocoa_KeyStackEvent( KeyEvent event, MacEventType type ) : Type(type), Event(event) {}
        const MacEventType type()   { return Type; }
        const KeyEvent event()      { return Event; }
    private:
        MacEventType Type;
        KeyEvent Event;
    } CocoaKeyStackEvent;
    
    class CocoaKeyboard : public Keyboard
    {
    public:
        CocoaKeyboard( InputManager* creator, bool buffered, bool repeat );
        virtual ~CocoaKeyboard();

        virtual void setBuffered( bool buffered );

        virtual bool isKeyDown( KeyCode key ) const;

        virtual void capture();

        virtual void copyKeyStates( char keys[256] ) const;

        virtual std::string& getAsString( KeyCode key );

        virtual Interface* queryInterface( Interface::IType type ) { return 0; }

        virtual void _initialize();
        
        unsigned int _getModifiers() { return mModifiers; }
        void _setModifiers(unsigned int mods) { mModifiers = mods; }

    protected:
        CocoaKeyboardView	*mResponder;
        std::string			getString;
    };
}

typedef std::map<unsigned short, KeyCode> VirtualtoAIS_KeyMap;
typedef std::list<AIS::CocoaKeyStackEvent> eventStack;

@interface CocoaKeyboardView : NSResponder
{
    CocoaKeyboard		*oisKeyboardObj;
    VirtualtoAIS_KeyMap keyConversion;

    char				KeyBuffer[256];
    NSUInteger			prevModMask;
    
    // buffered events, fifo stack
    eventStack			pendingEvents;
    bool				useRepeat;
}

- (void)setAISKeyboardObj:(CocoaKeyboard *)obj;
- (void)populateKeyConversion;
- (void)capture;
- (void)injectEvent:(KeyCode)kc eventTime:(unsigned int)time eventType:(MacEventType)type;
- (void)injectEvent:(KeyCode)kc eventTime:(unsigned int)time eventType:(MacEventType)type eventText:(unsigned int)txt;
- (void)copyKeyStates:(char [256])keys;
- (bool)isKeyDown:(KeyCode)key;
- (void)setUseRepeat:(bool)repeat;
- (VirtualtoAIS_KeyMap)keyConversionMap;

@end


#endif //!AIS_APPLE_PLATFORM
