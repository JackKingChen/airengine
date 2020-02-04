#include "stdafx.h"
#include "AISInputSystem.h"
#include "AISInputManager.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdarg.h>
#include <algorithm>

#ifdef AIS_ANDROID_PLATFORM
#include <jni.h>
#include <android/log.h>
#define  LOG_TAG    "AISInputSystem"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#else
#define  LOGI(...)
#endif //!AIS_ANDROID_PLATFORM

using namespace AIS;

//--------------------------------------------------------------------------------------------------

EventHandlerWrapper::EventHandlerWrapper()
//: mpDelegate(NULL) 
{
}
//EventHandlerWrapper::EventHandlerWrapper( EventHandlerDelegate* _pDelegate )
//: mpDelegate(_pDelegate)
//{
//		
//}
EventHandlerWrapper::EventHandlerWrapper( const EventHandlerDelegate& other ):mDelegate(other)
{

}
EventHandlerWrapper::~EventHandlerWrapper()
{
	//if ( mpDelegate )
	//{
	//	delete mpDelegate;
	//	mpDelegate = NULL;
	//}
}
bool EventHandlerWrapper::Equal( EventHandlerDelegate& other )
{
	//return (mpDelegate != NULL && ((*mpDelegate) == other));
	return mDelegate == other;
}
bool EventHandlerWrapper::HandleEvent( EventBase* pEvent )
{
	bool bRes = false;
	//if ( mpDelegate )
	//{
	//	bRes = (*mpDelegate)(pEvent);
	//}
	bRes = mDelegate(pEvent);
	return bRes;
}																			   

//--------------------------------------------------------------------------------------------------

EventProcessor::EventProcessor(const char* szName)
: mReturnImmediateWhenHandleSuccess(false)
, mEventAcceptFlag(-1)
{
	if ( szName )
		mName = szName;
	mEventHandlerMap.clear();
	mChildrenProcessor.clear();
}
EventProcessor::~EventProcessor()
{
	Clear();
}

AISDeviceType GetEventAcceptFlag( unsigned int eventType )
{
    switch (eventType)
    {          
        case ET_KeyDown:					// 閿洏 - 鎸変笅鎸夐敭
        case ET_KeyUp:						// 閿洏 - 鏉惧紑鎸夐敭
            return AISKeyboard;
            
        case ET_MouseMove:					// 榧犳爣 - 绉诲姩
        case ET_MouseButtonDown:			// 榧犳爣 - 鎸変笅鎸夐敭
        case ET_MouseButtonUp:				// 榧犳爣 - 鏉惧紑鎸夐敭
        case ET_MouseButtonDBClick:			// 榧犳爣 - 鍙屽嚮鎸夐敭
            return AISMouse;
            
        case ET_JoyStickButtonDown:			// 鎵嬫焺 - 鎸変笅鎵嬫焺涓婄殑鏌愪釜鎸夐敭
        case ET_JoyStickButtonUp:			// 鎵嬫焺 - 鏉惧紑鎵嬫焺涓婄殑鏌愪釜鎸夐敭
        case ET_JoyStickSliderMove:			// 鎵嬫焺 - Slider Axis鍙戠敓鍙樺寲
        case ET_JoyStickAxisMove:           // 鎵嬫焺 - 鎽囨潌鍙戠敓鍙樺寲
        case ET_JoyStickPovMove:			// 鎵嬫焺 - 鍗佸瓧鏂瑰悜閿彂鐢熷彉鍖�
        case ET_JoyStickVector3Move:		// 鎵嬫焺 - Vector type controls the device exports
            return AISJoyStick;
            
        case ET_MultiTouchesPress:			// 瑙︽懜灞�- 鎵嬫寚鎸変笅
        case ET_MultiTouchesRelease:		// 瑙︽懜灞�- 鎵嬫寚鏉惧紑
        case ET_MultiTouchesCancle:         // 瑙︽懜灞�- 鎵嬫寚绉诲姩鍒板睆骞曞,鍙栨秷瑙︽懜
        case ET_MultiTouchesMove:			// 瑙︽懜灞�- 鎵嬫寚绉诲姩
            return AISMultiTouch;
            
        case ET_SensorGravityChanged:		// 鎰熷簲鍣�- 閲嶅姏鎰熷簲鍙戠敓鏀瑰彉
        case ET_SensorOrientationChanged:	// 鎰熷簲鍣�- 灞忓箷鏃嬭浆
            return AISSensor;
            
        default:
            return AISUnknown;
    }
}

