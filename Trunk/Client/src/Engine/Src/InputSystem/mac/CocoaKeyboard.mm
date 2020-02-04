#include "stdafx.h"

#ifdef AIS_APPLE_PLATFORM

#include "mac/CocoaKeyboard.h"
#include "mac/CocoaInputManager.h"
#include "mac/CocoaHelpers.h"


#include <Cocoa/Cocoa.h>

#include <list>
#include <string>
#include <iostream>

using namespace AIS;

//-------------------------------------------------------------------//
CocoaKeyboard::CocoaKeyboard( InputManager* creator, bool buffered, bool repeat )
	: Keyboard(creator->inputManagerName(), buffered, 0, creator)
{
	CocoaInputManager *man = static_cast<CocoaInputManager*>(mCreator);
    mResponder = [[CocoaKeyboardView alloc] init];
    if(!mResponder)
        AIS_EXCEPT( E_General, "CocoaKeyboardView::CocoaKeyboardView >> Error creating event responder" );

    [man->_getWindow() makeFirstResponder:mResponder];
    [mResponder setUseRepeat:repeat];
    [mResponder setAISKeyboardObj:this];
	mModifiers = 0;

	static_cast<CocoaInputManager*>(mCreator)->_setKeyboardUsed(true);
}

//-------------------------------------------------------------------//
CocoaKeyboard::~CocoaKeyboard()
{
    if (mResponder)
    {
        [mResponder release];
        mResponder = nil;
    }

	// Free the input managers keyboard
	static_cast<CocoaInputManager*>(mCreator)->_setKeyboardUsed(false);
}

//-------------------------------------------------------------------//
void CocoaKeyboard::_initialize()
{
	mModifiers = 0;
}

//-------------------------------------------------------------------//
bool CocoaKeyboard::isKeyDown( KeyCode key ) const
{
	return [mResponder isKeyDown:key];
}

//-------------------------------------------------------------------//
void CocoaKeyboard::capture()
{
    [mResponder capture];
}

//-------------------------------------------------------------------//
std::string& CocoaKeyboard::getAsString( KeyCode key )
{
	getString = "";
    
    CGKeyCode deviceKeycode = 0;
    
    // Convert AIS KeyCode back into device keycode
    VirtualtoAIS_KeyMap keyMap = [mResponder keyConversionMap];
    for(VirtualtoAIS_KeyMap::iterator it = keyMap.begin(); it != keyMap.end(); ++it)
    {
        if(it->second == key)
            deviceKeycode = it->first;
    }

    UniChar unicodeString[1];
    UniCharCount actualStringLength;

    CGEventSourceRef sref = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    CGEventRef ref = CGEventCreateKeyboardEvent(sref, deviceKeycode, true);
    CGEventKeyboardGetUnicodeString(ref, sizeof(unicodeString) / sizeof(*unicodeString), &actualStringLength, unicodeString);
    CFRelease(ref);
    CFRelease(sref);
    getString = unicodeString[0];

    return getString;
}

//-------------------------------------------------------------------//
void CocoaKeyboard::setBuffered( bool buffered )
{
	mBuffered = buffered;
}

//-------------------------------------------------------------------//
void CocoaKeyboard::copyKeyStates( char keys[256] ) const
{
	[mResponder copyKeyStates:keys];
}

@implementation CocoaKeyboardView

