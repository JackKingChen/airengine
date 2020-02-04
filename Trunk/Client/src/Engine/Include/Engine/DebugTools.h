// ***************************************************************
//  �ļ�: DebugTools.h
//  ����: 2009-12-11
//  ����: huangjunjie
//  ˵��: ���Թ���
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
		//----------------------------------- ��־���ֵĽӿ� ------------------------------------
		// �����־
		// parm[in] szLogName : ��־����ID
		static void			ClearLog( AIRString szLogName );

		// ��յ�ǰ��־
		static void			ClearLog();

		// ���һ����־�ļ�
		// parm[in] szName : ��־����ID
		// parm[in] szFile : ��־�ļ���
		static void			AddLogFile( AIRString szName, AIRString szFile );

		// ���õ�ǰ����־
		// parm[in] szLogName : ��־����ID
		static void			SetCurLog( AIRString szLogName );

		static AIRString	GetCurLog( void );

		// �����Ϣ����־
		// parm[in] ml : ��Ϣ�ȼ�
		// parm[in] szLogName : ��־����ID
		// parm[in] buf : ��Ϣ��
		static void			OutPutToLog( LogType ml, AIRString szLogName, char* buf, ... );
		static void			OutPutToLog( AIRString szLogName, char* buf, ... );
		//---------------------------------------------------------------------------------------



		//---------------------------------------------------------------------------------------
		//----------------------------------- �����Ϣ�Ľӿ� ------------------------------------
		// ���ɸ�ʽ������Ϣ��
		// parm[in] ml : ��Ϣ�ȼ�
		// parm[in] msg : ��Ϣ��
		// parm[in] szFile : ��ǰ�Ĵ����ļ�
		// parm[in] nLine : �ڼ��е���
		// parm[in] szFunc :�ĸ���������
		static AIRString	GenFormatMsg( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static AIRString	GenFormatMsg( LogType ml, char* msg );

		// �����Ϣ����־
		// parm[in] ml : ��Ϣ�ȼ�
		// parm[in] msg : ��Ϣ��
		// parm[in] szFile : ��ǰ�Ĵ����ļ�
		// parm[in] nLine : �ڼ��е���
		// parm[in] szFunc :�ĸ���������
		static void			OutPutToLog( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void         OutPutToLog( LogType ml, char* msg );

		// �����Ϣ������̨
		// parm[in] ml : ��Ϣ�ȼ�
		// parm[in] msg : ��Ϣ��
		// parm[in] szFile : ��ǰ�Ĵ����ļ�
		// parm[in] nLine : �ڼ��е���
		// parm[in] szFunc :�ĸ���������
		static void			OutPutToConsole( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void         OutPutToConsole( LogType ml, char* msg );

		// �����Ϣ�����Դ���
		// parm[in] ml : ��Ϣ�ȼ�
		// parm[in] msg : ��Ϣ��
		// parm[in] szFile : ��ǰ�Ĵ����ļ�
		// parm[in] nLine : �ڼ��е���
		// parm[in] szFunc :�ĸ���������
		static void			OutPutToDebugString( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void			OutPutToDebugString( LogType ml, char* msg );

		// �����Ϣ����Ϣ�Ի���
		// parm[in] ml : ��Ϣ�ȼ�
		// parm[in] msg : ��Ϣ��
		// parm[in] szFile : ��ǰ�Ĵ����ļ�
		// parm[in] nLine : �ڼ��е���
		// parm[in] szFunc :�ĸ���������
		static void			OutPutToMsgBox( LogType ml, char* msg, char* szFile, int nLine, char* szFunc );
		static void			OutPutToMsgBox( LogType ml, char* msg );
		//---------------------------------------------------------------------------------------


#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		//---------------------------------------------------------------------------------------
		//--------------------------------- �����̨�����Ľӿ� ----------------------------------
		// �򿪵��Կ���̨(�ⲿAPPLICATION����)
		// hwndCaller - ���ÿ���̨�Ĵ���
		static void			StartDebugConsoleWindow( HWND hwndCaller );

		// �رյ��Կ���̨(�ⲿAPPLICATION����)
		static void			ShutDebugConsoleWindow( void );

		// ������̨�����Ϣ(�ⲿAPPLICATION����)
		// LogType - ��־����
		// text - ��־��Ϣ
		static void			OutPutToDebugConsoleWindow( LogType lt, const AIRString& text );

		// �ӿ���̨��ȡ�����ָ��(windows��Ϣ����)
		// pCopyDataStruct - ����֮�䴫�������
		static void			OnCopyData( COPYDATASTRUCT* pCopyDataStruct );

		// ע�����̨�ص�����(�ⲿAPPLICATION����)
		static void         RegDebugConsoleCallBackFunc( DebugConsoleCallBackFunc func );

		static HWND	                            m_HWndCaller;					//���ÿ���̨�Ľ��̴���handle
		static DebugConsoleCallBackFunc         m_DebugConsoleCallBackFunc;		//�������̨�������Ϣ�ص�
		//---------------------------------------------------------------------------------------
#endif


		static void         FailedAssert( char* file, long line, char* func, char* statement );


	private:
		static std::map<AIRString, AIRString>   m_LogFileMap;					//��־�ļ�ӳ���
		static AIRString                        m_CurLogFile;					//��ǰ����־�ļ�


	}; //! end class DebugTools


	//---------------------------------------------------------------------------------------
	// ���һ����־�ļ�
	// name : ��־����id
	// file : ��־·��
	#ifndef DT_ADD_LOG
	#define DT_ADD_LOG(name,file)			{ DebugTools::AddLogFile(name, file);   }
	#endif

	// �����־�ļ�
	// name : ��־����id
	#ifndef DT_CLR_LOG
	#define DT_CLR_LOG(name)				{ DebugTools::ClearLog(name);           }
	#endif

	// ��յ�ǰ����־
	#ifndef DT_CLR_CUR_LOG
	#define DT_CLR_CUR_LOG()				{ DebugTools::ClearLog();               }
	#endif

	// ����ĳ����־Ϊ��ǰ��־
	#ifndef DT_SET_LOG
	#define DT_SET_LOG(name)				{ DebugTools::SetCurLog(name);          }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// ����ʽ������Ϣд����־
	// lt : ��Ϣ����
	// msg : ��Ϣ����
	#ifndef DT_TO_LOG_FMT
	#define DT_TO_LOG_FMT(lt,msg)			{ DebugTools::OutPutToLog( lt, msg, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// ����Ϣд����־
	// lt : ��Ϣ����
	// msg : ��Ϣ����
	#ifndef DT_TO_LOG
	#define DT_TO_LOG(lt,msg)				{ DebugTools::OutPutToLog( lt, msg ); }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// ����ʽ������Ϣд�����̨
	// lt : ��Ϣ����
	// msg : ��Ϣ����
	#ifndef DT_TO_CONSOLE_FMT
	#define DT_TO_CONSOLE_FMT(lt,msg)		{ DebugTools::OutPutToConsole( lt, msg, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// ����Ϣд�����̨
	// lt : ��Ϣ����
	// msg : ��Ϣ����
	#ifndef DT_TO_CONSOLE
	#define DT_TO_CONSOLE(lt,msg)			{ DebugTools::OutPutToConsole( lt, msg ); }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// ����ʽ������Ϣд����Դ���
	// lt : ��Ϣ����
	// msg : ��Ϣ����
	#ifndef DT_TO_DBGSTR_FMT
	#define DT_TO_DBGSTR_FMT(lt,msg)		{ DebugTools::OutPutToDebugString( lt, msg, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// ����Ϣд����Դ���
	// lt : ��Ϣ����
	// msg : ��Ϣ����
	#ifndef DT_TO_DBGSTR
	#define DT_TO_DBGSTR(lt,msg)			{ DebugTools::OutPutToDebugString( lt, msg ); }
	#endif
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	// ����ʽ������Ϣд����Ϣ��
	// lt : ��Ϣ����
	// msg : ��Ϣ����
	#ifndef DT_TO_MSGBOX_FMT
	#define DT_TO_MSGBOX_FMT(lt,msg)		{ DebugTools::OutPutToMsgBox( lt, msg ); } //, __FILE__, __LINE__, __FUNCTION__ ); }
	#endif

	// ����Ϣд����Ϣ��
	// lt : ��Ϣ����
	// msg : ��Ϣ����
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
//	// �����־�ļ�
//	// name : ��־����id
//	#ifndef DT_CLR_LOG
//	#define DT_CLR_LOG(name)		    
//	#endif
//
//	// ��յ�ǰ����־
//	#ifndef DT_CLR_CUR_LOG
//	#define DT_CLR_CUR_LOG()		    
//	#endif
//
//	// ����ĳ����־Ϊ��ǰ��־
//	#ifndef DT_SET_LOG
//	#define DT_SET_LOG(name)		    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// ����ʽ������Ϣд����־
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_LOG_FMT
//	#define DT_TO_LOG_FMT(lt,msg)		
//	#endif
//
//	// ����Ϣд����־
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_LOG
//	#define DT_TO_LOG(lt,msg)		    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// ����ʽ������Ϣд�����̨
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_CONSOLE_FMT
//	#define DT_TO_CONSOLE_FMT(lt,msg)	
//	#endif
//
//	// ����Ϣд�����̨
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_CONSOLE
//	#define DT_TO_CONSOLE(lt,msg)	    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// ����ʽ������Ϣд����Դ���
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_DBGSTR_FMT
//	#define DT_TO_DBGSTR_FMT(lt,msg)	
//	#endif
//
//	// ����Ϣд����Դ���
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_DBGSTR
//	#define DT_TO_DBGSTR(lt,msg)	    
//	#endif
//	//---------------------------------------------------------------------------------------
//
//
//	//---------------------------------------------------------------------------------------
//	// ����ʽ������Ϣд����Ϣ��
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_MSGBOX_FMT
//	#define DT_TO_MSGBOX_FMT(lt,msg)	
//	#endif
//
//	// ����Ϣд����Ϣ��
//	// lt : ��Ϣ����
//	// msg : ��Ϣ����
//	#ifndef DT_TO_MSGBOX
//	#define DT_TO_MSGBOX(lt,msg)	    
//	#endif
//
//#endif
