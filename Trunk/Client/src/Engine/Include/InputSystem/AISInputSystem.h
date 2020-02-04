
#pragma once

#include "AISKeyboard.h"
#include "AISMouse.h"
#include "AISMultiTouch.h"
#include "AISSensor.h"
#include "AISJoyStick.h"

#include "FastDelegate.h"
#include "MemoryAllocatorConfig.h"
#include <assert.h>

namespace AIS
{

	class InputManager;

	typedef fastdelegate::FastDelegate1<EventBase*, bool> EventHandlerDelegate;

	//--------------------------------------------------------------------------------------------------

    // 事件处理包装
	class AIR_INPUT_API EventHandlerWrapper
	{
		EngineAllocator(EventHandlerWrapper);
	public:
		EventHandlerWrapper();
		EventHandlerWrapper( const EventHandlerDelegate& other );
		~EventHandlerWrapper();
		bool Equal( EventHandlerDelegate& other );
		bool HandleEvent( EventBase* pEvent );

	protected:
        // 事件回调代理
		EventHandlerDelegate mDelegate;
	};
	typedef std::list<EventHandlerWrapper*> EventHandlerWrapperList;
	typedef std::map<unsigned int, EventHandlerWrapperList*> EventHandlerMap;

	//--------------------------------------------------------------------------------------------------

    // 事件处理器
	class AIR_INPUT_API EventProcessor
	{
		EngineAllocator(EventProcessor);
	public:
		typedef std::list<EventProcessor*> EventProcessorList;

	public:
		EventProcessor( const char* szName );
		virtual ~EventProcessor();

        // 设置当有事件处理成功后是否立即返回
		void SetReturnImmediateWhenHandleSuccess( bool b )	{ mReturnImmediateWhenHandleSuccess = b; }
		bool GetReturnImmediateWhenHandleSuccess() { return mReturnImmediateWhenHandleSuccess; }

        // 添加全局/静态事件处理Handler
		template<typename EventStruct>
		void AddEventHandler( unsigned int eventType, bool (*phandlerFunc)(EventStruct*) );

        // 添加成员事件处理Handler
		template<typename HandlerObject, typename EventStruct>
		void AddEventHandler( unsigned int eventType, HandlerObject* phObject, bool (HandlerObject::* phandlerFunc)(EventStruct*) );

        // 移除全局/静态事件处理Handler
		template<typename EventStruct>
		void RemoveEventHandler( unsigned int eventType, bool (*phandlerFunc)(EventStruct*) );

        // 移除成员事件处理Handler
		template<typename HandlerObject, typename EventStruct>
		void RemoveEventHandler( unsigned int eventType, HandlerObject* phObject, bool (HandlerObject::* phandlerFunc)(EventStruct*) );

		// 派发并处理事件
		virtual bool ProcessEvent(unsigned int eventType, EventBase* pEvent);

		// 添加子事件处理器
		virtual void AddChildEventProcessor( EventProcessor* pChild, bool bAddWithoutNameCheck = true );
		// 移除子事件处理器, 如果bDestroy == true, 则顺带销毁子事件处理器对象
		virtual void RemoveChildEventProcessor( EventProcessor* pChild, bool bDestroy = true );


        // 处理自身的事件
		virtual bool ProcessSelfEvent( unsigned int eventType, EventBase* pEvent );

		virtual EventProcessor* GetChildEventProcessor( const char* szName );
		virtual void Clear();
        
		// 设置是否处理某种设备事件
        void SetEventAccept( AISDeviceType type, bool bEnable );
        // 检测是否处理某种设备事件
        bool IsEventAccept( AISDeviceType type );

		const std::string& GetName() const;

