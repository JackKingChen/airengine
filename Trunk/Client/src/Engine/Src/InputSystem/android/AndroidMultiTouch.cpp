#include "stdafx.h"
#include "android/AndroidMultiTouch.h"
#include "android/AndroidInputManager.h"

using namespace AIS;


AndroidMultiTouch::AndroidMultiTouch( InputManager* creator, bool buffered )
: MultiTouch(creator->inputManagerName(), buffered, 0, creator )
, mContentScale(1.0f)
{
	AndroidInputManager *man = static_cast<AndroidInputManager*>(creator);
	man->_setMultiTouchUsed( true );
	man->_getDelegate()->mTouchObject = this;
}

AndroidMultiTouch::~AndroidMultiTouch()
{
	AndroidInputManager *man = static_cast<AndroidInputManager*>(mCreator);

	man->_setMultiTouchUsed(false);
	man->_getDelegate()->mTouchObject = NULL;

}

/** @copydoc Object::setBuffered */
void AndroidMultiTouch::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

/** @copydoc Object::capture */
void AndroidMultiTouch::capture()
{

}

/** @copydoc Object::_initialize */
void AndroidMultiTouch::_initialize()
{

}
