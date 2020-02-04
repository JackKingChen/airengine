// ***************************************************************
//  文件: DebugTools.h
//  日期: 2009-12-11
//  作者: huangjunjie
//  说明: 调试工具
// ***************************************************************


#pragma once


#include "EngineConfig.h"
//#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include "Util.h"
#include "LogStruct.h"

//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

	typedef void (*DebugConsoleCallBackFunc)( const AIRString& szCmd );


	class EngineExport DebugTools
	{
	public:
		//---------------------------------------------------------------------------------------
		//----------------------------------- 日志部分的接口 ------------------------------------
		// 清空日志
		// parm[in] szLogName : 日志名称ID
		static void			ClearLog( AIRString szLogName );

		// 清空当前日志
		static void			ClearLog();

		// 添加一个日志文件
		// parm[in] szName : 日志名称ID
		// parm[in] szFile : 日志文件名
		static void			AddLogFile( AIRString szName, AIRString szFile );

		// 设置当前的日志
		// parm[in] szLogName : 日志名称ID
		static void			SetCurLog( AIRString szLogName );

		static AIRString	GetCurLog( void );

		// 输出信息到日志
		// parm[in] ml : 信息等级
		// parm[in] szLogName : 日志名称ID
		// parm[in] buf : 信息串
		static void			OutPutToLog( LogType ml, AIRString szLogName, char* buf, ... );
		static void			OutPutToLog( AIRString szLogName, char* buf, ... );
		//---------------------------------------------------------------------------------------



		//---------------------------------------------------------------------------------------
		//----------------------------------- 输出信息的接口 ------------------------------------
		// 生成格式化的信息串
		// parm[in] ml : 信息等级
		// parm[in] msg : 信息串
		// parm[in] szFile : 当前的代码文件
		// parm[in] nLine : 第几行调用
		// parm[in] szFunc :哪个函数调用
		static AIRString	GenFormatMsg( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static AIRString	GenFormatMsg( LogType ml, char* msg );

		// 输出信息到日志
		// parm[in] ml : 信息等级
		// parm[in] msg : 信息串
		// parm[in] szFile : 当前的代码文件
		// parm[in] nLine : 第几行调用
		// parm[in] szFunc :哪个函数调用
		static void			OutPutToLog( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void         OutPutToLog( LogType ml, char* msg );

		// 输出信息到控制台
		// parm[in] ml : 信息等级
		// parm[in] msg : 信息串
		// parm[in] szFile : 当前的代码文件
		// parm[in] nLine : 第几行调用
		// parm[in] szFunc :哪个函数调用
		static void			OutPutToConsole( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void         OutPutToConsole( LogType ml, char* msg );

		// 输出信息到调试窗口
		// parm[in] ml : 信息等级
		// parm[in] msg : 信息串
		// parm[in] szFile : 当前的代码文件
		// parm[in] nLine : 第几行调用
		// parm[in] szFunc :哪个函数调用
		static void			OutPutToDebugString( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void			OutPutToDebugString( LogType ml, char* msg );

		// 输出信息到消息对话框
		// parm[in] ml : 信息等级
		// parm[in] msg : 信息串
		// parm[in] szFile : 当前的代码文件
		// parm[in] nLine : 第几行调用
		// parm[in] szFunc :哪个函数调用
		static void			OutPutToMsgBox( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void			OutPutToMsgBox( LogType ml, char* msg );
		//---------------------------------------------------------------------------------------


#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		//---------------------------------------------------------------------------------------
		//--------------------------------- 与控制台交互的接口 ----------------------------------
		// 打开调试控制台(外部APPLICATION调用)
		// hwndCaller - 调用控制台的窗口
		static void			StartDebugConsoleWindow( HWND hwndCaller );

		// 关闭调试控制台(外部APPLICATION调用)
		static void			ShutDebugConsoleWindow( void );

		// 往控制台输出信息(外部APPLICATION调用)
		// LogType - 日志类型
		// text - 日志信息
		static void			OutPutToDebugConsoleWindow( LogType lt, const AIRString& text );

		// 从控制台获取输入的指令(windows消息调用)
		// pCopyDataStruct - 进程之间传输的数据
		static void			OnCopyData( COPYDATASTRUCT* pCopyDataStruct );

		// 注册控制台回调函数(外部APPLICATION调用)
		static void         RegDebugConsoleCallBackFunc( DebugConsoleCallBackFunc func );

		static HWND	                            m_HWndCaller;					//调用控制台的进程窗口handle
		static DebugConsoleCallBackFunc         m_DebugConsoleCallBackFunc;		//处理控制台输入的消息回调
		//---------------------------------------------------------------------------------------
#endif


		static void         FailedAssert( char* file, long line, char* func, char* statement );


	private:
		static std::map<AIRString, AIRString>   m_LogFileMap;					//日志文件映射表
		static AIRString                        m_CurLogFile;					//当前的日志文件


	}; //! end class DebugTools


	//---------------------------------------------------------------------------------------
	// 添加一个日志文件
	// name : 日志名称id
	// file : 日志路径
	#ifndef DT_ADD_LOG
	#define DT_ADD_LOG(name,file)			{ DebugTools::AddLogFile(name, file);   }
	#endif

	// 清空日志文件
	// name : 日志名称id
	#ifndef DT_CLR_LOG
	#define DT_CLR_LOG(name)				{ DebugTools::ClearLog(name);           }
	#endif

	// 清空当前的日志
	#ifndef DT_CLR_CUR_LOG
	#define DT_CLR_CUR_LOG()				{ DebugTools::ClearLog();               }
	#endif

	// 设置某个日志为当前日志
	#ifndef DT_SET_LOG
	#define DT_SET_LOG(name)				{ DebugTools::SetCurLog(name);          }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// 将格式化的信息写入日志
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_LOG_FMT
	#define DT_TO_LOG_FMT(lt,msg)			{ DebugTools::OutPutToLog( lt, msg, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// 将信息写入日志
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_LOG
	#define DT_TO_LOG(lt,msg)				{ DebugTools::OutPutToLog( lt, msg ); }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// 将格式化的信息写入控制台
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_CONSOLE_FMT
	#define DT_TO_CONSOLE_FMT(lt,msg)		{ DebugTools::OutPutToConsole( lt, msg, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// 将信息写入控制台
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_CONSOLE
	#define DT_TO_CONSOLE(lt,msg)			{ DebugTools::OutPutToConsole( lt, msg ); }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// 将格式化的信息写入调试窗口
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_DBGSTR_FMT
	#define DT_TO_DBGSTR_FMT(lt,msg)		{ DebugTools::OutPutToDebugString( lt, msg, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// 将信息写入调试窗口
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_DBGSTR
	#define DT_TO_DBGSTR(lt,msg)			{ DebugTools::OutPutToDebugString( lt, msg ); }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// 将格式化的信息写入消息框
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_MSGBOX_FMT
	#define DT_TO_MSGBOX_FMT(lt,msg)		{ DebugTools::OutPutToMsgBox( lt, msg ); } //, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// 将信息写入消息框
	// lt : 信息级别
	// msg : 信息内容
	#ifndef DT_TO_MSGBOX
	#define DT_TO_MSGBOX(lt,msg)			{ DebugTools::OutPutToMsgBox( lt, msg ); }
	#endif
	//---------------------------------------------------------------------------------------

	#ifndef ASSERT
	#define ASSERT( equation )				{ if( !(equation) ) {DebugTools::FailedAssert(__FILE__, __LINE__, __FUNCTION__, #equation);} }	
	#endif

	#ifndef ASSERT_MSG
	#define ASSERT_MSG( equation, msg )		{ if( !(equation) ) {DebugTools::FailedAssert(__FILE__, __LINE__, __FUNCTION__, msg);} }
	#endif

	#ifndef ASSERT_RET
	#define ASSERT_RET( equation, ret )		{ if( !(equation) ) {return (ret);} }
	#endif

//#else
//
//	#ifndef DT_ADD_LOG
//	#define DT_ADD_LOG(name,file)	    
//	#endif
//
//	// 清空日志文件
//	// name : 日志名称id
//	#ifndef DT_CLR_LOG
//	#define DT_CLR_LOG(name)		    
//	#endif
//
//	// 清空当前的日志
//	#ifndef DT_CLR_CUR_LOG
//	#define DT_CLR_CUR_LOG()		    
//	#endif
//
//	// 设置某个日志为当前日志
//	#ifndef DT_SET_LOG
//	#define DT_SET_LOG(name)		    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// 将格式化的信息写入日志
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_LOG_FMT
//	#define DT_TO_LOG_FMT(lt,msg)		
//	#endif
//
//	// 将信息写入日志
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_LOG
//	#define DT_TO_LOG(lt,msg)		    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// 将格式化的信息写入控制台
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_CONSOLE_FMT
//	#define DT_TO_CONSOLE_FMT(lt,msg)	
//	#endif
//
//	// 将信息写入控制台
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_CONSOLE
//	#define DT_TO_CONSOLE(lt,msg)	    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// 将格式化的信息写入调试窗口
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_DBGSTR_FMT
//	#define DT_TO_DBGSTR_FMT(lt,msg)	
//	#endif
//
//	// 将信息写入调试窗口
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_DBGSTR
//	#define DT_TO_DBGSTR(lt,msg)	    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// 将格式化的信息写入消息框
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_MSGBOX_FMT
//	#define DT_TO_MSGBOX_FMT(lt,msg)	
//	#endif
//
//	// 将信息写入消息框
//	// lt : 信息级别
//	// msg : 信息内容
//	#ifndef DT_TO_MSGBOX
//	#define DT_TO_MSGBOX(lt,msg)	    
//	#endif
//
//#endif