- (id)init
{
    self = [super init];
    if (self) {
        [self populateKeyConversion];
        memset( &KeyBuffer, 0, 256 );
        prevModMask = 0;
    }
    return self;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (void)setAISKeyboardObj:(CocoaKeyboard *)obj
{
    oisKeyboardObj = obj;
}

- (void)capture
{
	// If not buffered just return, we update the unbuffered automatically
	if ( !oisKeyboardObj->buffered() && !oisKeyboardObj->getEventCallback() )
		return;

	// Run through our event stack
	eventStack::iterator cur_it;
	
	for (cur_it = pendingEvents.begin(); cur_it != pendingEvents.end(); cur_it++)
	{
		if ( (*cur_it).type() == MAC_KEYDOWN || (*cur_it).type() == MAC_KEYREPEAT)
			oisKeyboardObj->getEventCallback()->keyPressed( (*cur_it).event() );
		else if ( (*cur_it).type() == MAC_KEYUP )
			oisKeyboardObj->getEventCallback()->keyReleased( (*cur_it).event() );
	}
	
	pendingEvents.clear();
}

- (void)setUseRepeat:(bool)repeat
{
    useRepeat = repeat;
}

- (bool)isKeyDown:(KeyCode)key
{
    return KeyBuffer[key];
}

- (void)copyKeyStates:(char [256])keys
{
	memcpy( keys, KeyBuffer, 256 );
}

- (void)populateKeyConversion
{
	// Virtual Key Map to KeyCode
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x12, KC_1));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x13, KC_2));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x14, KC_3));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x15, KC_4));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x17, KC_5));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x16, KC_6));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x1A, KC_7));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x1C, KC_8));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x19, KC_9));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x1D, KC_0));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x33, KC_BACK));  // might be wrong
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x1B, KC_MINUS));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x18, KC_EQUALS));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x31, KC_SPACE));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x2B, KC_COMMA));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x2F, KC_PERIOD));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x2A, KC_BACKSLASH));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x2C, KC_SLASH));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x21, KC_LBRACKET));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x1E, KC_RBRACKET));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x35, KC_ESCAPE));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x39, KC_CAPITAL));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x30, KC_TAB));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x24, KC_RETURN));  // double check return/enter
	
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_colon, KC_COLON));	 // no colon?
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x29, KC_SEMICOLON));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x27, KC_APOSTROPHE));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x32, KC_GRAVE));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x0B, KC_B));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x00, KC_A));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x08, KC_C));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x02, KC_D));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x0E, KC_E));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x03, KC_F));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x05, KC_G));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x04, KC_H));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x22, KC_I));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x26, KC_J));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x28, KC_K));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x25, KC_L));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x2E, KC_M));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x2D, KC_N));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x1F, KC_O));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x23, KC_P));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x0C, KC_Q));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x0F, KC_R));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x01, KC_S));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x11, KC_T));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x20, KC_U));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x09, KC_V));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x0D, KC_W));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x07, KC_X));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x10, KC_Y));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x06, KC_Z));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x7A, KC_F1));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x78, KC_F2));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x63, KC_F3));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x76, KC_F4));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x60, KC_F5));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x61, KC_F6));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x62, KC_F7));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x64, KC_F8));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x65, KC_F9));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x6D, KC_F10));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x67, KC_F11));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x6F, KC_F12));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x69, KC_F13));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x6B, KC_F14));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x71, KC_F15));
	
	// Keypad
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x52, KC_NUMPAD0));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x53, KC_NUMPAD1));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x54, KC_NUMPAD2));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x55, KC_NUMPAD3));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x56, KC_NUMPAD4));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x57, KC_NUMPAD5));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x58, KC_NUMPAD6));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x59, KC_NUMPAD7));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x5B, KC_NUMPAD8));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x5C, KC_NUMPAD9));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x45, KC_ADD));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x4E, KC_SUBTRACT));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x41, KC_DECIMAL));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x51, KC_NUMPADEQUALS));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x4B, KC_DIVIDE));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x43, KC_MULTIPLY));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x4C, KC_NUMPADENTER));
	
	// Keypad with numlock off
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x73, KC_NUMPAD7));  // not sure of these
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Up, KC_NUMPAD8)); // check on a non-laptop
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Page_Up, KC_NUMPAD9));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Left, KC_NUMPAD4));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Begin, KC_NUMPAD5));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Right, KC_NUMPAD6));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_End, KC_NUMPAD1));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Down, KC_NUMPAD2));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Page_Down, KC_NUMPAD3));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Insert, KC_NUMPAD0));
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_KP_Delete, KC_DECIMAL));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x7E, KC_UP));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x7D, KC_DOWN));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x7B, KC_LEFT));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x7C, KC_RIGHT));
	
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x74, KC_PGUP));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x79, KC_PGDOWN));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x73, KC_HOME));
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x77, KC_END));
	
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_Print, KC_SYSRQ));		// ??
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_Scroll_Lock, KC_SCROLL)); // ??
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_Pause, KC_PAUSE));		// ??
	
	
	//keyConversion.insert(VirtualtoAIS_KeyMap::value_type(XK_Insert, KC_INSERT));	  // ??
	keyConversion.insert(VirtualtoAIS_KeyMap::value_type(0x75, KC_DELETE)); // del under help key?
}