bool EventProcessor::ProcessSelfEvent( unsigned int eventType, EventBase* pEvent )
{
    if (false == IsEventAccept(GetEventAcceptFlag(eventType)))
    {
        return false;
    }
    
	EventHandlerWrapperList* pEvtList = NULL;
	EventHandlerMap::iterator it = mEventHandlerMap.find( eventType );
	if ( it == mEventHandlerMap.end() )
		return false;
	pEvtList = it->second;
	EventHandlerWrapperList::iterator itb = pEvtList->begin(), ite = pEvtList->end();
	bool bResult = false, bFinalRes = false;
	for ( ; itb != ite; ++itb )
	{
		EventHandlerWrapper* pPair = *itb;
		if ( pPair )
		{
			bResult = pPair->HandleEvent(pEvent);
			if ( bResult )
			{
				bFinalRes = true;
				if ( mReturnImmediateWhenHandleSuccess )
					return true;
			}
		}
	}
	return bFinalRes;
}

bool EventProcessor::ProcessEvent(unsigned int eventType, EventBase* pEvent) 
{
    if (false == IsEventAccept(GetEventAcceptFlag(eventType)))
    {
        return false;
    }
    
	bool bResult = false, bFinalRes = false;
	EventProcessorList::iterator itb = mChildrenProcessor.begin(), ite = mChildrenProcessor.end();
	for ( ; itb != ite; ++itb )
	{
		EventProcessor* pChild = *itb;
		bResult = pChild->ProcessEvent( eventType, pEvent );
		if ( bResult )
		{
			bFinalRes = bResult;
			if ( mReturnImmediateWhenHandleSuccess )
				return true;
		}
	}
	if ( !bFinalRes )
		bFinalRes = ProcessSelfEvent(eventType, pEvent);
	return bFinalRes;
}

void EventProcessor::AddChildEventProcessor( EventProcessor* pChild, bool bAddWithoutNameCheck )
{
	if ( bAddWithoutNameCheck == false )
	{
		if ( GetChildEventProcessor( pChild->GetName().c_str() ) )
			return;
	}
	mChildrenProcessor.push_back( pChild );
}

void EventProcessor::RemoveChildEventProcessor( EventProcessor* pChild, bool bDestroy /*= true*/ )
{
	EventProcessorList::iterator it = std::find( mChildrenProcessor.begin(), mChildrenProcessor.end(), pChild );
	if ( it != mChildrenProcessor.end() )
	{
		EventProcessor* pChild = *it;
		if ( pChild && bDestroy )
		{
			pChild->Clear();
			delete pChild;
		}
		mChildrenProcessor.erase( it );
	}
}

EventProcessor* EventProcessor::GetChildEventProcessor( const char* szName )
{
	EventProcessorList::iterator itb = mChildrenProcessor.begin(), ite = mChildrenProcessor.end();
	while ( itb != ite )
	{
		EventProcessor* pChild = *itb;
		if ( pChild && pChild->GetName() == szName )
		{
			return pChild;
		}
		++itb;
	}
	return NULL;
}

void EventProcessor::Clear()
{
	EventProcessorList::iterator itb = mChildrenProcessor.begin(), ite = mChildrenProcessor.end();
	for ( ; itb != ite; ++itb )
	{
		EventProcessor* pChild = *itb;
		if ( pChild )
		{
			pChild->Clear();
			delete pChild;
		}
	}
	mChildrenProcessor.clear();
	EventHandlerMap::iterator eitb = mEventHandlerMap.begin(), eite = mEventHandlerMap.end();
	for ( ; eitb != eite; ++eitb )
	{
		EventHandlerWrapperList* pEvtList = eitb->second;
		EventHandlerWrapperList::iterator ditb = pEvtList->begin(), dite = pEvtList->end();
		for ( ; ditb != dite; ++ditb )
		{
			EventHandlerWrapper* pPair = *ditb;
			if ( pPair )
			{
				delete pPair;
			}
		}
		pEvtList->clear();
	}
	mEventHandlerMap.clear();
}


