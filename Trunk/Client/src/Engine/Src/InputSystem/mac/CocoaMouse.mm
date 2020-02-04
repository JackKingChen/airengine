#include "stdafx.h"

#ifdef AIS_APPLE_PLATFORM

#include "mac/CocoaMouse.h"
#include "mac/CocoaInputManager.h"
#include "mac/CocoaHelpers.h"


using namespace AIS;

//-------------------------------------------------------------------//
CocoaMouse::CocoaMouse( InputManager* creator, bool buffered )
	: Mouse(creator->inputManagerName(), buffered, 0, creator)
{
    mResponder = nil;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    reCreateResponder();
    
	static_cast<CocoaInputManager*>(mCreator)->_setMouseUsed(true);

    [pool drain];
}

CocoaMouse::~CocoaMouse()
{
	// Restore Mouse
//	CGAssociateMouseAndMouseCursorPosition(true);
	CGDisplayShowCursor(kCGDirectMainDisplay);

    if (mResponder)
    {
        [mResponder release];
        mResponder = nil;
    }
    
	static_cast<CocoaInputManager*>(mCreator)->_setMouseUsed(false);
}

void CocoaMouse::reCreateResponder()
{
    CocoaInputManager *man = static_cast<CocoaInputManager*>(mCreator);
    if (mResponder)
    {
        [[man->_getWindow() contentView] willRemoveSubview:mResponder];
        [mResponder release];
        mResponder = nil;
    }
    mLastRC = [[man->_getWindow() contentView] frame];
    mResponder = [[CocoaMouseView alloc] initWithFrame:mLastRC];
    if(!mResponder)
        AIS_EXCEPT( E_General, "CocoaMouseView::CocoaMouseView >> Error creating event responder" );
    
    [[man->_getWindow() contentView] addSubview:mResponder];
    [mResponder setAISMouseObj:this];   
}

void CocoaMouse::_initialize()
{
	mState.clear();
	CGAssociateMouseAndMouseCursorPosition(false);
}

void CocoaMouse::setBuffered( bool buffered )
{
	mBuffered = buffered;
}

void CocoaMouse::capture()
{
    //--------------------------------------------------------------------------------------------
    // 如果窗口大小发生改变了，需要重新创建跟窗口一样大小的子视图来接受键盘，鼠标消息
    CocoaInputManager *man = static_cast<CocoaInputManager*>(mCreator);
    NSRect newRC = [[man->_getWindow() contentView] frame];
    if (newRC.size.width != mLastRC.size.width || newRC.size.height != mLastRC.size.height)
    {
        reCreateResponder();
        return;
    }
    //--------------------------------------------------------------------------------------------
    [mResponder capture];
}

void CocoaMouse::setMouseShow( bool bShow )
{
    mShowMouse = bShow;
    
    if (!mShowMouse)
        CGDisplayHideCursor(kCGDirectMainDisplay);
    else
        CGDisplayShowCursor(kCGDirectMainDisplay);
}

void CocoaMouse::getClientRect( int& x, int& y, int& width, int& height )
{
	x = mLastRC.origin.x;
	y = mLastRC.origin.y;
	width = mLastRC.size.width;
	height = mLastRC.size.height;
}
void CocoaMouse::setMousePos( int x, int y )
{
	[mResponder setMousePos:x :y];
}

@implementation CocoaMouseView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        mTrackingArea = nil;
        // Use NSTrackingArea to track mouse move events
        mTrackOptions = NSTrackingMouseMoved | NSTrackingActiveAlways;// | NSTrackingActiveInKeyWindow;
        mTrackUserData = nil;//[NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt:0], @"AISMouseTrackingKey", nil];
        
//        mTempState.clear();
        mMouseWarped = false;
        mNeedsToRegainFocus = false;
        
//        // Hide OS Mouse
//        CGDisplayHideCursor(kCGDirectMainDisplay);

        mLastRC = [self frame];

//        CGPoint warpPoint;
//		warpPoint.x = (((frame.origin.x + frame.size.width) - frame.origin.x) / 2) + frame.origin.x;
//		warpPoint.y = (((frame.origin.y + frame.size.height) - frame.origin.y) / 2) - frame.origin.y;
//        CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint);

        [self resetTrackArea:[self frame]];        
        [[self window] setAcceptsMouseMovedEvents:YES];
    }
    return self;
}

- (void)setMousePos:(int)x :(int)y
{
	CGPoint warpPoint;
	warpPoint.x = x + frame.origin.x;
	warpPoint.y = y - frame.origin.y;
	CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint);
}

