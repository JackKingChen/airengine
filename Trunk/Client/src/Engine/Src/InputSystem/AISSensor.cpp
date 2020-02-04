#include "stdafx.h"
#include "AISSensor.h"

using namespace AIS;

void SensorState::clear()
{
	mGravity.clear();
	mOrientation = SO_UNDEFINED;
	mRotateAngle = SR_0;
}

//----------------------------------------------------------------------------//
Sensor::Sensor(const std::string &vendor, bool buffered, int devID, InputManager* creator)
: DeviceObject(vendor, AISSensor, buffered, devID, creator)
, mListener(0)
{
}

//----------------------------------------------------------------------------//
void Sensor::setEventCallback( SensorListener *pListener )
{
	mListener = pListener;
}

//----------------------------------------------------------------------------//
SensorListener* Sensor::getEventCallback() const
{
	return mListener;
}
