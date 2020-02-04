#include "stdafx.h"

#ifdef AIS_APPLE_PLATFORM

#include "mac/MacJoyStick.h"
#include "mac/MacHIDManager.h"
#include "mac/MacInputManager.h"


#include <cassert>

using namespace AIS;

//--------------------------------------------------------------------------------------------------//
MacJoyStick::MacJoyStick(const std::string &vendor, bool buffered, HidInfo* info, InputManager* creator, int devID) : 
JoyStick(vendor, buffered, devID, creator), mInfo(info)
{
	
}

//--------------------------------------------------------------------------------------------------//
MacJoyStick::~MacJoyStick()
{
	//TODO: check if the queue has been started first?
	//(*mQueue)->stop(mQueue); 
	(*mQueue)->dispose(mQueue); 
	(*mQueue)->Release(mQueue); 
	
	
	//TODO: check if the interface has been opened first?
	(*mInfo->interface)->close(mInfo->interface);
	(*mInfo->interface)->Release(mInfo->interface); 
}

//--------------------------------------------------------------------------------------------------//
void MacJoyStick::_initialize()
{
	assert(mInfo && "Given HidInfo invalid");
	assert(mInfo->interface && "Joystick interface invalid");
	
	//TODO: Is this necessary?
	//Clear old state
	mState.mAxes.clear();
	
	if ((*mInfo->interface)->open(mInfo->interface, 0) != KERN_SUCCESS)
		AIS_EXCEPT(E_General, "MacJoyStick::_initialize() >> Could not initialize joy device!");
	
	mState.clear();
	
	_enumerateCookies();
	
	mState.mButtons.resize(mInfo->numButtons);
	mState.mAxes.resize(mInfo->numAxes);
	
	mQueue = _createQueue();
}

class FindAxisCookie : public std::unary_function<std::pair<IOHIDElementCookie, AxisInfo>&, bool>
{
public:
	FindAxisCookie(IOHIDElementCookie cookie) : m_Cookie(cookie) {}
	bool operator()(const std::pair<IOHIDElementCookie, AxisInfo>& pair) const
	{
		return pair.first == m_Cookie;
	}
private:
	IOHIDElementCookie m_Cookie;
};

//--------------------------------------------------------------------------------------------------//
void MacJoyStick::capture()
{
	assert(mQueue && "Queue must be initialized before calling MacJoyStick::capture()");
	
	AbsoluteTime zeroTime = {0,0}; 
	
	IOHIDEventStruct event; 
	IOReturn result = (*mQueue)->getNextEvent(mQueue, &event, zeroTime, 0); 
	while(result == kIOReturnSuccess)
	{
		switch(event.type)
		{
            default:
            case kIOHIDElementTypeInput_ScanCodes:
            case kIOHIDElementTypeOutput:
            case kIOHIDElementTypeFeature:
            case kIOHIDElementTypeCollection:
                break;

			case kIOHIDElementTypeInput_Button:
			{
				std::vector<IOHIDElementCookie>::iterator buttonIt = std::find(mCookies.buttonCookies.begin(), mCookies.buttonCookies.end(), event.elementCookie);
				int button = std::distance(mCookies.buttonCookies.begin(), buttonIt);
				mState.mButtons[button] = (event.value == 1);
				
				if(mBuffered && mListener)
				{
					if(event.value == 0)
						mListener->buttonPressed(JoyStickEvent(ET_JoyStickButtonDown, this, mState, button));
					else if(event.value == 1)
						mListener->buttonReleased(JoyStickEvent(ET_JoyStickButtonUp, this, mState, button));
				}
				break;
			}
			case kIOHIDElementTypeInput_Misc:
				//TODO: It's an axis! - kind of - for gamepads - or should this be a pov?
			case kIOHIDElementTypeInput_Axis:
				std::map<IOHIDElementCookie, AxisInfo>::iterator axisIt = std::find_if(mCookies.axisCookies.begin(), mCookies.axisCookies.end(), FindAxisCookie(event.elementCookie));
				int axis = std::distance(mCookies.axisCookies.begin(), axisIt);
				
				//Copied from LinuxJoyStickEvents.cpp, line 149
				const AxisInfo& axisInfo = axisIt->second;
				float proportion = (float) (event.value - axisInfo.max) / (float) (axisInfo.min - axisInfo.max);
				mState.mAxes[axis].m_AbsVal = -JoyStick::MIN_AXIS - (JoyStick::MAX_AXIS * 2 * proportion);
				
				if(mBuffered && mListener) mListener->axisMoved(JoyStickEvent(ET_JoyStickAxisMove, this, mState, axis));
				break;
		}
		
		result = (*mQueue)->getNextEvent(mQueue, &event, zeroTime, 0);
	}
}

//--------------------------------------------------------------------------------------------------//
void MacJoyStick::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

//--------------------------------------------------------------------------------------------------//
Interface* MacJoyStick::queryInterface(Interface::IType type)
{
	//Thought about using covariant return type here.. however,
	//some devices may allow LED light changing, or other interface stuff
	
	//f( ff_device && type == Interface::ForceFeedback )
	//return ff_device;
	//else
	return 0;
}