- (void)dealloc
{
    [super dealloc];
    
    [mTrackingArea release];
    mTrackingArea = 0;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (void)setAISMouseObj:(CocoaMouse *)obj
{
    oisMouseObj = obj;    
    
    MouseState *state = oisMouseObj->getMouseStatePtr();
    state->width = mLastRC.size.width;
    state->height = mLastRC.size.height;
}

- (void)resetTrackArea:(NSRect)trackRect
{
    if (mTrackingArea)
    {
        [self removeTrackingArea:mTrackingArea];
        [mTrackingArea release];
        mTrackingArea = nil;
    }
    
    mTrackingArea = [[NSTrackingArea alloc]
                     initWithRect:trackRect// in our case track the entire view
                     options:mTrackOptions
                     owner:self
                     userInfo:mTrackUserData];
    
    [self addTrackingArea:mTrackingArea];
    
    mLastRC = trackRect;
}

- (void)capture
{
	MouseState *state = oisMouseObj->getMouseStatePtr();
    state->X.rel = 0;
    state->Y.rel = 0;
    state->Z.rel = 0;
    
//	if(mTempState.X.rel || mTempState.Y.rel || mTempState.Z.rel)
//	{
////		NSLog(@"%i %i %i", mTempState.X.rel, mTempState.Y.rel, mTempState.Z.rel);
//        
//		// Set new relative motion values
//		state->X.rel = mTempState.X.rel;
//		state->Y.rel = mTempState.Y.rel;
//		state->Z.rel = mTempState.Z.rel;
//		
//		// Update absolute position
//		state->X.abs += mTempState.X.rel;
//		state->Y.abs += mTempState.Y.rel;
//		
//		if(state->X.abs > state->width)
//			state->X.abs = state->width;
//		else if(state->X.abs < 0)
//			state->X.abs = 0;
//        
//		if(state->Y.abs > state->height)
//			state->Y.abs = state->height;
//		else if(state->Y.abs < 0)
//			state->Y.abs = 0;
//        
//		state->Z.abs += mTempState.Z.rel;
//		
//		//Fire off event
//        if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
//			oisMouseObj->getEventCallback()->mouseMoved(MouseEvent(oisMouseObj, *state));
//	}
//    
//	mTempState.clear();
}

#pragma mark Left Mouse Event overrides
- (void)mouseDown:(NSEvent *)theEvent
{
    int mouseButton = MB_Left;
    NSEventType type = [theEvent type];
    MouseState *state = oisMouseObj->getMouseStatePtr();

    if(mNeedsToRegainFocus)
        return;

    if((type == NSLeftMouseDown) && ([theEvent modifierFlags] & NSAlternateKeyMask))
    {
        mouseButton = MB_Middle;
    }
    else if((type == NSLeftMouseDown) && ([theEvent modifierFlags] & NSControlKeyMask))
    {
        mouseButton = MB_Right;
    }
    else if(type == NSLeftMouseDown)
    {
        mouseButton = MB_Left;
    }
    state->buttons |= 1 << mouseButton;
    
    bool isDBClick = oisMouseObj->isDBClickButton(mouseButton);
    
    if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
    {
        if (isDBClick)
            oisMouseObj->getEventCallback()->mouseDBClicked( MouseEvent(ET_MouseButtonDBClick, oisMouseObj, *state, (MouseButtonID)mouseButton ));
        else
            oisMouseObj->getEventCallback()->mousePressed( MouseEvent(ET_MouseButtonDown, oisMouseObj, *state, (MouseButtonID)mouseButton ));
    }
}

- (void)mouseUp:(NSEvent *)theEvent
{
    int mouseButton = MB_Left;
    NSEventType type = [theEvent type];
    MouseState *state = oisMouseObj->getMouseStatePtr();

    if((type == NSLeftMouseUp) && ([theEvent modifierFlags] & NSAlternateKeyMask))
    {
        mouseButton = MB_Middle;
    }
    else if((type == NSLeftMouseUp) && ([theEvent modifierFlags] & NSControlKeyMask))
    {
        mouseButton = MB_Right;
    }
    else if(type == NSLeftMouseUp)
    {
        mouseButton = MB_Left;
    }
    state->buttons &= ~(1 << mouseButton);

    if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
        oisMouseObj->getEventCallback()->mouseReleased( MouseEvent(ET_MouseButtonUp, oisMouseObj, *state, (MouseButtonID)mouseButton ));
}


- (void)mouseDragged:(NSEvent *)theEvent
{
//    CGPoint delta = CGPointMake([theEvent deltaX], [theEvent deltaY]);
    if(mNeedsToRegainFocus)
        return;
    
//    // Relative positioning
//    if(!mMouseWarped)
//    {
//        mTempState.X.rel += delta.x;
//        mTempState.Y.rel += delta.y;
//    }
    
    MouseState *state = oisMouseObj->getMouseStatePtr();
    NSPoint location = [[self window] mouseLocationOutsideOfEventStream];
    NSRect clipRect = [[[self window] contentView] frame];
    
    int newX = location.x - clipRect.origin.x;
    int newY = clipRect.size.height - location.y + clipRect.origin.y;
    state->X.rel = newX - state->X.abs;
    state->Y.rel = newY - state->Y.abs;
    // clip the mouse, absolute positioning
    state->X.abs = newX;
    state->Y.abs = newY;
    
    mMouseWarped = false;
    NSLog(@"Mouse Drag...\n");
}

#pragma mark Right Mouse Event overrides
- (void)rightMouseDown:(NSEvent *)theEvent
{
    int mouseButton = MB_Right;
    NSEventType type = [theEvent type];
    MouseState *state = oisMouseObj->getMouseStatePtr();
    
    if(mNeedsToRegainFocus)
        return;
    
    if(type == NSRightMouseDown)
    {	
        state->buttons |= 1 << mouseButton;
    }
    
    bool isDBClick = oisMouseObj->isDBClickButton(mouseButton);    
    if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
    {
        if (isDBClick)
            oisMouseObj->getEventCallback()->mouseDBClicked( MouseEvent(ET_MouseButtonDBClick, oisMouseObj, *state, (MouseButtonID)mouseButton ));
        else
            oisMouseObj->getEventCallback()->mousePressed( MouseEvent(ET_MouseButtonDown, oisMouseObj, *state, (MouseButtonID)mouseButton ));
    }
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    int mouseButton = MB_Right;
    NSEventType type = [theEvent type];
    MouseState *state = oisMouseObj->getMouseStatePtr();
    
    if(type == NSRightMouseUp)
    {	
        state->buttons &= ~(1 << mouseButton);
    }

    if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
        oisMouseObj->getEventCallback()->mouseReleased( MouseEvent(ET_MouseButtonUp, oisMouseObj, *state, (MouseButtonID)mouseButton ));
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
//    CGPoint delta = CGPointMake([theEvent deltaX], [theEvent deltaY]);
    if(mNeedsToRegainFocus)
        return;
    
//    // Relative positioning
//    if(!mMouseWarped)
//    {
//        mTempState.X.rel += delta.x;
//        mTempState.Y.rel += delta.y;
//    }
    
    MouseState *state = oisMouseObj->getMouseStatePtr();
    NSPoint location = [[self window] mouseLocationOutsideOfEventStream];
    NSRect clipRect = [[[self window] contentView] frame];
    
    int newX = location.x - clipRect.origin.x;
    int newY = clipRect.size.height - location.y + clipRect.origin.y;
    state->X.rel = newX - state->X.abs;
    state->Y.rel = newY - state->Y.abs;
    // clip the mouse, absolute positioning
    state->X.abs = newX;
    state->Y.abs = newY;
    
    mMouseWarped = false;
    NSLog(@"rightMouse Drag...\n");
}

#pragma mark Other Mouse Event overrides
- (void)otherMouseDown:(NSEvent *)theEvent
{
    int mouseButton = MB_Middle;
    NSEventType type = [theEvent type];
    MouseState *state = oisMouseObj->getMouseStatePtr();
    
    if(mNeedsToRegainFocus)
        return;
    
    if(type == NSOtherMouseDown)
    {
        state->buttons |= 1 << mouseButton;
    }

    bool isDBClick = oisMouseObj->isDBClickButton(mouseButton);
    if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
    {
        if (isDBClick)
            oisMouseObj->getEventCallback()->mouseDBClicked( MouseEvent(ET_MouseButtonDBClick, oisMouseObj, *state, (MouseButtonID)mouseButton ));
        else
            oisMouseObj->getEventCallback()->mousePressed( MouseEvent(ET_MouseButtonDown, oisMouseObj, *state, (MouseButtonID)mouseButton ));
    }
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
    int mouseButton = MB_Middle;
    NSEventType type = [theEvent type];
    MouseState *state = oisMouseObj->getMouseStatePtr();
    
    if(type == NSOtherMouseUp)
    {
        state->buttons &= ~(1 << mouseButton);
    }

    if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
        oisMouseObj->getEventCallback()->mouseReleased( MouseEvent(ET_MouseButtonUp, oisMouseObj, *state, (MouseButtonID)mouseButton ));
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
//    CGPoint delta = CGPointMake([theEvent deltaX], [theEvent deltaY]);
    if(mNeedsToRegainFocus)
        return;
    
//    // Relative positioning
//    if(!mMouseWarped)
//    {
//        mTempState.X.rel += delta.x;
//        mTempState.Y.rel += delta.y;
//    }
    
    MouseState *state = oisMouseObj->getMouseStatePtr();
    NSPoint location = [[self window] mouseLocationOutsideOfEventStream];
    NSRect clipRect = [[[self window] contentView] frame];
    
    int newX = location.x - clipRect.origin.x;
    int newY = clipRect.size.height - location.y + clipRect.origin.y;
    state->X.rel = newX - state->X.abs;
    state->Y.rel = newY - state->Y.abs;
    // clip the mouse, absolute positioning
    state->X.abs = newX;
    state->Y.abs = newY;
    
    mMouseWarped = false;
    NSLog(@"otherMouse Drag...\n");
}

- (void)scrollWheel:(NSEvent *)theEvent
{
//    if([theEvent deltaY] != 0.0)
//        mTempState.Z.rel += ([theEvent deltaY] * 60);
    
    MouseState *state = oisMouseObj->getMouseStatePtr();
    if([theEvent deltaY] != 0.0)
        state->Z.rel += ([theEvent deltaY] * 60);
    if(state->Z.rel != 0)
    {
        if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
            oisMouseObj->getEventCallback()->mouseMoved( MouseEvent(ET_MouseMove, oisMouseObj, *state, -1) );
    }
}

- (void)mouseMoved:(NSEvent *)theEvent
{
//    CGPoint delta = CGPointMake([theEvent deltaX], [theEvent deltaY]);
    if(mNeedsToRegainFocus)
        return;
    
//    // Relative positioning
//    if(!mMouseWarped)
//    {
//        mTempState.X.rel += delta.x;
//        mTempState.Y.rel += delta.y;
//    }
    
    MouseState *state = oisMouseObj->getMouseStatePtr();
    NSPoint location = [[self window] mouseLocationOutsideOfEventStream];
    NSRect clipRect = [[[self window] contentView] frame];
    
    int newX = location.x - clipRect.origin.x;
    int newY = clipRect.size.height - location.y + clipRect.origin.y;
    state->X.rel = newX - state->X.abs;
    state->Y.rel = newY - state->Y.abs;
    // clip the mouse, absolute positioning
    state->X.abs = newX;
    state->Y.abs = newY;
    
    mMouseWarped = false;
    
    if ( oisMouseObj->buffered() && oisMouseObj->getEventCallback() )
        oisMouseObj->getEventCallback()->mouseMoved( MouseEvent(ET_MouseMove, oisMouseObj, *state, -1) );
}

- (void)mouseEntered:(NSEvent *)theEvent
{
//	CGDisplayHideCursor(kCGDirectMainDisplay);
	CGAssociateMouseAndMouseCursorPosition(false);
    if(!mMouseWarped)
    {
        NSPoint pos = [[self window] mouseLocationOutsideOfEventStream];
        NSRect frame = [[[self window] contentView] frame];

        // Clear the previous mouse state
        MouseState *state = oisMouseObj->getMouseStatePtr();
        state->clear();

//        // Cocoa's coordinate system has the origin in the bottom left so we need to transform the height
//        mTempState.X.rel = pos.x;
//        mTempState.Y.rel = frame.size.height - pos.y;
        
        state->X.rel = 0;
        state->Y.rel = 0;
        state->X.abs = pos.x;
        state->Y.abs = frame.size.height - pos.y;
        
        NSLog(@"on mouse enter frame : %d, %d, %d, %d\n", state->width, state->height, state->X.abs, state->Y.abs);
    }
}

- (void)mouseExited:(NSEvent *)theEvent
{
	CGDisplayShowCursor(kCGDirectMainDisplay);
	CGAssociateMouseAndMouseCursorPosition(true);
    NSLog(@"on mouse exist frame\n");
}

@end


#endif //! AIS_APPLE_PLATFORM