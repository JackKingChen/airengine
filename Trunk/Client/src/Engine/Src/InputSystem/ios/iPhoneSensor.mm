
#include "stdafx.h"

#ifdef AIS_IPHONE_PLATFORM

#include "ios/iPhoneSensor.h"
#include "ios/iPhoneInputManager.h"

using namespace AIS;



//-------------------------------------------------------------------//
iPhoneSensor::iPhoneSensor( InputManager* creator, bool buffered )
	: Sensor(creator->inputManagerName(), buffered, 0, creator)
{
    iPhoneInputManager *man = static_cast<iPhoneInputManager*>(mCreator);
    
    man->_setAccelerometerUsed(true);
    [man->_getDelegate() setAccelerometerObject:this];
    [[UIAccelerometer sharedAccelerometer] setDelegate:man->_getDelegate()];
    mUpdateInterval = 60.0f;
}

iPhoneSensor::~iPhoneSensor()
{
    iPhoneInputManager *man = static_cast<iPhoneInputManager*>(mCreator);
    
    man->_setAccelerometerUsed(false);
    [man->_getDelegate() setAccelerometerObject:nil];
}

void iPhoneSensor::_initialize()
{
	mState.clear();

    // Set the update interval
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / mUpdateInterval)];
}

void iPhoneSensor::setBuffered( bool buffered )
{
	mBuffered = buffered;
}

void GetSensorOrientationRotateAngle( UIInterfaceOrientation orient, SensorOrientation& so, SensorRotate& sr )
{
    switch (orient)
    {
        // 荧屏直立
        case UIDeviceOrientationPortrait:            // Device oriented vertically, home button on the bottom
        {
            so = SO_PORTRAIT;
            sr = SR_0;
            break;
        }
        // 荧屏直立,上下颠倒
        case UIDeviceOrientationPortraitUpsideDown:  // Device oriented vertically, home button on the top
        {
            so = SO_PORTRAIT;
            sr = SR_180;
            break;
        }
        // 荧屏向左横置
        case UIDeviceOrientationLandscapeLeft:       // Device oriented horizontally, home button on the right
        {
            so = SO_LANDSCAPE;
            sr = SR_90;
            break;
        }
        // 荧屏向右横置
        case UIDeviceOrientationLandscapeRight:      // Device oriented horizontally, home button on the left
        {
            so = SO_LANDSCAPE;
            sr = SR_270;
            break;
        }
        // 荧屏向上平躺
        case UIDeviceOrientationFaceUp:              // Device oriented flat, face up
        {
            so = SO_SQUARE;
            sr = SR_0;
            break;
        }
        // 荧屏向下平躺
        case UIDeviceOrientationFaceDown:             // Device oriented flat, face down
        {
            so = SO_SQUARE;
            sr = SR_180;
            break;
        }
        // 荧屏朝向未知
        case UIDeviceOrientationUnknown:
        default:
        {
            so = SO_UNDEFINED;
            sr = SR_0;
            break;
        }
    }
}

//float GetLength( float x, float y, float z )
//{
//    return sqrtf( x*x + y*y + z*z );
//}
//
//float GetLengthSQ(float x, float y, float z)
//{
//    return ( x*x + y*y + z*z );
//}
//
//float DotProduct( AIS::Vector3& v1, AIS::Vector3& v2 )
//{
//    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
//}
//
//AIS::Vector3 CrossProduct( AIS::Vector3& v1, AIS::Vector3& v2 )
//{
//    return AIS::Vector3( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x );
//}
//
//float GetAngle(AIS::Vector3& v1, AIS::Vector3& v2)
//{
//    float lenProduct = GetLength(v1.x,v1.y,v1.z) * GetLength(v2.x,v2.y,v2.z);
//    
//    // Divide by zero check
//    if(lenProduct < 1e-6f)
//        lenProduct = 1e-6f;
//    
//    float f = DotProduct(v1, v2) / lenProduct;
//    if (f < -1.0f) f = -1.0f;
//    else if ( f > 1.0f) f = 1.0f;
//
//    return acos(f);
//}


void iPhoneSensor::didAccelerate(UIAcceleration *acceleration)
{
    UIInterfaceOrientation orient = [[UIApplication sharedApplication] statusBarOrientation];

    SensorOrientation so;
    SensorRotate sr;
    GetSensorOrientationRotateAngle( orient, so, sr );
    bool isOrientationChanged = ((so != mState.mOrientation) || (sr != mState.mRotateAngle));
    float cx = acceleration.x;
    float cy = acceleration.y;
    float cz = acceleration.z;
    
    float tmp = cx;
    switch (orient)
    {
        case UIInterfaceOrientationLandscapeRight:
            cx = -cy;
            cy = tmp;
            break;
            
        case UIInterfaceOrientationLandscapeLeft:
            cx = cy;
            cy = -tmp;
            break;
            
        case UIInterfaceOrientationPortraitUpsideDown:
            cx = -cy;
            cy = -tmp;
            break;
            
        case UIInterfaceOrientationPortrait:
            break;
    }
    bool isGravityChanged = ((cx != mState.mGravity.x) || (cy != mState.mGravity.y) || (cz != mState.mGravity.z));
    
    mState.mGravity.Set(cx, cy, cz);
    mState.mOrientation = so;
    mState.mRotateAngle = sr;
    
    if (mListener && mBuffered)
    {
        if (isGravityChanged)
        {
            mListener->gravityChanged( SensorEvent(ET_SensorGravityChanged, this, mState) );
        }
        if (isOrientationChanged)
        {
            mListener->orientationChanged( SensorEvent(ET_SensorOrientationChanged, this, mState) );
        }
    }
  
}

void iPhoneSensor::capture()
{

}


#endif //! AIS_IPHONE_PLATFORM