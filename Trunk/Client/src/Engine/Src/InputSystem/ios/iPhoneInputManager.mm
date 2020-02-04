 
#include "stdafx.h"

#ifdef AIS_IPHONE_PLATFORM

#include "ios/iPhoneInputManager.h"
#include "ios/iPhoneSensor.h"
#include "ios/iPhoneMultiTouch.h"


using namespace std;
using namespace AIS;


//--------------------------------------------------------------------------------//

@implementation InputDelegate

@synthesize touchObject;
@synthesize accelerometerObject;

- (void) handleTouchEventBegan:(NSSet *)touches :(UIEvent *)event :(CGRect)rc
{
    int ids[MAX_TOUCHES] = {0};
    float xs[MAX_TOUCHES] = {0.0f};
    float ys[MAX_TOUCHES] = {0.0f};
    
    CGFloat contentScale = 1.0;
#if __IPHONE_4_0
    if([self respondsToSelector:@selector(contentScaleFactor)])
        contentScale = [[UIScreen mainScreen] scale];
#endif
    
    int i = 0;
    for (UITouch *touch in touches)
    {
        ids[i] = (int)(long)touch;
        xs[i] = [touch locationInView: [touch view]].x * contentScale;
        ys[i] = [touch locationInView: [touch view]].y * contentScale;
        ++i;
    }
    
    touchObject->SetViewSize( rc.size.width, rc.size.height );
    touchObject->handleTouchesBegan( i, ids, xs, ys );
}
- (void) handleTouchEventEnded:(NSSet *)touches :(UIEvent *)event :(CGRect)rc
{
    int ids[MAX_TOUCHES] = {0};
    float xs[MAX_TOUCHES] = {0.0f};
    float ys[MAX_TOUCHES] = {0.0f};
    
    CGFloat contentScale = 1.0;
#if __IPHONE_4_0
    if([self respondsToSelector:@selector(contentScaleFactor)])
        contentScale = [[UIScreen mainScreen] scale];
#endif
    
    int i = 0;
    for (UITouch *touch in touches)
    {
        ids[i] = (int)(long)touch;
        xs[i] = [touch locationInView: [touch view]].x * contentScale;
        ys[i] = [touch locationInView: [touch view]].y * contentScale;
        ++i;
    }
    
    touchObject->SetViewSize( rc.size.width, rc.size.height );
    touchObject->handleTouchesEnded( i, ids, xs, ys );
}
- (void) handleTouchEventMoved:(NSSet *)touches :(UIEvent *)event :(CGRect)rc
{
    int ids[MAX_TOUCHES] = {0};
    float xs[MAX_TOUCHES] = {0.0f};
    float ys[MAX_TOUCHES] = {0.0f};
    
    CGFloat contentScale = 1.0;
#if __IPHONE_4_0
    if([self respondsToSelector:@selector(contentScaleFactor)])
        contentScale = [[UIScreen mainScreen] scale];
#endif
    
    int i = 0;
    for (UITouch *touch in touches)
    {
        ids[i] = (int)(long)touch;
        xs[i] = [touch locationInView: [touch view]].x * contentScale;
        ys[i] = [touch locationInView: [touch view]].y * contentScale;
        ++i;
    }
    
    touchObject->SetViewSize( rc.size.width, rc.size.height );
    touchObject->handleTouchesMoved( i, ids, xs, ys );
}
- (void) handleTouchEventCancelled:(NSSet *)touches :(UIEvent *)event :(CGRect)rc
{
    int ids[MAX_TOUCHES] = {0};
    float xs[MAX_TOUCHES] = {0.0f};
    float ys[MAX_TOUCHES] = {0.0f};
    
    CGFloat contentScale = 1.0;
#if __IPHONE_4_0
    if([self respondsToSelector:@selector(contentScaleFactor)])
        contentScale = [[UIScreen mainScreen] scale];
#endif
    
    int i = 0;
    for (UITouch *touch in touches)
    {
        ids[i] = (int)(long)touch;
        xs[i] = [touch locationInView: [touch view]].x * contentScale;
        ys[i] = [touch locationInView: [touch view]].y * contentScale;
        ++i;
    }
    
    touchObject->SetViewSize( rc.size.width, rc.size.height );
    touchObject->handleTouchesCancelled( i, ids, xs, ys );
}

- (id)init
{
    if((self = [super init]))
    {
        touchObject = nil;
        accelerometerObject = nil;
    }
    return self;
}

- (void)dealloc
{
    delete touchObject; touchObject = NULL;
    delete accelerometerObject; accelerometerObject = NULL;
    [super dealloc];
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}


#pragma mark Accelerator Event Handling
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
    accelerometerObject->didAccelerate(acceleration);
}

