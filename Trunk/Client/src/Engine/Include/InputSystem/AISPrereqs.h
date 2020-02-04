
#pragma once


//------------------------------------------------------------------
// Common STL Containers
#include <vector>
#include <string>
#include <map>
#include <list>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdarg.h>

#include <exception>

#include <math.h>


//------------------------------------------------------------------
// Determine Compiler
#if defined( _MSC_VER )
#	define AIS_MSVC_COMPILER
#   pragma message("AIS_MSVC_COMPILER")
#elif defined( __GNUC__ )
#	if defined( __WIN32__ ) || defined( _WIN32 )
#		define AIS_MINGW_COMPILER
#       pragma message("AIS_MINGW_COMPILER")
#	else
#		define AIS_GCC_COMPILER
#       pragma message("AIS_GCC_COMPILER")
#	endif
#elif defined( __BORLANDC__ )
#	define AIS_BORLAND_COMPILER
#   pragma message("AIS_BORLAND_COMPILER")
#else
#	error No Recognized Compiler!
#endif

//------------------------------------------------------------------
// Determine Operating System Platform 
#define AIR_INPUT_API
#if defined( __WIN32__ ) || defined( _WIN32 ) // Windows 2000, XP, ETC
#	if defined ( _XBOX )
#		define AIS_XBOX_PLATFORM
#       pragma message("AIS_XBOX_PLATFORM")
#	else
#		define AIS_WIN32_PLATFORM
#		ifndef AIS_STATIC_LIB
#			undef AIR_INPUT_API
#           if !defined(AIS_MINGW_COMPILER)
#			    pragma warning (disable : 4251)
#           endif
#			if defined( AIS_NONCLIENT_BUILD )
#				define AIR_INPUT_API __declspec( dllexport )
#			else
#               if defined(AIS_MINGW_COMPILER)
#                   define AIR_INPUT_API
#               else
#				    define AIR_INPUT_API __declspec( dllimport )
#               endif
#			endif
#		endif
#	endif
#elif defined( __APPLE_CC__ ) // Apple OS X
//	ios 4.0以上版本
#   if __IPHONE_OS_VERSION_MIN_REQUIRED >= 40300
#       define AIS_IPHONE_PLATFORM
#       pragma message("AIS_IPHONE_PLATFORM")
//  Leopard OS以上版本 
#   elif __MAC_OS_X_VERSION_MIN_REQUIRED >= 1050 
#       define AIS_APPLE_PLATFORM
#       pragma message("AIS_APPLE_PLATFORM")
#   endif
#   undef AIR_INPUT_API
#   define AIR_INPUT_API __attribute__((visibility("default")))
#   pragma message("export for mac")
#elif defined( ANDROID )
#	define AIS_ANDROID_PLATFORM
#   pragma message("AIS_ANDROID_PLATFORM")
#else //Probably Linux
#	define AIS_LINUX_PLATFORM
#   pragma message("AIS_LINUX_PLATFORM")
#	include <unistd.h>
#endif

#include "PlatformConfig.h"

#undef	AIR_INPUT_API
#define AIR_INPUT_API	AIR_API


//------------------------------------------------------------------
//Is Processor 32 or 64 bits...
#if defined(__x86_64__)
#	define AIS_ARCH_64
#   pragma message("AIS_ARCH_64")
#else
#	define AIS_ARCH_32
#   pragma message("AIS_ARCH_32")
#endif



//------------------------------------------------------------------
#define AIS_VERSION_MAJOR 1
#define AIS_VERSION_MINOR 0
#define AIS_VERSION_PATCH 0
#define AIS_VERSION_NAME "1.0.0"
#define AIS_VERSION ((AIS_VERSION_MAJOR << 16) | (AIS_VERSION_MINOR << 8) | AIS_VERSION_PATCH)