//--------------------------------------------------------------------------------------------------//
void MacJoyStick::_enumerateCookies()
{
	assert(mInfo && "Given HidInfo invalid");
	assert(mInfo->interface && "Joystick interface invalid");
	
	CFTypeRef                               object; 
	long                                    number; 
	IOHIDElementCookie                      cookie; 
	long                                    usage; 
	long                                    usagePage;
	int										min;
	int										max;

	CFDictionaryRef                         element; 
	
	// Copy all elements, since we're grabbing most of the elements 
	// for this device anyway, and thus, it's faster to iterate them 
	// ourselves. When grabbing only one or two elements, a matching 
	// dictionary should be passed in here instead of NULL. 
	CFArrayRef elements; 
	IOReturn success = reinterpret_cast<IOHIDDeviceInterface122*>(*mInfo->interface)->copyMatchingElements(mInfo->interface, NULL, &elements); 
	
	if (success == kIOReturnSuccess)
	{ 
		const CFIndex numOfElements = CFArrayGetCount(elements);
		for (CFIndex i = 0; i < numOfElements; ++i) 
		{ 
			element = static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(elements, i));
			
			//Get cookie 
			object = (CFDictionaryGetValue(element, 
										   CFSTR(kIOHIDElementCookieKey))); 
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID()) 
				continue; 
			if(!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, 
								 &number)) 
				continue; 
			cookie = (IOHIDElementCookie) number; 
			
			//Get usage 
			object = CFDictionaryGetValue(element, 
										  CFSTR(kIOHIDElementUsageKey)); 
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID()) 
				continue; 
			if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, 
								  &number)) 
				continue; 
			usage = number; 
			
			//Get min
			object = CFDictionaryGetValue(element,
										  CFSTR(kIOHIDElementMinKey)); // kIOHIDElementMinKey or kIOHIDElementScaledMinKey?, no idea ...
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
				continue;
			if (!CFNumberGetValue((CFNumberRef) object, kCFNumberIntType,
								  &number))
				continue;
			min = number;
			
			//Get max
			object = CFDictionaryGetValue(element,
										  CFSTR(kIOHIDElementMaxKey)); // kIOHIDElementMaxKey or kIOHIDElementScaledMaxKey?, no idea ...
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
				continue;
			if (!CFNumberGetValue((CFNumberRef) object, kCFNumberIntType,
								  &number))
				continue;
			max = number;			
			
			//Get usage page 
			object = CFDictionaryGetValue(element, 
										  CFSTR(kIOHIDElementUsagePageKey)); 
			
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID()) 
				continue; 
			
			if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType, 
								  &number)) 
				continue; 
			
			usagePage = number;
			switch(usagePage)
			{
				case kHIDPage_GenericDesktop:
					switch(usage)
				{
					case kHIDUsage_GD_Pointer:
						break;
					case kHIDUsage_GD_X:
					case kHIDUsage_GD_Y:
					case kHIDUsage_GD_Z:
					case kHIDUsage_GD_Rx:
					case kHIDUsage_GD_Ry:
					case kHIDUsage_GD_Rz:
						mCookies.axisCookies.insert(std::make_pair(cookie, AxisInfo(min, max)));
						break;
					case kHIDUsage_GD_Slider:
					case kHIDUsage_GD_Dial:
					case kHIDUsage_GD_Wheel:
						break;
					case kHIDUsage_GD_Hatswitch:
						break;
				}
					break;
				case kHIDPage_Button:
					mCookies.buttonCookies.push_back(cookie);
					break;
			}		
		}
		
		mInfo->numButtons = mCookies.buttonCookies.size();
		mInfo->numAxes = mCookies.axisCookies.size();

	} 
	else 
	{ 
		AIS_EXCEPT(E_General, "JoyStick elements could not be copied: copyMatchingElements failed with error: " + success); 
	}
	
}

//--------------------------------------------------------------------------------------------------//
IOHIDQueueInterface** MacJoyStick::_createQueue(unsigned int depth)
{	
	assert(mInfo && "Given HidInfo invalid");
	assert(mInfo->interface && "Joystick interface invalid");
	
	IOHIDQueueInterface** queue = (*mInfo->interface)->allocQueue(mInfo->interface); 
	
	if (queue) 
	{		
		//create the queue 
		IOReturn result = (*queue)->create(queue, 0, depth); 
		
		if(result == kIOReturnSuccess)
		{		
			//add elements to the queue
			std::map<IOHIDElementCookie, AxisInfo>::iterator axisIt = mCookies.axisCookies.begin();
			for(; axisIt != mCookies.axisCookies.end(); ++axisIt)
			{
				(*queue)->addElement(queue, axisIt->first, 0);
			}
			
			std::vector<IOHIDElementCookie>::iterator buttonIt = mCookies.buttonCookies.begin();
			for(; buttonIt != mCookies.buttonCookies.end(); ++buttonIt)
			{
				(*queue)->addElement(queue, (*buttonIt), 0);
			}

			//start data delivery to queue 
			result = (*queue)->start(queue); 
			if(result == kIOReturnSuccess)
			{
				return queue;
			}
			else
			{
				AIS_EXCEPT(E_General, "Queue could not be started.");
			}
		}
		else
		{
			AIS_EXCEPT(E_General, "Queue could not be created.");
		}
	}
	else
	{
		AIS_EXCEPT(E_General, "Queue allocation failed.");
	}
}

#endif //! AIS_APPLE_PLATFORM