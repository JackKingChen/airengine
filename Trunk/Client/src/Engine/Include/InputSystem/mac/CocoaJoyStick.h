
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISJoyStick.h"
#include "mac/MacHIDManager.h"

namespace AIS
{
	struct AxisInfo
	{
		int min;
		int max;
		
		AxisInfo(int min, int max)
			: min(min), max(max) {}
	};
	
	typedef struct cookie_struct 
	{ 
		std::map<IOHIDElementCookie, AxisInfo>	axisCookies; 			
		std::vector<IOHIDElementCookie>			buttonCookies; 
	} 
	cookie_struct_t; 
	
	
	class CocoaJoyStick : public JoyStick
	{
	public:
		CocoaJoyStick(const std::string& vendor, bool buffered, HidInfo* info, InputManager* creator, int devID);
		
		virtual ~CocoaJoyStick();
		
		virtual void setBuffered(bool buffered);
		
		virtual void capture();
		
		virtual Interface* queryInterface(Interface::IType type);
		
		virtual void _initialize();
		
		void _enumerateCookies();
		
		IOHIDQueueInterface** _createQueue(unsigned int depth = 8);
        
	protected:
		HidInfo				*mInfo;
		cookie_struct_t		mCookies;
		IOHIDQueueInterface	**mQueue;
	};
}

#endif //!AIS_APPLE_PLATFORM