namespace AIS
{
	class InputManager;
	class InputSystem;
	class FactoryCreator;
	class DeviceObject;
	class Keyboard;
	class Mouse;
	class JoyStick;
	class MultiTouch;
	class Sensor;
	class KeyListener;
	class MouseListener;
	class MultiTouchListener;
	class JoyStickListener;
	class SensorListener;
	class Interface;
	class ForceFeedback;
	class Effect;
	class Exception;

	typedef std::multimap<std::string, std::string>		ParamList;
	typedef std::vector<FactoryCreator*>				FactoryList;
	typedef std::map<DeviceObject*, FactoryCreator*>	FactoryCreatedObject;

	enum AISDeviceType
	{
		AISUnknown       = 0, 
		AISKeyboard      = 1, 
		AISMouse         = 2, 
		AISJoyStick      = 3, 
		AISMultiTouch    = 4,
		AISSensor		 = 5, 
		AISMaxDevice	
	};

	typedef std::multimap<AISDeviceType, std::string>	DeviceList;


	//------------------------------------------------------------------
	// Shared common components

	enum ComponentType
	{
		AIS_Unknown = 0,
		AIS_Button  = 1, //ie. Key, mouse button, joy button, etc
		AIS_Axis    = 2, //ie. A joystick or mouse axis
		AIS_Slider  = 3, //
		AIS_POV     = 4, //ie. Arrow direction keys
		AIS_Vector3 = 5  //ie. WiiMote orientation
	};

	class AIR_INPUT_API Component
	{
	public:
		Component() : cType(AIS_Unknown) {};
		Component(ComponentType type) : cType(type) {};

	public:
		ComponentType cType;
	};


	class AIR_INPUT_API Button : public Component
	{
	public:
		Button() : Component(AIS_Button), pushed(false) {}
		Button(bool bPushed) : Component(AIS_Button), pushed(bPushed) {}

	public:
		bool pushed;
	};


	class AIR_INPUT_API Axis : public Component
	{
	public:
		Axis() : Component(AIS_Axis), m_AbsVal(0), m_RelVal(0), m_AbsOnly(false) {};
		void clear() { m_AbsVal = m_RelVal = 0; }

	public:
		int		m_AbsVal;
		int		m_RelVal;
		bool	m_AbsOnly;
	};

	class AIR_INPUT_API Vector3 : public Component
	{
	public:
		Vector3() : Component(AIS_Vector3), x(0.0f), y(0.0f), z(0.0f) {}
		Vector3(float _x, float _y, float _z) : Component(AIS_Vector3), x(_x), y(_y), z(_z) {};
		void Set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
		void clear() { x = y = z = 0.0f; }

	public:
		float x;
		float y;
		float z;
	};


	//------------------------------------------------------------------

	enum EventType
	{
		ET_Unknown = 0,					// 未定义

		ET_KeyDown,						// 键盘 - 按下按键
		ET_KeyUp,						// 键盘 - 松开按键

		ET_MouseMove,					// 鼠标 - 移动
		ET_MouseButtonDown,				// 鼠标 - 按下按键
		ET_MouseButtonUp,				// 鼠标 - 松开按键
		ET_MouseButtonDBClick,			// 鼠标 - 双击按键

		ET_JoyStickButtonDown,			// 手柄 - 按下手柄上的某个按键
		ET_JoyStickButtonUp,			// 手柄 - 松开手柄上的某个按键
		ET_JoyStickSliderMove,			// 手柄 - Slider Axis发生变化
		ET_JoyStickAxisMove,			// 手柄 - 摇杆发生变化
		ET_JoyStickPovMove,				// 手柄 - 十字方向键发生变化
		ET_JoyStickVector3Move,			// 手柄 - Vector type controls the device exports

		ET_MultiTouchesPress,			// 触摸屏 - 手指按下
		ET_MultiTouchesRelease,			// 触摸屏 - 手指松开
		ET_MultiTouchesCancle,			// 触摸屏 - 手指移动到屏幕外,取消触摸
		ET_MultiTouchesMove,			// 触摸屏 - 手指移动