//--------------------------------------------------------------------------------------------------


bool InputEventListener::keyPressed( const KeyEvent &arg ) 
{
	std::string info = FormatString( "KeyPressed { %d, %s, 0x%x } || Character ( %c )\n",
		arg.key, arg.GetCastEventSender<Keyboard>()->getAsString(arg.key).c_str(), arg.key, (char)arg.text );
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );		   

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::keyReleased( const KeyEvent &arg ) 
{
	std::string info = FormatString( "KeyReleased { %s }\n", arg.GetCastEventSender<Keyboard>()->getAsString(arg.key).c_str() );
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::mouseMoved( const MouseEvent &arg ) 
{
	const AIS::MouseState& s = arg.state;
	std::string info = FormatString( "MouseMoved: Abs(%d, %d, %d) Rel(%d, %d, %d)\n",
		s.X.m_AbsVal, s.Y.m_AbsVal, s.Z.m_AbsVal, s.X.m_RelVal, s.Y.m_RelVal, s.Z.m_RelVal);
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::mousePressed( const MouseEvent &arg ) 
{
	const AIS::MouseState& s = arg.state;
	std::string info = FormatString( "MousePress: button(%d) Abs(%d, %d, %d) Rel(%d, %d, %d)\n",
		arg.buttonID, s.X.m_AbsVal, s.Y.m_AbsVal, s.Z.m_AbsVal, s.X.m_RelVal, s.Y.m_RelVal, s.Z.m_RelVal);
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::mouseReleased( const MouseEvent &arg ) 
{
	const AIS::MouseState& s = arg.state;
	std::string info = FormatString( "MouseRelease: button(%d) Abs(%d, %d, %d) Rel(%d, %d, %d)\n",
		arg.buttonID, s.X.m_AbsVal, s.Y.m_AbsVal, s.Z.m_AbsVal, s.X.m_RelVal, s.Y.m_RelVal, s.Z.m_RelVal);
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::mouseDBClicked( const MouseEvent &arg )
{
	const AIS::MouseState& s = arg.state;
	std::string info = FormatString( "MouseDBClick: button(%d) Abs(%d, %d, %d) Rel(%d, %d, %d)\n",
		arg.buttonID, s.X.m_AbsVal, s.Y.m_AbsVal, s.Z.m_AbsVal, s.X.m_RelVal, s.Y.m_RelVal, s.Z.m_RelVal);
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::buttonPressed( const JoyStickEvent &arg ) 
{
	std::string info = FormatString( "%s. JoyStick Button Pressed #%d\n", arg.GetCastEventSender<JoyStick>()->vendor().c_str(), arg.mButtonAxisPovIndex );
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::buttonReleased( const JoyStickEvent &arg ) 
{
	std::string info = FormatString( "%s. JoyStick Button Released #%d\n", arg.GetCastEventSender<JoyStick>()->vendor().c_str(), arg.mButtonAxisPovIndex );
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::sliderMoved( const JoyStickEvent &arg )
{
	std::string info = FormatString( "%s. JoyStick Event sliderMoved : %d\n", arg.GetCastEventSender<JoyStick>()->vendor().c_str(), arg.mButtonAxisPovIndex );
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::axisMoved( const JoyStickEvent &arg )
{
	//Provide a little dead zone
	if( arg.state.mAxes.size() == 0 )
		return true;
	std::string info("");
	if( arg.state.mAxes[arg.mButtonAxisPovIndex].m_AbsVal > 2500 || arg.state.mAxes[arg.mButtonAxisPovIndex].m_AbsVal < -2500 )
	{
		info += FormatString( "%s. JoyStick Axis: %d;  Value:%d\n", arg.GetCastEventSender<JoyStick>()->vendor().c_str(), arg.mButtonAxisPovIndex, arg.state.mAxes[arg.mButtonAxisPovIndex].m_AbsVal );
	}
	//std::cout << info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::povMoved( const JoyStickEvent &arg )
{
	std::string info = FormatString( "%s. JoyStick POV( %d ) ", arg.GetCastEventSender<JoyStick>()->vendor().c_str(), arg.mButtonAxisPovIndex );

	if( arg.state.mPOV[arg.mButtonAxisPovIndex].direction & Pov::North ) //Going up
	{
		info += "North";
	}
	else if( arg.state.mPOV[arg.mButtonAxisPovIndex].direction & Pov::South ) //Going down
	{
		info += "South";
	}

	if( arg.state.mPOV[arg.mButtonAxisPovIndex].direction & Pov::East ) //Going right
	{
		info += "East";
	}
	else if( arg.state.mPOV[arg.mButtonAxisPovIndex].direction & Pov::West ) //Going left
	{
		info += "West";
	}

	if( arg.state.mPOV[arg.mButtonAxisPovIndex].direction == Pov::Centered ) //stopped/centered out
	{
		info += "Centered";
	}
	info += "\n";
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}

bool InputEventListener::vector3Moved( const JoyStickEvent &arg )
{
	std::cout.precision(2);
	std::cout.flags(std::ios::fixed | std::ios::right);

	std::string info = FormatString( "%s. Orientation # %d X(%d), Y(%d), Z(%d)\n",
		arg.GetCastEventSender<JoyStick>()->vendor().c_str(), arg.mButtonAxisPovIndex, arg.state.mVectors[arg.mButtonAxisPovIndex].x, arg.state.mVectors[arg.mButtonAxisPovIndex].y, arg.state.mVectors[arg.mButtonAxisPovIndex].z );
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	std::cout.precision();
	std::cout.flags();

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::touchesMoved( const MultiTouchEvent &arg )
{
	std::string info = FormatString( "@@@@@@@ trigger touchesMoved, there are %d touches!\n", arg.mStates.size() );
	for (int i = 0; i < arg.mStates.size(); ++i)
	{
		info += FormatString( "Event touchesMoved : %d, pos(%d, %d), type(%d)\n",
			arg.mStates[i]->m_touchID, int(arg.mStates[i]->m_point.mX), int(arg.mStates[i]->m_point.mY), arg.mStates[i]->m_touchType );
	}
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::touchesPressed( const MultiTouchEvent &arg )
{
	std::string info = FormatString( "@@@@@@@ trigger touchesPressed, there are %d touches!\n", arg.mStates.size() );
	for (int i = 0; i < arg.mStates.size(); ++i)
	{
		info += FormatString( "Event touchesMoved : %d, pos(%d, %d), type(%d)\n",
            arg.mStates[i]->m_touchID, int(arg.mStates[i]->m_point.mX), int(arg.mStates[i]->m_point.mY), arg.mStates[i]->m_touchType );
	}
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::touchesReleased( const MultiTouchEvent &arg )
{
	std::string info = FormatString( "@@@@@@@ trigger touchesReleased, there are %d touches!\n", arg.mStates.size() );
	for (int i = 0; i < arg.mStates.size(); ++i)
	{
		info += FormatString( "Event touchesMoved : %d, pos(%d, %d), type(%d)\n",
            arg.mStates[i]->m_touchID, int(arg.mStates[i]->m_point.mX), int(arg.mStates[i]->m_point.mY), arg.mStates[i]->m_touchType );
	}
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::touchesCancelled( const MultiTouchEvent &arg )
{
	std::string info = FormatString( "@@@@@@@ trigger touchesCancelled, there are %d touches!\n", arg.mStates.size() );
	for (int i = 0; i < arg.mStates.size(); ++i)
	{
		info += FormatString( "Event touchesMoved : %d, pos(%d, %d), type(%d)\n",
            arg.mStates[i]->m_touchID, int(arg.mStates[i]->m_point.mX), int(arg.mStates[i]->m_point.mY), arg.mStates[i]->m_touchType );
	}
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::orientationChanged( const SensorEvent& arg )
{
	std::string info = FormatString( "sensor orientationChanged : %d, %d\n", arg.state.mOrientation, arg.state.mRotateAngle );
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}
bool InputEventListener::gravityChanged( const SensorEvent& arg )
{
	std::string info = FormatString( "sensor gravityChanged : %f, %f, %f\n", arg.state.mGravity.x, arg.state.mGravity.y, arg.state.mGravity.z );
	//std::cout<< info.c_str();
	//LOGI( "%s", info.c_str() );

	return ProcessEvent( arg.GetEventType(), (EventBase*)(&arg) );
}

//--------------------------------------------------------------------------------------------------
AISInputSystem* AISInputSystem::sInst = NULL;

AISInputSystem::AISInputSystem()
: mpListener(NULL)
, mpInputManager(NULL)
, mpMultiTouchDevice(NULL)
, mpKeyBoardDevice(NULL)
, mpMouseDevice(NULL)
, mpJoyStick(NULL)
, mpSensor(NULL)
{

}

AISInputSystem::~AISInputSystem()
{

}


AISInputSystem* AISInputSystem::CreateInstance()
{
	if ( sInst == NULL )
	{
		sInst = new AISInputSystem();
	}
	return sInst;
}

AISInputSystem* AISInputSystem::GetInstance()
{
	return sInst;
}

void AISInputSystem::DestroyInstance()
{
	if ( sInst )
	{
		delete sInst;
		sInst = NULL;
	}	
}

void AISInputSystem::SetListener(InputEventListener* pListener)
{
    if (mpListener)
    {
		mpListener->Clear();
        delete mpListener;
    }
    mpListener = pListener;
    
	if ( mpKeyBoardDevice )
	{
		mpKeyBoardDevice->setEventCallback(mpListener);
	}
    
	if ( mpMouseDevice )
	{
		mpMouseDevice->setEventCallback(mpListener);
	}
    
	if ( mpJoyStick )
	{
		mpJoyStick->setEventCallback(mpListener);
	}
    
	if ( mpMultiTouchDevice )
	{
		mpMultiTouchDevice->setEventCallback(mpListener);
	}
    
	if ( mpSensor )
	{
		mpSensor->setEventCallback(mpListener);
	}
}

bool AISInputSystem::SetupInput( unsigned long winHandle, InputEventListener* pListener, bool nograb /*= false*/ )
{
	mpListener = pListener;

	AIS::ParamList pl;
	std::ostringstream winHandleStr;
	winHandleStr << winHandle;
	std::string handlwStr = winHandleStr.str();
	std::string::size_type pos = handlwStr.find(",");
	while ( pos != std::string::npos )
	{
		handlwStr.replace( pos, 1, "" );
		pos = handlwStr.find( "," );
	}

	pl.insert(std::make_pair("WINDOW", handlwStr.c_str()));
	if (nograb)
	{
		pl.insert(std::make_pair("x11_keyboard_grab", "false"));
		pl.insert(std::make_pair("x11_mouse_grab", "false"));
		pl.insert(std::make_pair("w32_mouse", "DISCL_FOREGROUND"));
		pl.insert(std::make_pair("w32_mouse", "DISCL_NONEXCLUSIVE"));
		pl.insert(std::make_pair("w32_keyboard", "DISCL_FOREGROUND"));
		pl.insert(std::make_pair("w32_keyboard", "DISCL_NONEXCLUSIVE"));
	}

	mpInputManager = AIS::InputManager::createInputManager(pl);
	if ( !mpInputManager )
		return false;

	//Print debugging information
	unsigned int v = mpInputManager->getVersionNumber();
    /*
	std::cout << "AIS Version: " << (v>>16 ) << "." << ((v>>8) & 0x000000FF) << "." << (v & 0x000000FF)
		<< "\nRelease Name: "		<< mpInputManager->getVersionName().c_str()
		<< "\nManager: "			<< mpInputManager->inputManagerName().c_str()
		<< "\nTotal Keyboards: "	<< mpInputManager->getNumberOfDevices(AISKeyboard)
		<< "\nTotal Mouses: "		<< mpInputManager->getNumberOfDevices(AISMouse)
		<< "\nTotal JoySticks: "	<< mpInputManager->getNumberOfDevices(AISJoyStick)
		<< "\nTotal Sensor: "		<< mpInputManager->getNumberOfDevices(AISSensor)
		<< "\nTotal MultiTouchs: "	<< mpInputManager->getNumberOfDevices(AISMultiTouch)
		<< "\n";*/

	const char *g_DeviceType[6] = { "AISUnknown", "AISKeyboard", "AISMouse", "AISJoyStick", "AISMultiTouch", "AISSensor" };
	//List all devices
	AIS::DeviceList list = mpInputManager->listFreeDevices();
	for( AIS::DeviceList::iterator i = list.begin(); i != list.end(); ++i )
    {
        std::string buf = FormatString("\tDevice : %s Vendor : %s\n", g_DeviceType[i->first], i->second.c_str());
        //std::cout << buf.c_str();
		//std::cout << "\n\tDevice: " << g_DeviceType[i->first] << " Vendor: " << i->second << "\n";
    }

	// create the specific input devices
	mpKeyBoardDevice = static_cast<AIS::Keyboard*>(mpInputManager->createInputObject(AIS::AISKeyboard, true));
	if ( mpKeyBoardDevice )
	{
		mpKeyBoardDevice->setEventCallback(mpListener);
	}

	mpMouseDevice = static_cast<AIS::Mouse*>(mpInputManager->createInputObject(AIS::AISMouse, true));
	if ( mpMouseDevice )
	{
		//mpMouseDevice->setMouseShow(true);
		mpMouseDevice->setEventCallback(mpListener);
	}

	mpJoyStick = static_cast<AIS::JoyStick*>(mpInputManager->createInputObject(AIS::AISJoyStick, true));
	if ( mpJoyStick )
	{
		mpJoyStick->setEventCallback(mpListener);
	}

	mpMultiTouchDevice = static_cast<AIS::MultiTouch*>(mpInputManager->createInputObject(AIS::AISMultiTouch, true));
	if ( mpMultiTouchDevice )
	{
		mpMultiTouchDevice->setEventCallback(mpListener);
	}

	mpSensor = static_cast<AIS::Sensor*>(mpInputManager->createInputObject(AIS::AISSensor, true));
	if ( mpSensor )
	{
		mpSensor->setEventCallback(mpListener);
	}

	return true;
}

void AISInputSystem::ShutdownInput()
{
	if (mpInputManager)
	{
		if ( mpKeyBoardDevice )
		{
			mpInputManager->destroyInputObject(mpKeyBoardDevice);
			mpKeyBoardDevice = NULL;
		}
		if ( mpMouseDevice )
		{
			mpInputManager->destroyInputObject(mpMouseDevice);
			mpMouseDevice = NULL;
		}
		if ( mpMultiTouchDevice )
		{
			mpInputManager->destroyInputObject(mpMultiTouchDevice);
			mpMultiTouchDevice = NULL;
		}
		if ( mpJoyStick )
		{
			mpInputManager->destroyInputObject(mpJoyStick);
			mpJoyStick = NULL;
		}
		if ( mpSensor )
		{
			mpInputManager->destroyInputObject(mpSensor);
			mpSensor = NULL;
		}
		AIS::InputManager::destroyInputManager(mpInputManager);
		mpInputManager = NULL;
	}
	if (mpListener)
	{
		mpListener->Clear();
		delete mpListener;
	}
}

void AISInputSystem::UpdateInput()
{
	if ( mpKeyBoardDevice )
	{
		mpKeyBoardDevice->capture();
	}
	if ( mpMouseDevice )
	{
		mpMouseDevice->capture();
	}
	if ( mpMultiTouchDevice )
	{
		mpMultiTouchDevice->capture();
	}
	if ( mpJoyStick )
	{
		mpJoyStick->capture();
	}
	if ( mpSensor )
	{
		mpSensor->capture();
	}
}
