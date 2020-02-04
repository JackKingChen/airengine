
#include "stdafx.h"

#ifdef AIS_IPHONE_PLATFORM

#include "ios/iPhoneMultiTouch.h"
#include "ios/iPhoneInputManager.h"

using namespace AIS;



//-------------------------------------------------------------------//
iPhoneMultiTouch::iPhoneMultiTouch( InputManager* creator, bool buffered )
	: MultiTouch(creator->inputManagerName(), buffered, 0, creator)
{
	iPhoneInputManager *man = static_cast<iPhoneInputManager*>(mCreator);

    man->_setMultiTouchUsed(true);
    [man->_getDelegate() setTouchObject:this];
}

iPhoneMultiTouch::~iPhoneMultiTouch()
{
	iPhoneInputManager *man = static_cast<iPhoneInputManager*>(mCreator);
    
    man->_setMultiTouchUsed(false);
    [man->_getDelegate() setTouchObject:nil];
}

void iPhoneMultiTouch::_initialize()
{
//    mTempState.clear();
}

void iPhoneMultiTouch::setBuffered( bool buffered )
{
	mBuffered = buffered;
}

void iPhoneMultiTouch::capture()
{

}


#endif //! AIS_IPHONE_PLATFORM