		ET_SensorGravityChanged,		// 感应器 - 重力感应发生改变
		ET_SensorOrientationChanged,	// 感应器 - 屏幕旋转

		ET_UserDef
	};

	//--------------------------------------------------------------------------------------------------

	// 用户数据包装
	class AIR_INPUT_API UserDataVariant
	{
	public:
		enum UserDataVariantType
		{
			UDVT_NULL = 0,
			UDVT_BOOL,
			UDVT_INT,
			UDVT_FLOAT,
			UDVT_OBJECT,
			UDVT_STRING,
			UDVT_MAX
		};

		UserDataVariant() : m_Type(UDVT_NULL)							{}
		UserDataVariant(bool b) : m_Type(UDVT_BOOL), m_b(b)				{}
		UserDataVariant(int i) : m_Type(UDVT_INT), m_i(i)				{}
		UserDataVariant(float f) : m_Type(UDVT_FLOAT), m_f(f)			{}
		UserDataVariant(void* p) : m_Type(UDVT_OBJECT), m_ptr(p)		{}
		UserDataVariant(const char* s) : m_Type(UDVT_STRING), m_str(s)	{}
		UserDataVariant(UserDataVariant& other)							{ Set(&other); }
		~UserDataVariant()												{}

		bool operator == ( UserDataVariant& rhs )
		{
			if ( rhs.GetType() != m_Type )
				return false;
			switch( rhs.GetType() )
			{
			case UDVT_BOOL:		return m_b == rhs.GetBool(); 
			case UDVT_INT:		return m_i == rhs.GetInt(); 
			case UDVT_FLOAT:	return m_f == rhs.GetFloat(); 
			case UDVT_STRING:	return m_str == rhs.m_str; 
			case UDVT_OBJECT:	return m_ptr == rhs.GetObject();
			default:            return false;
			}
		}

		void Set(UserDataVariant* rhs)
		{
			if ( !rhs )
				return;
			switch( rhs->GetType() )
			{
			case UDVT_BOOL:		m_b = rhs->GetBool();		break;
			case UDVT_INT:		m_i = rhs->GetInt();		break;
			case UDVT_FLOAT:	m_f = rhs->GetFloat();		break;
			case UDVT_STRING:	m_str = rhs->m_str;			break;
			case UDVT_OBJECT:	m_ptr = rhs->GetObject();	break;
			default: break;
			}
			m_Type = rhs->GetType();
		}

		template<typename ValueType>
		void Set(UserDataVariantType type, ValueType value)
		{
			switch (type)
			{
			case UDVT_BOOL:     SetBool(value);                 break;
			case UDVT_INT:      SetInt(value);                  break;
			case UDVT_FLOAT:    SetFloat(value);                break;
			case UDVT_OBJECT:   SetObject((void*)value);        break;
			case UDVT_STRING:   SetString((const char*)value);  break;
			default:
				break;
			}
		}

		void SetBool(bool b)
		{
			m_Type = UDVT_BOOL;
			m_b = false;
		}
		void SetInt(int i)
		{
			m_Type = UDVT_INT;
			m_i = 0;
		}
		void SetFloat(float f)
		{
			m_Type = UDVT_FLOAT;
			m_f = 0.0f;
		}
		void SetString(const char* s)
		{
			m_Type = UDVT_STRING;
			m_str = s;
		}
		void SetObject(void* p)
		{
			m_Type = UDVT_OBJECT;
			m_ptr = NULL;
		}

		bool GetBool()					{ return m_b; }
		int  GetInt()					{ return m_i; }
		float GetFloat()				{ return m_f; }
		const char* GetString()			{ return m_str.c_str(); }
		void* GetObject()				{ return m_ptr; }
		UserDataVariantType GetType()	{ return m_Type; }

	protected:
		UserDataVariantType	m_Type;
		union
		{
			bool		m_b;
			int			m_i;
			float		m_f;			
			void*		m_ptr;
		};
		std::string m_str;
	};

