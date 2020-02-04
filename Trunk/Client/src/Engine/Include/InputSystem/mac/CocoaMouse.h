
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISMouse.h"
#include "mac/CocoaHelpers.h"

#include <Cocoa/Cocoa.h>

@class CocoaMouseView;

using namespace AIS;

namespace AIS
{
	class CocoaMouse : public Mouse
    {
	public:
		CocoaMouse( InputManager* creator, bool buffered );
		virtual ~CocoaMouse();
		
		virtual void setBuffered(bool buffered);

		virtual void capture();

		virtual Interface* queryInterface(Interface::IType type) {return 0;}

		virtual void _initialize();
        
        MouseState * getMouseStatePtr() { return &(mState); }
        
        virtual void setMouseShow( bool bShow );

		virtual void getClientRect( int& x, int& y, int& width, int& height );
		virtual void setMousePos( int x, int y );

	protected:
        void reCreateResponder();
        
        CocoaMouseView	*mResponder;
        NSRect			mLastRC;
	};
}

@interface CocoaMouseView : NSView
{
    NSTrackingArea          *mTrackingArea;
    CocoaMouse              *oisMouseObj;
    bool                    mNeedsToRegainFocus;
    bool                    mMouseWarped;
    NSRect                  mLastRC;
    NSTrackingAreaOptions   mTrackOptions;
    NSDictionary            *mTrackUserData;
}

- (void)setAISMouseObj:(CocoaMouse *)obj;
- (void)capture;
- (void)resetTrackArea:(NSRect)trackRect;
- (void)setMousePos:(int)x :(int)y;

@end


#endif //!AIS_APPLE_PLATFORM
