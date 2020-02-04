
#pragma once

#ifdef AIS_IPHONE_PLATFORM

#include "AISInputManager.h"
#include "AISFactoryCreator.h"

namespace AIS
{
    class iPhoneSensor;
    class iPhoneMultiTouch;
};

//--------------------------------------------------------------------------------//

#if __OBJC__
#import <UIKit/UIKit.h>

// 输入代理
@interface InputDelegate : UIView <UIAccelerometerDelegate>
{
    AIS::iPhoneSensor                       *accelerometerObject;   // 感应器
    AIS::iPhoneMultiTouch                   *touchObject;           // 触摸屏
}

@property (assign) AIS::iPhoneSensor        *accelerometerObject;   // 感应器
@property (assign) AIS::iPhoneMultiTouch    *touchObject;           // 触摸屏

// 按下
- (void) handleTouchEventBegan:(NSSet *)touches :(UIEvent *)event :(CGRect)rc;
// 离开
- (void) handleTouchEventEnded:(NSSet *)touches :(UIEvent *)event :(CGRect)rc;
// 移动
- (void) handleTouchEventMoved:(NSSet *)touches :(UIEvent *)event :(CGRect)rc;
// 取消
- (void) handleTouchEventCancelled:(NSSet *)touches :(UIEvent *)event :(CGRect)rc;

@end

#endif

//--------------------------------------------------------------------------------//

namespace AIS
{

    class iPhoneInputManager : public InputManager, public FactoryCreator
    {
    public:
        iPhoneInputManager();
        virtual ~iPhoneInputManager();

		void _initialize( ParamList &paramList );

		DeviceList freeDeviceList();

		int totalDevices(AISDeviceType iType);

		int freeDevices(AISDeviceType iType);

		bool vendorExist(AISDeviceType iType, const std::string & vendor);

		DeviceObject* createObject(InputManager* creator, AISDeviceType iType, bool bufferMode, const std::string & vendor = "");

		void destroyObject(DeviceObject* obj);

		//Internal Items
		void _setMultiTouchUsed(bool used) { bMultiTouchUsed = used; }

		void _setAccelerometerUsed(bool used) { bAccelerometerUsed = used; }

        
#if __OBJC__
        //! methodfor getting the delegate
        InputDelegate * _getDelegate() { return mDelegate; }

        //! method for getting window
        UIWindow * _getWindow() { return mWindow; }
#endif

    protected:        
        void _parseConfigSettings( ParamList& paramList );

        // iPhone stuff
#if __OBJC__
		UIWindow *mWindow;
        InputDelegate *mDelegate;
#endif

        // settings
        bool mHideMouse;

		//! Used to know if we used up multi-touch device
		bool bMultiTouchUsed;

        //! Used to know if we used up accelerometer
		bool bAccelerometerUsed;

        bool mIsContentScalingSupported;
        float mContentScalingFactor;
    };
}

#endif //! AIS_IPHONE_PLATFORM
