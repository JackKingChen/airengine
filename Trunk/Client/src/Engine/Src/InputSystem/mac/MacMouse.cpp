#include "stdafx.h"

#ifdef AIS_APPLE_PLATFORM

#ifndef __LP64__

#include "mac/MacMouse.h"
#include "mac/MacInputManager.h"
#include "mac/MacHelpers.h"


#include <Carbon/Carbon.h>

#include <list>

#include <iostream>

using namespace AIS;

//Events we subscribe to and remove from queue
const EventTypeSpec mouseEvents[] =
{
	{ kEventClassMouse, kEventMouseDown },
	{ kEventClassMouse, kEventMouseUp },
	{ kEventClassMouse, kEventMouseMoved },
	{ kEventClassMouse, kEventMouseDragged },
	{ kEventClassMouse, kEventMouseWheelMoved }
};

const EventTypeSpec WinFocusAcquired [] = {{kEventClassApplication, kEventAppDeactivated}};

//-------------------------------------------------------------------//
MacMouse::MacMouse( InputManager* creator, bool buffered )
: Mouse(creator->inputManagerName(), buffered, 0, creator), mNeedsToRegainFocus( false )
{
    mouseEventRef = NULL;
	mWindowFocusHandler = NULL;
    
    // Get a "Univeral procedure pointer" for our callback
    mouseUPP = NewEventHandlerUPP(MouseWrapper);
	mWindowFocusListener = NewEventHandlerUPP(WindowFocusChanged);
    
	static_cast<MacInputManager*>(mCreator)->_setMouseUsed(true);
}

MacMouse::~MacMouse()
{
    if(mouseEventRef != NULL)
		RemoveEventHandler(mouseEventRef);
    
	if(mWindowFocusHandler != NULL)
		RemoveEventHandler(mWindowFocusHandler);
	
	DisposeEventHandlerUPP(mouseUPP);
	DisposeEventHandlerUPP(mWindowFocusListener);
	
	// Restore Mouse
	CGAssociateMouseAndMouseCursorPosition(TRUE);
	CGDisplayShowCursor(kCGDirectMainDisplay);
    
	static_cast<MacInputManager*>(mCreator)->_setMouseUsed(false);
}

void MacMouse::setMouseShow( bool bShow )
{
    mShowMouse = bShow;
    
    if (!mShowMouse)
        CGDisplayHideCursor(kCGDirectMainDisplay);
    else
        CGDisplayShowCursor(kCGDirectMainDisplay);
}

void MacMouse::_initialize()
{
	mState.clear();
	mMouseWarped = false;
	
//	// Hide OS Mouse
// 	CGDisplayHideCursor(kCGDirectMainDisplay);
    
	MacInputManager* im = static_cast<MacInputManager*>(mCreator);
	WindowRef win = im->_getWindow();
	
	if(win)
	{
		Rect clipRect = {0.0f, 0.0f, 0.0f, 0.0f};
		GetWindowBounds(win, kWindowContentRgn, &clipRect);
		
		CGPoint warpPoint;
		warpPoint.x = ((clipRect.right - clipRect.left) / 2) + clipRect.left;
		warpPoint.y = ((clipRect.bottom - clipRect.top) / 2) + clipRect.top;
		CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint); //Place at display origin
		
		mMouseWarped = true;
	}
    
	//Now that mouse is warped, start listening for events
	EventTargetRef event = ((MacInputManager*)mCreator)->_getEventTarget();
    
	if(mouseEventRef != NULL)
		RemoveEventHandler(mouseEventRef);
    
	if(mWindowFocusHandler != NULL)
		RemoveEventHandler(mWindowFocusHandler);
    
	mouseEventRef = mWindowFocusHandler = NULL;
    
	if(InstallEventHandler(event, mouseUPP, GetEventTypeCount(mouseEvents), mouseEvents, this, &mouseEventRef) != noErr)
		AIS_EXCEPT( E_General, "MacMouse::_initialize >> Error loading Mouse event handler" );
    
	if(InstallEventHandler(event, mWindowFocusListener, GetEventTypeCount(WinFocusAcquired), WinFocusAcquired, this, &mWindowFocusHandler) != noErr)
		AIS_EXCEPT( E_General, "MacMouse::_initialize >> Error loading Mouse event handler" );
    
	//Lock OS Mouse movement
	mNeedsToRegainFocus = false;
	CGAssociateMouseAndMouseCursorPosition(FALSE);
}

OSStatus MacMouse::WindowFocusChanged(EventHandlerCallRef nextHandler, EventRef event, void* macMouse)
{
    MacMouse* _this = static_cast<MacMouse*>(macMouse);
    if (_this)
	{
		_this->mNeedsToRegainFocus = true;
		CGAssociateMouseAndMouseCursorPosition(TRUE);
        
        // propagate the event down the chain
        return CallNextEventHandler(nextHandler, event);
    }
    else
        AIS_EXCEPT(E_General, "MouseWrapper >> Being called by something other than our event handler!");
}