	//--------------------------------------------------------------------------------------------------

	class AIR_INPUT_API EventBase
	{
	public:
		EventBase() : mEventType(ET_Unknown), mpEventSender(NULL)
		{
		}
		EventBase( unsigned int evtType, void* pSender )
			: mEventType(evtType), mpEventSender(pSender)
		{
		}
		virtual ~EventBase()
		{
		}

		template<typename ValueType>
		void Set(unsigned int evtType, void* pSender, int valuetype, ValueType value)
		{
			mEventType      = evtType;
			mpEventSender   = pSender;
			mEventParam.Set<ValueType>((UserDataVariant::UserDataVariantType)valuetype, value);
		}

		unsigned int				GetEventType()		const	{ return mEventType;	}
		void*						GetEventSender()	const	{ return mpEventSender; }
		const UserDataVariant&		GetEventParm()		const	{ return mEventParam;	}

		template<class T>
		T*							GetCastEvent()		const	{ return (T*)this;		}

		template<class T>			
		T*							GetCastEventSender() const	{ return (T*)mpEventSender; }

	protected:
		unsigned int		mEventType;		// 事件类型
		void*				mpEventSender;	// 事件发送者
		UserDataVariant		mEventParam;	// 事件参数
	};


	//------------------------------------------------------------------

	enum AIS_ERROR
	{
		E_InputDisconnected = 0,
		E_InputDeviceNonExistant,
		E_InputDeviceNotSupported,
		E_DeviceFull,
		E_NotSupported,
		E_NotImplemented,
		E_Duplicate,
		E_InvalidParam,
		E_General
	};

	inline const char* GetErrString( int err )
	{
		static const std::string G_ERR_INFOS[] = 
		{
			"E_InputDisconnected", 
			"E_InputDeviceNonExistant", 
			"E_InputDeviceNotSupported", 
			"E_DeviceFull", 
			"E_NotSupported", 
			"E_NotImplemented", 
			"E_Duplicate", 
			"E_InvalidParam", 
			"E_General"
			"UNKNOWN"
		};
		if ( err < 0 || err > E_General )
			return G_ERR_INFOS[9].c_str();
		return G_ERR_INFOS[err].c_str();
	}


	class AIR_INPUT_API Exception : public std::exception
	{
		Exception() : m_eType(E_General), m_eLine(0), m_eFile(0) {}
	public:
		Exception( AIS_ERROR err, const char* str, int line, const char *file )
			: m_eType(err), m_eLine(line), m_eFile(file), m_eText(str) {}

		~Exception() throw() {}

		virtual const char* what() const throw() { return m_eText; }

		const AIS_ERROR m_eType;
		const int		m_eLine;
		const char*		m_eFile;
		const char*		m_eText;
	};

	inline std::string FormatString(const char* pstr, ...)
	{
//      This will use to match CPU and is for log useless
//		va_list	argptr;
//		char tmp[2048];
//		va_start( argptr, pstr );
//		vsnprintf(tmp, 2048, pstr, argptr);
//		va_end( argptr );
//		return std::string(tmp);
        return "";
    }

	inline void ExceptReport( AIS_ERROR err, const char* str, const char* file, int line)
	{
		std::string info = FormatString( "[ErrCode : %s, ErrInfo : %s, At File %s, Line %d\n", GetErrString(err), str, file, line);
		//std::cout<< info.c_str();
		//throw( AIS::Exception(err, str, line, file) );
	}

	//! Use this macro to handle exceptions easily
	//#define AIS_EXCEPT( err, str ) throw( AIS::Exception(err, str, __LINE__, __FILE__) )
	//#define AIS_EXCEPT( err, str ) std::cout<<"Error : "<<str<<" at File : "<<__FILE__<<" at Line : "<<__LINE__<<"\n"
	#define AIS_EXCEPT( err, str ) ExceptReport(err, str, __FILE__, __LINE__)


};