- (void)injectEvent:(KeyCode)kc eventTime:(unsigned int)time eventType:(MacEventType)type
{
    [self injectEvent:kc eventTime:time eventType:type eventText:0];
}

- (void)injectEvent:(KeyCode)kc eventTime:(unsigned int)time eventType:(MacEventType)type eventText:(unsigned int)txt
{
	// set to 1 if this is either a keydown or repeat
	KeyBuffer[kc] = ( type == MAC_KEYUP ) ? 0 : 1;
    EventType evttype = (type == MAC_KEYUP) ? ET_KeyUp : ET_KeyDown;
	
	if ( oisKeyboardObj->buffered() && oisKeyboardObj->getEventCallback() )
		pendingEvents.push_back( CocoaKeyStackEvent( KeyEvent(evttype, oisKeyboardObj, kc, txt), type) );
}

#pragma mark Key Event overrides
- (void)keyDown:(NSEvent *)theEvent
{
	unsigned short virtualKey = [theEvent keyCode];
	unsigned int time = (unsigned int)[theEvent timestamp];
	KeyCode kc = keyConversion[virtualKey];

	// Record what kind of text we should pass the KeyEvent
	unichar text[10];
	char macChar;
	if (oisKeyboardObj->getTextTranslation() == AIS::Keyboard::Unicode)
	{
		// Get string size
		NSUInteger stringsize = [[theEvent charactersIgnoringModifiers] length];
        [[theEvent charactersIgnoringModifiers] getCharacters:text range:NSMakeRange(0, stringsize)];
//		NSLog(@"Characters: %ls", text);
//		std::cout << "String length: " << stringsize << std::endl;

		if(stringsize > 0)
		{
			// For each unicode char, send an event
			for ( unsigned int i = 0; i < stringsize; i++ )
			{
                [self injectEvent:kc eventTime:time eventType:MAC_KEYDOWN eventText:(unsigned int)text[i]];
			}
		}
	} 
	else if (oisKeyboardObj->getTextTranslation() == AIS::Keyboard::Ascii)
	{
        macChar = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
		[self injectEvent:kc eventTime:time eventType:MAC_KEYDOWN eventText:(unsigned int)macChar];
	}
	else
	{
		[self injectEvent:kc eventTime:time eventType:MAC_KEYDOWN];
	}
    
    // Handle modifiers
    NSUInteger mods = [theEvent modifierFlags];
    unsigned int modsRef = oisKeyboardObj->_getModifiers();
//    cout << "Modifiers before: 0x" << hex << modsRef << endl;
    if(mods & NSShiftKeyMask)
    {
        modsRef |= AIS::Keyboard::Shift;
        [self injectEvent:KC_LSHIFT eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSAlternateKeyMask)
    {
        modsRef |= AIS::Keyboard::Alt;
        [self injectEvent:KC_LMENU eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSControlKeyMask)
    {
        modsRef |= AIS::Keyboard::Ctrl;
        [self injectEvent:KC_LCONTROL eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSCommandKeyMask)
    {
        [self injectEvent:KC_LWIN eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSFunctionKeyMask)
    {
        [self injectEvent:KC_APPS eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSAlphaShiftKeyMask)
    {
        [self injectEvent:KC_CAPITAL eventTime:time eventType:MAC_KEYDOWN];
    }
    
    if([theEvent keyCode] == NSClearLineFunctionKey) // numlock
        [self injectEvent:KC_NUMLOCK eventTime:time eventType:MAC_KEYDOWN];
    
//    cout << "Modifiers after: 0x" << hex << modsRef << endl;
    oisKeyboardObj->_setModifiers(modsRef);
}

- (void)keyUp:(NSEvent *)theEvent
{
    unsigned short virtualKey = [theEvent keyCode];
	unsigned int time = (unsigned int)[theEvent timestamp];

	KeyCode kc = keyConversion[virtualKey];
    [self injectEvent:kc eventTime:[theEvent timestamp] eventType:MAC_KEYUP];

    // Handle modifiers
    NSUInteger mods = [theEvent modifierFlags];
    unsigned int modsRef = oisKeyboardObj->_getModifiers();
//    cout << "Modifiers before: 0x" << hex << modsRef << endl;
    if(mods & NSShiftKeyMask)
    {
        modsRef &= ~AIS::Keyboard::Shift;
        [self injectEvent:KC_LSHIFT eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSAlternateKeyMask)
    {
        modsRef &= ~AIS::Keyboard::Alt;
        [self injectEvent:KC_LMENU eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSControlKeyMask)
    {
        modsRef &= ~AIS::Keyboard::Ctrl;
        [self injectEvent:KC_LCONTROL eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSCommandKeyMask)
    {
        [self injectEvent:KC_LWIN eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSFunctionKeyMask)
    {
        [self injectEvent:KC_APPS eventTime:time eventType:MAC_KEYDOWN];
    }
    if(mods & NSAlphaShiftKeyMask)
    {
        [self injectEvent:KC_CAPITAL eventTime:time eventType:MAC_KEYDOWN];
    }
    
    if([theEvent keyCode] == NSClearLineFunctionKey) // numlock
        [self injectEvent:KC_NUMLOCK eventTime:time eventType:MAC_KEYDOWN];
    
//    cout << "Modifiers after: 0x" << hex << modsRef << endl;
    oisKeyboardObj->_setModifiers(modsRef);
}

- (void)flagsChanged:(NSEvent *)theEvent
{
	NSUInteger mods = [theEvent modifierFlags];
	
	// Find the changed bit
	NSUInteger change = prevModMask ^ mods;
	MacEventType newstate = ((change & prevModMask) > 0) ? MAC_KEYUP : MAC_KEYDOWN;
	unsigned int time = (unsigned int)[theEvent timestamp];
    unsigned int modsRef = oisKeyboardObj->_getModifiers();
//    cout << "Key " << ((newstate == MAC_KEYDOWN) ? "Down" : "Up") << endl;
//    cout << "preMask: 0x" << hex << prevModMask << endl;
//    cout << "ModMask: 0x" << hex << mods << endl;
//    cout << "Change:  0x" << hex << (change & prevModMask) << endl << endl;

//    cout << "Modifiers before: 0x" << hex << modsRef << endl;
    
    if(mods & NSShiftKeyMask)
    {
        modsRef |= AIS::Keyboard::Shift;
        [self injectEvent:KC_LSHIFT eventTime:time eventType:newstate];
    }
    else
    {
        modsRef &= ~AIS::Keyboard::Shift;
        [self injectEvent:KC_LSHIFT eventTime:time eventType:newstate];
    }

    if(mods & NSAlternateKeyMask)
    {
        modsRef |= AIS::Keyboard::Alt;
        [self injectEvent:KC_LMENU eventTime:time eventType:newstate];
    }
    else
    {
        modsRef &= ~AIS::Keyboard::Alt;
        [self injectEvent:KC_LMENU eventTime:time eventType:newstate];
    }

    if(mods & NSControlKeyMask)
    {
        modsRef |= AIS::Keyboard::Ctrl;
        [self injectEvent:KC_LCONTROL eventTime:time eventType:newstate];
    }
    else {
        modsRef &= ~AIS::Keyboard::Ctrl;
        [self injectEvent:KC_LCONTROL eventTime:time eventType:newstate];
    }

    if(mods & NSCommandKeyMask)
    {
        [self injectEvent:KC_LWIN eventTime:time eventType:newstate];
    }
    if(mods & NSFunctionKeyMask)
    {
        [self injectEvent:KC_APPS eventTime:time eventType:newstate];
    }
    if(mods & NSAlphaShiftKeyMask)
    {
        [self injectEvent:KC_CAPITAL eventTime:time eventType:newstate];
    }

    if([theEvent keyCode] == NSClearLineFunctionKey) // numlock
        [self injectEvent:KC_NUMLOCK eventTime:time eventType:newstate];

//    cout << "Modifiers after: 0x" << hex << modsRef << endl;
    oisKeyboardObj->_setModifiers(modsRef);
	prevModMask = mods;
}

- (VirtualtoAIS_KeyMap)keyConversionMap
{
    return keyConversion;
}

@end



#endif //! AIS_APPLE_PLATFORM