void MacMouse::setBuffered( bool buffered )
{
	mBuffered = buffered;
}

void MacMouse::capture()
{
	mState.X.m_RelVal = 0;
	mState.Y.m_RelVal = 0;
	mState.Z.m_RelVal = 0;
}

void MacMouse::getClientRect( int& x, int& y, int& width, int& height )
{
	MacInputManager* im = static_cast<MacInputManager*>(mCreator);
	WindowRef win = im->_getWindow();
	Rect clipRect = {0.0f, 0.0f, 0.0f, 0.0f};

	if(win != NULL)
	{
		GetWindowBounds(win, kWindowContentRgn, &clipRect);
		width = clipRect.right - clipRect.left;
		height = clipRect.bottom - clipRect.top;
		x = clipRect.left;
		y = clipRect.top;
	}
}

void MacMouse::setMousePos( int x, int y )
{
	MacInputManager* im = static_cast<MacInputManager*>(mCreator);
	WindowRef win = im->_getWindow();
	Rect clipRect = {0.0f, 0.0f, 0.0f, 0.0f};
	GetWindowBounds(win, kWindowContentRgn, &clipRect);
	CGPoint warpPoint;
	warpPoint.x = x + clipRect.left;
	warpPoint.y = y + clipRect.top;
	CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint);
}