	protected:
		std::string			mName;								// 事件处理器名称
		EventHandlerMap		mEventHandlerMap;					// 事件处理Map表
		bool				mReturnImmediateWhenHandleSuccess;	// 如果当前有事件Handler返回True的话,是否立即返回,不再对余下的Handler进行判断
		EventProcessorList	mChildrenProcessor;					// 子事件处理器列表
        unsigned int        mEventAcceptFlag;                   // 接收事件类型的标记, 从低位到高位,分别代表 键盘事件,鼠标事件,手柄事件,触摸屏事件,感应器事件...,置1表示处理,否则表示不处理
	};

	//--------------------------------------------------------------------------------------------------

	class AIR_INPUT_API IDeviceMsgHandler
	{
		EngineAllocator(IDeviceMsgHandler);
	public:
		IDeviceMsgHandler(){}
		virtual ~IDeviceMsgHandler(){}

		// key board event
		virtual bool OnKeyPressed( EventBase* pEvent ) { return false; }
		virtual bool OnKeyReleased( EventBase* pEvent ) { return false; }

		// mouse event
		virtual bool OnMouseMoved( EventBase* pEvent ) { return false; }
		virtual bool OnMousePressed( EventBase* pEvent ) { return false; }
		virtual bool OnMouseReleased( EventBase* pEvent ) { return false; }
		virtual bool OnMouseDBClicked( EventBase* pEvent ) { return false; }

		// joystick or accelerometer event
		virtual bool OnButtonPressed( EventBase* pEvent ) { return false; }
		virtual bool OnButtonReleased( EventBase* pEvent ) { return false; }
		virtual bool OnSliderMoved( EventBase* pEvent ) { return false; }
		virtual bool OnAxisMoved( EventBase* pEvent ) { return false; }
		virtual bool OnPovMoved( EventBase* pEvent ) { return false; }
		virtual bool OnVector3Moved( EventBase* pEvent )  { return false; }

		// multitouch event	   
		virtual bool OnTouchesMoved( EventBase* pEvent ) { return false; }
		virtual bool OnTouchesPressed( EventBase* pEvent ) { return false; }
		virtual bool OnTouchesReleased( EventBase* pEvent ) { return false; }
		virtual bool OnTouchesCancelled( EventBase* pEvent ) { return false; }

		// accelerometer event
		virtual bool OnOrientationChanged( EventBase* pEvent ) { return false; }
		virtual bool OnGravityChanged( EventBase* pEvent ) { return false; }
	};

#define ADD_MEMBER_MSG_HANDLER( event_processor_ptr, event_type, HANDLER_CLS, handler_obj_ptr, EVENT_CLS, handler_func ) \
	(event_processor_ptr)->AddEventHandler<HANDLER_CLS, EVENT_CLS>(event_type, handler_obj_ptr, handler_func)

#define ADD_MSG_HANDLER( event_processor_ptr, event_type, EVENT_CLS, handler_func ) \
	(event_processor_ptr)->AddEventHandler<EVENT_CLS>( event_type, handler_func )


#define REMOVE_MEMBER_MSG_HANDLER( event_processor_ptr, event_type, HANDLER_CLS, handler_obj_ptr, EVENT_CLS, handler_func ) \
	(event_processor_ptr)->RemoveEventHandler<HANDLER_CLS, EVENT_CLS>(event_type, handler_obj_ptr, handler_func)

#define REMOVE_MSG_HANDLER( event_processor_ptr, event_type, EVENT_CLS, handler_func ) \
	(event_processor_ptr)->RemoveEventHandler<EVENT_CLS>( event_type, handler_func )

	//--------------------------------------------------------------------------------------------------

