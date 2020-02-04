#include "stdafx.h"
#include "win32/Win32Mouse.h"
#include "win32/Win32InputManager.h"

#ifdef AIS_WIN32_PLATFORM

using namespace AIS;

//--------------------------------------------------------------------------------------------------//
Win32Mouse::Win32Mouse( InputManager* creator, IDirectInput8* pDI, bool buffered, DWORD coopSettings )
	: Mouse(creator->inputManagerName(), buffered, 0, creator)
{
	mMouse = 0;
	mDirectInput = pDI;
	mCoopSetting = coopSettings;

	static_cast<Win32InputManager*>(mCreator)->_setMouseUsed(true);
}

//--------------------------------------------------------------------------------------------------//
void Win32Mouse::_initialize()
{
	DIPROPDWORD dipdw;

	//Clear old state
	mState.clear();

    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = MOUSE_DX_BUFFERSIZE;
	
	if( FAILED(mDirectInput->CreateDevice(GUID_SysMouse, &mMouse, NULL)) )
		AIS_EXCEPT( E_General, "Win32Mouse::Win32Mouse >> Failed to create device" );

	if( FAILED(mMouse->SetDataFormat(&c_dfDIMouse2)) )
		AIS_EXCEPT( E_General, "Win32Mouse::Win32Mouse >> Failed to set format" );
	
	mHwnd = ((Win32InputManager*)mCreator)->getWindowHandle();

	if( FAILED(mMouse->SetCooperativeLevel(mHwnd, mCoopSetting)) )
		AIS_EXCEPT( E_General, "Win32Mouse::Win32Mouse >> Failed to set coop level" );
	
	if( FAILED(mMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph )) )
		AIS_EXCEPT( E_General, "Win32Mouse::Win32Mouse >> Failed to set property" );

	HRESULT hr = mMouse->Acquire();
	if (FAILED(hr) && hr != DIERR_OTHERAPPHASPRIO)
		AIS_EXCEPT( E_General, "Win32Mouse::Win32Mouse >> Failed to aquire mouse!" );
}

//--------------------------------------------------------------------------------------------------//
Win32Mouse::~Win32Mouse()
{
	if (mMouse)
	{
		mMouse->Unacquire();
		mMouse->Release();
		mMouse = 0;
	}

	static_cast<Win32InputManager*>(mCreator)->_setMouseUsed(false);
}

//--------------------------------------------------------------------------------------------------//