void MacMouse::_mouseCallback( EventRef theEvent )
{
    UInt32 kind = GetEventKind (theEvent);
    
	switch(kind)
	{
		case kEventMouseDragged:
		case kEventMouseMoved:
		{
			HIPoint location = {0.0f, 0.0f};
			HIPoint delta = {0.0f, 0.0f};
			Rect clipRect = {0.0f, 0.0f, 0.0f, 0.0f};
			
			if(mNeedsToRegainFocus)
				break;
            
			// Capture the parameters
			// TODO: Look into HIViewNewTrackingArea
			GetEventParameter(theEvent, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &location);
			GetEventParameter(theEvent, kEventParamMouseDelta, typeHIPoint, NULL, sizeof(HIPoint), NULL, &delta);
			
			// Mouse X and Y are the position on the screen,
			// startng from top-left at 0,0 caps at full monitor resolution
			
			// If we have a window we need to return adjusted coordinates
			// If not, just use raw coordinates - only do this if showing OS mouse
			MacInputManager* im = static_cast<MacInputManager*>(mCreator);
			WindowRef win = im->_getWindow();
			
			if(win != NULL)
			{
				GetWindowBounds(win, kWindowContentRgn, &clipRect);
                
                //------------------------------------------------
                //set size
                mState.width = clipRect.right - clipRect.left;
                mState.height = clipRect.bottom - clipRect.top;
                //------------------------------------------------
			}
            else
            {
                clipRect.right = mState.width;
                clipRect.bottom = mState.height;
            }
            
//            // clip the mouse, absolute positioning
//            if (location.x <= clipRect.left)
//				mState.X.abs = 0;
//			else if(location.x >= clipRect.right)
//				mState.X.abs = clipRect.right - clipRect.left;
//			else
//				mState.X.abs = location.x - clipRect.left;
//
//			if (location.y <= clipRect.top)
//				mState.Y.abs = 0;
//			else if(location.y >= clipRect.bottom)
//				mState.Y.abs = clipRect.bottom - clipRect.top;
//			else
//				mState.Y.abs = location.y - clipRect.top;
            
            
            int newX = location.x - clipRect.left;
            int newY = location.y - clipRect.top;
            mState.X.m_RelVal = newX - mState.X.m_AbsVal;
            mState.Y.m_RelVal = newY - mState.Y.m_AbsVal;
            // clip the mouse, absolute positioning
			mState.X.m_AbsVal = newX;
			mState.Y.m_AbsVal = newY;
            
			
//			// relative positioning
//			if(!mMouseWarped)
//			{
//				mTempState.X.rel += delta.x;
//				mTempState.Y.rel += delta.y;
//			}
			
			mMouseWarped = false;
            
            //------------------------------------------------
            // trigger mouse move
            if( mListener && mBuffered )
                mListener->mouseMoved( MouseEvent( ET_MouseMove, this, mState, -1 ) );
            //------------------------------------------------
            
			break;
		}
		case kEventMouseDown:
		{
			EventMouseButton button = 0;
			int mouseButton = 3;
			UInt32 modifiers = 0;
			
			if(mNeedsToRegainFocus)
				break;
            
			// Capture parameters
			GetEventParameter(theEvent, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &button);
			GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
            std::cout<<"mouse button : "<<button<<"\n";
			
			if((button == kEventMouseButtonTertiary) || ((button == kEventMouseButtonPrimary) && (modifiers & optionKey)))
			{
				mouseButton = 2;
				mState.buttons |= 1 << mouseButton;
			}
            else if((button == kEventMouseButtonSecondary) || ((button == kEventMouseButtonPrimary) && (modifiers & controlKey)))
            {
                mouseButton = 1;
                mState.buttons |= 1 << mouseButton;
            }
            else if(button == kEventMouseButtonPrimary)
            {
                mouseButton = 0;
                mState.buttons |= 1 << mouseButton;
            }
            
            //------------------------------------------------
            // trigger mouse pressed
            bool isDBClick = isDBClickButton(mouseButton);
            if( mListener && mBuffered )
            {
                if (isDBClick)
                    mListener->mouseDBClicked( MouseEvent( ET_MouseButtonDBClick, this, mState, (MouseButtonID)mouseButton ) );
                else
                    mListener->mousePressed( MouseEvent( ET_MouseButtonDown, this, mState, (MouseButtonID)mouseButton ) );
            }
            //------------------------------------------------
            
            break;
		}
		case kEventMouseUp:
		{
			EventMouseButton button = 0;
			int mouseButton = 3;
			UInt32 modifiers = 0;
			
			if(mNeedsToRegainFocus)
			{
				mNeedsToRegainFocus = false;
				CGAssociateMouseAndMouseCursorPosition(false);
				
				MacInputManager* im = static_cast<MacInputManager*>(mCreator);
				WindowRef win = im->_getWindow();
				
				if(win)
				{
//					Rect clipRect = {0.0f, 0.0f, 0.0f, 0.0f};
//					GetWindowBounds(win, kWindowContentRgn, &clipRect);
//					
//					CGPoint warpPoint;
//					warpPoint.x = ((clipRect.right - clipRect.left) / 2) + clipRect.left;
//					warpPoint.y = ((clipRect.bottom - clipRect.top) / 2) + clipRect.top;
//					CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, warpPoint); //Place at display origin
					
//					CGDisplayHideCursor(kCGDirectMainDisplay);
					
					mMouseWarped = true;
				}
				
				//Once we regain focus, we do not really know what state all the buttons are in - for now, set to not pressed. todo, check current status
				//compare against old status, and send off any needed events
				mState.buttons = 0;
				
				break;
			}
			
			// Capture parameters
			GetEventParameter(theEvent, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &button);
			GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modifiers);
			
			if ((button == kEventMouseButtonTertiary) || ((button == kEventMouseButtonPrimary) && (modifiers & optionKey)))
			{
				mouseButton = 2;
				mState.buttons &= ~(1 << mouseButton);
			}
            else if ((button == kEventMouseButtonSecondary) || ((button == kEventMouseButtonPrimary) && (modifiers & controlKey)))
            {
                mouseButton = 1;
                mState.buttons &= ~(1 << mouseButton);
            }
            else if (button == kEventMouseButtonPrimary)
            {
                mouseButton = 0;
                mState.buttons &= ~(1 << mouseButton);
            }
            
            //------------------------------------------------
            // trigger mouse up
            if( mListener && mBuffered )
                mListener->mouseReleased( MouseEvent( ET_MouseButtonUp, this, mState, (MouseButtonID)mouseButton ) );
            //------------------------------------------------
            
            break;
		}
		case kEventMouseWheelMoved:
		{
			SInt32 wheelDelta = 0;
			EventMouseWheelAxis	wheelAxis = 0;
            
			// Capture parameters
			GetEventParameter(theEvent, kEventParamMouseWheelAxis, typeMouseWheelAxis, NULL, sizeof(EventMouseWheelAxis), NULL, &wheelAxis);
			GetEventParameter(theEvent, kEventParamMouseWheelDelta, typeSInt32, NULL, sizeof(SInt32), NULL, &wheelDelta);
			
			// If the Y axis of the wheel changed, then update the Z
			// Does AIS care about the X wheel axis?
            //			if(wheelAxis == kEventMouseWheelAxisY)
            //				mTempState.Z.rel += (wheelDelta * 60);
            if(wheelAxis == kEventMouseWheelAxisY)
				mState.Z.m_RelVal += (wheelDelta * 60);
            
            //------------------------------------------------
            // trigger mouse moved
            if (mState.Z.m_RelVal != 0)
            {
                if( mListener && mBuffered )
                    mListener->mouseMoved( MouseEvent( ET_MouseMove, this, mState, -1 ) );
            }
            //------------------------------------------------
            break;
		}
		default:
			break;
	}    
}
#endif


#endif //! AIS_APPLE_PLATFORM