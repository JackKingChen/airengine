
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include <string>
#include <list>
#include <CoreFoundation/CoreFoundation.h>

namespace AIS
{
    class MacInputManager;
	class MacHIDManager;
	class MacMouse;
    class MacKeyboard;

	/** 
		Simple wrapper class for CFString which will create a valid CFString and retain ownership until class instance is outof scope
		To Access the CFStringRef instance, simply cast to void*, pass into a function expecting a void* CFStringRef object, or access via cf_str() method
	*/
	class AIS_CFString
	{
	public:
		AIS_CFString() { m_StringRef = CFStringCreateWithCString(NULL, "", kCFStringEncodingUTF8); }
		AIS_CFString(const char* c_str) { m_StringRef = CFStringCreateWithCString(NULL, c_str, kCFStringEncodingUTF8); }
		AIS_CFString(const std::string &s_str) { m_StringRef = CFStringCreateWithCString(NULL, s_str.c_str(), kCFStringEncodingUTF8); }
		~AIS_CFString() { CFRelease(m_StringRef); }

		//Allow this class to be autoconverted to base class of StringRef (void*)
		operator void*() { return (void*)m_StringRef; }
		CFStringRef cf_str() { return m_StringRef; }
	
	private:
		CFStringRef m_StringRef;
	};
}

#endif //!AIS_APPLE_PLATFORM

