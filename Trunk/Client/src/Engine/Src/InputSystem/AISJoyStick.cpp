#include "stdafx.h"
#include "AISJoyStick.h"

using namespace AIS;


void JoyStickState::clear()
{
	for( std::vector<bool>::iterator i = mButtons.begin(), e = mButtons.end(); i != e; ++i )
	{
		(*i) = false;
	}

	for( std::vector<Axis>::iterator i = mAxes.begin(), e = mAxes.end(); i != e; ++i )
	{
		i->m_AbsOnly = true; //Currently, joysticks only report Absolute values
		i->clear();
	}

	for( std::vector<Vector3>::iterator i = mVectors.begin(), e = mVectors.end(); i != e; ++i )
	{
		i->clear();
	}

	for( int i = 0; i < 4; ++i )
	{
		mPOV[i].direction = Pov::Centered;
		mSliders[i].abX = mSliders[i].abY = 0;
	}
}

//----------------------------------------------------------------------------//
JoyStick::JoyStick(const std::string &vendor, bool buffered, int devID, InputManager* creator) :
	DeviceObject(vendor, AISJoyStick, buffered, devID, creator),
	mSliders(0),
	mPOVs(0),
	mListener(0),
	mVector3Sensitivity(AIS_JOYSTICK_VECTOR3_DEFAULT)
{
}

//----------------------------------------------------------------------------//
int JoyStick::getNumberOfComponents(ComponentType cType) const
{
	switch( cType )
	{
	case AIS_Button:	return (int)mState.mButtons.size();
	case AIS_Axis:		return (int)mState.mAxes.size();
	case AIS_Slider:	return mSliders;
	case AIS_POV:		return mPOVs;
	case AIS_Vector3:	return (int)mState.mVectors.size();
	default:			return 0;
	}
}

//----------------------------------------------------------------------------//
void JoyStick::setVector3Sensitivity(float degrees)
{
	mVector3Sensitivity = degrees;
}

//----------------------------------------------------------------------------//
float JoyStick::getVector3Sensitivity() const
{
	return mVector3Sensitivity;
}

//----------------------------------------------------------------------------//
void JoyStick::setEventCallback( JoyStickListener *joyListener )
{
	mListener = joyListener;
}

//----------------------------------------------------------------------------//
JoyStickListener* JoyStick::getEventCallback() const
{
	return mListener;
}