    // 输入事件监听器
	class AIR_INPUT_API InputEventListener
		: public KeyListener
		, public MouseListener
		, public MultiTouchListener
		, public JoyStickListener
		, public SensorListener
		, public EventProcessor
	{
		EngineAllocator(InputEventListener);
	public:
		InputEventListener(const char* szName) : EventProcessor(szName) {}
		virtual ~InputEventListener() {}

		// key board event
		virtual bool keyPressed( const KeyEvent &arg );
		virtual bool keyReleased( const KeyEvent &arg );

		// mouse event
		virtual bool mouseMoved( const MouseEvent &arg );
		virtual bool mousePressed( const MouseEvent &arg );
		virtual bool mouseReleased( const MouseEvent &arg );
		virtual bool mouseDBClicked( const MouseEvent &arg );

		// joystick or accelerometer event
		virtual bool buttonPressed( const JoyStickEvent &arg );
		virtual bool buttonReleased( const JoyStickEvent &arg );
		virtual bool sliderMoved( const JoyStickEvent &arg );
		virtual bool axisMoved( const JoyStickEvent &arg );
		virtual bool povMoved( const JoyStickEvent &arg );
		virtual bool vector3Moved( const JoyStickEvent &arg );

		// multitouch event	   
		virtual bool touchesMoved( const MultiTouchEvent &arg );
		virtual bool touchesPressed( const MultiTouchEvent &arg );
		virtual bool touchesReleased( const MultiTouchEvent &arg );
		virtual bool touchesCancelled( const MultiTouchEvent &arg );

		// accelerometer event
		virtual bool orientationChanged( const SensorEvent& arg );
		virtual bool gravityChanged( const SensorEvent& arg );

	};

	//--------------------------------------------------------------------------------------------------

	class AIR_INPUT_API AISInputSystem
	{	
		EngineAllocator(AISInputSystem);
		AISInputSystem();

	public:
		~AISInputSystem();

		static AISInputSystem* CreateInstance();
		static AISInputSystem* GetInstance();
		static void DestroyInstance();

        // initialize
        // winHandle - 窗口对象
        // pListener - 事件监听器
        // nograb - 非独占设备资源
		bool SetupInput( unsigned long winHandle, InputEventListener* pListener, bool nograb = true );

		void ShutdownInput();
        
        // update per frame
		void UpdateInput();

        void                SetListener(InputEventListener* pListener);
		InputEventListener* GetListener()               { return mpListener; }
		InputManager*       GetInputManager()           { return mpInputManager; }
		MultiTouch*         GetMultiTouchDevice()       { return mpMultiTouchDevice; }
		Keyboard*           GetKeyboardDevice()         { return mpKeyBoardDevice; }
		Mouse*              GetMouseDevice()            { return mpMouseDevice; }
		JoyStick*           GetJoyStick()				{ return mpJoyStick; }
		Sensor*				GetSensor()					{ return mpSensor; }

	protected:
		static AISInputSystem* sInst;

		InputEventListener* mpListener;				// 输入事件监听
		InputManager*       mpInputManager;			// 输入设备管理器
		MultiTouch*         mpMultiTouchDevice;		// 触摸屏
		Keyboard*           mpKeyBoardDevice;		// 键盘
		Mouse*              mpMouseDevice;			// 鼠标
		JoyStick*           mpJoyStick;				// 手柄
		Sensor*				mpSensor;				// 感应器

	};

    //--------------------------------------------------------------------------------------------------



	// 添加事件处理Handler
	// eventType - 事件类型
	// phandlerFunc - 执行事件回调的全局方法或者静态方法
	template<typename EventStruct>
	void EventProcessor::AddEventHandler( unsigned int eventType, bool (*phandlerFunc)(EventStruct*) )
	{
		EventHandlerWrapperList* pEvtList = NULL;
		EventHandlerMap::iterator it = mEventHandlerMap.find( eventType );
		if ( it == mEventHandlerMap.end() )
		{
			pEvtList = new EventHandlerWrapperList;
			mEventHandlerMap.insert( std::make_pair(eventType, pEvtList) );
		}
		else
			pEvtList = it->second;

		typedef bool (* HandlerFuncPtr)(EventBase*);
		EventHandlerDelegate delegateObj;
		delegateObj.bind((HandlerFuncPtr)phandlerFunc);
		EventHandlerWrapper* pEvtPair = new EventHandlerWrapper(delegateObj);
		pEvtList->push_back( pEvtPair );
	}

