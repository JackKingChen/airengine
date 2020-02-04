
#pragma once

#ifdef AIS_APPLE_PLATFORM

#include "AISInputManager.h"
#include "AISFactoryCreator.h"
#include "mac/MacPrereqs.h"
#include <Carbon/Carbon.h>

namespace AIS
{
    
    class MacInputManager : public InputManager, public FactoryCreator
    {
    public:
        MacInputManager();
        virtual ~MacInputManager();
        
		void _initialize( ParamList &paramList );

		DeviceList freeDeviceList();

		int totalDevices(AISDeviceType iType);

		int freeDevices(AISDeviceType iType);

		bool vendorExist(AISDeviceType iType, const std::string & vendor);

		DeviceObject* createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor = "");

		void destroyObject(DeviceObject* obj);

		void _setKeyboardUsed(bool used) {keyboardUsed = used; }

		void _setMouseUsed(bool used) { mouseUsed = used; }
        
        EventTargetRef _getEventTarget() {return mEventTargetRef;}
        
        WindowRef _getWindow() {return mWindow;}
        
    protected:        
        void _parseConfigSettings( ParamList& paramList );
        
        void _enumerateDevices();
        
        static const std::string iName;
        
		EventTargetRef	mEventTargetRef;
		WindowRef		mWindow;        
        bool			mHideMouse;
        bool			mUseRepeat;
		bool			keyboardUsed;
		bool			mouseUsed;		
		MacHIDManager	*mHIDManager;
    };
}

#endif //!AIS_APPLE_PLATFORM