void Win32Mouse::capture()
{
	//////////////////////////////////////////////////////////////////////////
	RECT rc;
	GetClientRect( mHwnd, &rc );
	mState.width = rc.right - rc.left;
	mState.height = rc.bottom - rc.top;
	//////////////////////////////////////////////////////////////////////////

	//Clear old relative values
	mState.X.m_RelVal = mState.Y.m_RelVal = mState.Z.m_RelVal = 0;

	DIDEVICEOBJECTDATA diBuff[MOUSE_DX_BUFFERSIZE];
	DWORD entries = MOUSE_DX_BUFFERSIZE;

	HRESULT hr = mMouse->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), diBuff, &entries, 0 );
	if( hr != DI_OK )
	{
		hr = mMouse->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = mMouse->Acquire();

		hr = mMouse->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), diBuff, &entries, 0 );
		
		//Perhaps the user just tabbed away, and coop settings
		//are nonexclusive..so just ignore
		if( FAILED(hr) )
			return;
	}

	bool axesMoved = false;
	//Accumulate all axis movements for one axesMove message..
	//Buttons are fired off as they are found
	for(unsigned int i = 0; i < entries; ++i )
	{
		switch( diBuff[i].dwOfs )
		{
			case DIMOFS_BUTTON0:
				if(!_doMouseClick(0, diBuff[i])) return;
				break;
			case DIMOFS_BUTTON1:
				if(!_doMouseClick(1, diBuff[i])) return;
				break;
			case DIMOFS_BUTTON2:
				if(!_doMouseClick(2, diBuff[i])) return;
				break;
			case DIMOFS_BUTTON3:
				if(!_doMouseClick(3, diBuff[i])) return;
				break;
			case DIMOFS_BUTTON4:
				if(!_doMouseClick(4, diBuff[i])) return;
				break;	
			case DIMOFS_BUTTON5:
				if(!_doMouseClick(5, diBuff[i])) return;
				break;
			case DIMOFS_BUTTON6:
				if(!_doMouseClick(6, diBuff[i])) return;
				break;
			case DIMOFS_BUTTON7:
				if(!_doMouseClick(7, diBuff[i])) return;
				break;
			case DIMOFS_X:
				mState.X.m_RelVal += diBuff[i].dwData;
				axesMoved = true;
				break;
			case DIMOFS_Y:
				mState.Y.m_RelVal += diBuff[i].dwData;
				axesMoved = true;
				break;
			case DIMOFS_Z:
				mState.Z.m_RelVal += diBuff[i].dwData;
				axesMoved = true;
				break;
			default: break;
		} //end switch
	}//end for

	if( axesMoved )
	{
		if( mCoopSetting & DISCL_NONEXCLUSIVE )
		{
			//DirectInput provides us with meaningless values, so correct that
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(mHwnd, &point);
			mState.X.m_AbsVal = point.x;
			mState.Y.m_AbsVal = point.y;
		}
		else
		{
			mState.X.m_AbsVal +=  mState.X.m_RelVal;
			mState.Y.m_AbsVal +=  mState.Y.m_RelVal;
		}
		mState.Z.m_AbsVal +=  mState.Z.m_RelVal;

		////Clip values to window
		//if( mState.X.abs < 0 )
		//	mState.X.abs = 0;
		//else if( mState.X.abs > mState.width )
		//	mState.X.abs = mState.width;
		//if( mState.Y.abs < 0 )
		//	mState.Y.abs = 0;
		//else if( mState.Y.abs > mState.height )
		//	mState.Y.abs = mState.height;

		//Do the move
		if( mListener && mBuffered )
			mListener->mouseMoved( MouseEvent( ET_MouseMove, this, mState, -1 ) );
	}
}

//--------------------------------------------------------------------------------------------------//
bool Win32Mouse::_doMouseClick( int mouseButton, DIDEVICEOBJECTDATA& di )
{
	//if( di.dwData & 0x80 )
	//{
	//	mState.buttons |= 1 << mouseButton; //turn the bit flag on
	//	if( mListener && mBuffered )
	//		return mListener->mousePressed( MouseEvent( this, mState ), (MouseButtonID)mouseButton );
	//}
	//else
	//{
	//	mState.buttons &= ~(1 << mouseButton); //turn the bit flag off
	//	if( mListener && mBuffered )
	//		return mListener->mouseReleased( MouseEvent( this, mState ), (MouseButtonID)mouseButton );
	//}

	if( di.dwData & 0x80 )
	{
		mState.buttons |= 1 << mouseButton; //turn the bit flag on

		bool bIsDBClick = mDBClickSimulater.IsDBClickWhenPress(mouseButton);

		if( mListener && mBuffered )
		{
			if ( bIsDBClick )
				return mListener->mouseDBClicked( MouseEvent( ET_MouseButtonDBClick, this, mState, (MouseButtonID)mouseButton ) );
			else
				return mListener->mousePressed( MouseEvent( ET_MouseButtonDown, this, mState, (MouseButtonID)mouseButton ) );
		}
	}
	else
	{
		mState.buttons &= ~(1 << mouseButton); //turn the bit flag off
		if( mListener && mBuffered )
			return mListener->mouseReleased( MouseEvent( ET_MouseButtonUp, this, mState, (MouseButtonID)mouseButton ) );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------//
void Win32Mouse::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

void Win32Mouse::setMouseShow( bool bShow )
{
	Mouse::setMouseShow(bShow);
	ShowCursor( (BOOL)bShow );	
}

void Win32Mouse::getClientRect( int&x, int&y, int& width, int& height )
{
	RECT rc;
	GetClientRect( mHwnd, &rc );
	x = rc.left;
	y = rc.right;
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
}

void Win32Mouse::setMousePos( int x, int y )
{
	SetCursorPos( x, y );
}


#endif //! AIS_WIN32_PLATFORM