#pragma mark Touch Event Handling
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self handleTouchEventEnded:touches :event :[self frame]];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self handleTouchEventMoved:touches :event :[self frame]];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self handleTouchEventCancelled:touches :event :[self frame]];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self handleTouchEventBegan:touches :event :[self frame]];
}

@end

//--------------------------------------------------------------------------------//
iPhoneInputManager::iPhoneInputManager() : InputManager("iPhone Input Manager"),
                                            mIsContentScalingSupported(false),
                                            mContentScalingFactor(1.0)
{
    mHideMouse = true;
	bAccelerometerUsed = bMultiTouchUsed = false;

    // Check for content scaling.  iOS 4 or later
    if([[[UIDevice currentDevice] systemVersion] floatValue] >= 4.0)
        mIsContentScalingSupported = true;
    
	// Setup our internal factories
	mFactories.push_back(this);
}

//--------------------------------------------------------------------------------//
iPhoneInputManager::~iPhoneInputManager()
{
    [mDelegate release]; mDelegate = nil;
    [mWindow release]; mWindow = nil;
}

//--------------------------------------------------------------------------------//
void iPhoneInputManager::_initialize( ParamList &paramList )
{
	_parseConfigSettings( paramList );
    
    CGSize screenSize;
    screenSize = [[UIScreen mainScreen] applicationFrame].size;
    mDelegate = [[InputDelegate alloc] initWithFrame:CGRectMake(0, 0, screenSize.width, screenSize.height)];
    [mDelegate init];

    // Set flags that we want to accept multiple finger touches and be the only one to receive touch events
    [mDelegate setMultipleTouchEnabled:YES];
    [mDelegate setExclusiveTouch:YES];
    [mDelegate becomeFirstResponder];

    [mWindow addSubview:mDelegate];
}

//--------------------------------------------------------------------------------//
void iPhoneInputManager::_parseConfigSettings( ParamList &paramList )
{
    // Some carbon apps are running in a window, however full screen apps
	// do not have a window, so we need to account for that too.
	ParamList::iterator i = paramList.find("WINDOW");
	if(i != paramList.end())
	{
		mWindow = (UIWindow *)strtoul(i->second.c_str(), 0, 10);
		if(mWindow == 0)
			mWindow = nil;
    }
	else
	{
		// else get the main active window.. user might not have access to it through some
		// graphics libraries, if that fails then try at the application level.
        mWindow = [[UIApplication sharedApplication] keyWindow];
	}
	
	if(mWindow == nil)
		AIS_EXCEPT( E_General, "iPhoneInputManager::_parseConfigSettings >> Unable to find a window or event target" );
}

//--------------------------------------------------------------------------------//
DeviceList iPhoneInputManager::freeDeviceList()
{
	DeviceList ret;

	if( bAccelerometerUsed == false )
		ret.insert(std::make_pair(AISSensor, mInputSystemName));

	if( bMultiTouchUsed == false )
		ret.insert(std::make_pair(AISMultiTouch, mInputSystemName));

	return ret;
}

//--------------------------------------------------------------------------------//
int iPhoneInputManager::totalDevices(AISDeviceType iType)
{
	switch(iType)
	{
        case AISSensor: return 1;
        case AISMultiTouch: return 1;
        default: return 0;
	}
}

//--------------------------------------------------------------------------------//
int iPhoneInputManager::freeDevices(AISDeviceType iType)
{
	switch(iType)
	{
        case AISSensor: return bAccelerometerUsed ? 0 : 1;
        case AISMultiTouch: return bMultiTouchUsed ? 0 : 1;
        default: return 0;
	}
}

//--------------------------------------------------------------------------------//
bool iPhoneInputManager::vendorExist(AISDeviceType iType, const std::string & vendor)
{
	if( ( iType == AISMultiTouch || iType == AISSensor ) && vendor == mInputSystemName )
		return true;

	return false;
}

//--------------------------------------------------------------------------------//
DeviceObject* iPhoneInputManager::createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, 
									  const std::string & vendor)
{
	DeviceObject *obj = 0;

	switch(iType)
	{
        case AISSensor:
        {
            if( bAccelerometerUsed == false )
                obj = new iPhoneSensor(this, bufferMode);
            break;
        }
        case AISMultiTouch:
        {
            if( bMultiTouchUsed == false )
                obj = new iPhoneMultiTouch(this, bufferMode);
            break;
        }
        default:
            break;
	}

	if( obj == 0 )
		AIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");

	return obj;
}

//--------------------------------------------------------------------------------//
void iPhoneInputManager::destroyObject(DeviceObject* obj)
{
	delete obj;
}



#endif //! AIS_IPHONE_PLATFORM