	// 添加事件处理Handler
	// eventType - 事件类型
	// phObject - 实行事件回调的对象
	// phandlerFunc - 执行事件回调的类成员函数
	template<typename HandlerObject, typename EventStruct>
	void EventProcessor::AddEventHandler( unsigned int eventType, HandlerObject* phObject, bool (HandlerObject::* phandlerFunc)(EventStruct*) )
	{
		EventHandlerWrapperList* pEvtList = NULL;
		EventHandlerMap::iterator it = mEventHandlerMap.find( eventType );
		if ( it == mEventHandlerMap.end() )
		{
			pEvtList = new EventHandlerWrapperList;
			mEventHandlerMap.insert( std::make_pair(eventType, pEvtList) );
		}
		else
			pEvtList = it->second;

		typedef bool (HandlerObject::* HandlerFuncPtr)(EventBase*);
		EventHandlerWrapper* pEvtPair = new EventHandlerWrapper( EventHandlerDelegate(phObject, (HandlerFuncPtr)phandlerFunc) );
		pEvtList->push_back( pEvtPair );
	}

	// 移除执行事件回调的全局方法或者静态方法
	template<typename EventStruct>
	void EventProcessor::RemoveEventHandler( unsigned int eventType, bool (*phandlerFunc)(EventStruct*) )
	{
		EventHandlerWrapperList* pEvtList = NULL;
		EventHandlerMap::iterator it = mEventHandlerMap.find( eventType );
		if ( it == mEventHandlerMap.end() )
			return;
		pEvtList = it->second;
		typedef bool (*HandlerFuncPtr)(EventBase*);
		EventHandlerDelegate tDelegate;
		tDelegate.bind( (HandlerFuncPtr)phandlerFunc );
		EventHandlerWrapperList::iterator itb = pEvtList->begin(), ite = pEvtList->end();
		for ( ; itb != ite; ++itb )
		{
			EventHandlerWrapper* pPair = *itb;
			if ( pPair && pPair->Equal( tDelegate ) )
			{
				delete pPair;
				pEvtList->erase( itb );
				return;
			}
		}
	}

	// 移除执行事件回调的类成员函数
	template<typename HandlerObject, typename EventStruct>
	void EventProcessor::RemoveEventHandler( unsigned int eventType, HandlerObject* phObject, bool (HandlerObject::* phandlerFunc)(EventStruct*) )
	{
		EventHandlerWrapperList* pEvtList = NULL;
		EventHandlerMap::iterator it = mEventHandlerMap.find( eventType );
		if ( it == mEventHandlerMap.end() )
			return;
		pEvtList = it->second;
		typedef bool (HandlerObject::* HandlerFuncPtr)(EventBase*);
		EventHandlerDelegate tDelegate(phObject, (HandlerFuncPtr)phandlerFunc);
		EventHandlerWrapperList::iterator itb = pEvtList->begin(), ite = pEvtList->end();
		for ( ; itb != ite; ++itb )
		{
			EventHandlerWrapper* pPair = *itb;
			if ( pPair && pPair->Equal( tDelegate ) )
			{
				delete pPair;
				pEvtList->erase( itb );
				return;
			}
		}
	}

	inline void EventProcessor::SetEventAccept( AISDeviceType type, bool bEnable )
	{
		assert( type > AISUnknown && type < AISMaxDevice );
		int value = type<<2;
		bEnable ? (mEventAcceptFlag |= value) : (mEventAcceptFlag &= ~value);
	}

	inline bool EventProcessor::IsEventAccept( AISDeviceType type )
	{
		assert( type > AISUnknown && type < AISMaxDevice );
		int value = type<<2;
		return (mEventAcceptFlag & value) != 0;
	}

	inline const std::string& EventProcessor::GetName() const 
	{
		return mName;
	}
}
