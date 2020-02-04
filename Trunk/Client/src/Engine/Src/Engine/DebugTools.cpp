#include "stdafx.h"
#include "DebugTools.h"
#include "Timer.h"

//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

#include <assert.h>

std::map<AIRString, AIRString>		DebugTools::m_LogFileMap;   
AIRString							DebugTools::m_CurLogFile;



static const AIRString				s_arLogTypeString[6] = 
{
    AIRString( "<< ComMsg : " ),
    AIRString( "<< DbgMsg : " ),
    AIRString( "<< CmdMsg : " ),
    AIRString( "<< WrnMsg : " ),
    AIRString( "<< ErrMsg : " ),
    AIRString( "<< FatMsg : " ),
};


void PrintAndroidLog( LogType ml, const char* msg )
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
	switch( ml )
	{
	case LT_ERROR:
	case LT_FATAL:
		DroidLogE( "%s", msg ); break;

	case LT_WARN:
		DroidLogW( "%s", msg ); break;

	case LT_DEBUG:
		DroidLogD( "%s", msg ); break;

	case LT_INFO:
	case LT_COMMAND:
	default:
		DroidLogI( "%s", msg ); break;
	}
#endif
}




void DebugTools::ClearLog( AIRString szLogName )
{
	std::map<AIRString, AIRString>::iterator it = m_LogFileMap.find( szLogName );
	if ( it == m_LogFileMap.end() )
	{
		return;
	}
	AIRString fileName = it->second;
	std::ofstream fout;
	fout.open( fileName.c_str(), std::ios::out );
	//fout<<"*********************************************************************"<<std::endl;
	//fout<<"********<<                   ��     ־                    >>*********"<<std::endl;
	//fout<<"*********************************************************************"<<std::endl;
	fout<<std::flush;
	fout.close();
}

void DebugTools::ClearLog()
{
	if ( m_CurLogFile.empty() )
	{
		return;
	}
	std::ofstream fout;
	fout.open( m_CurLogFile.c_str() ,std::ios::out );
	//fout<<"*********************************************************************"<<std::endl;
	//fout<<"********<<                   ��     ־                    >>*********"<<std::endl;
	//fout<<"*********************************************************************"<<std::endl;
	fout<<std::flush;
	fout.close();
}

void DebugTools::AddLogFile( AIRString szName, AIRString szFile )
{
	m_LogFileMap.insert( std::make_pair( szName, szFile ) );
	m_CurLogFile = szFile;
}

void DebugTools::SetCurLog( AIRString szLogName )
{
	std::map<AIRString, AIRString>::iterator it = m_LogFileMap.find( szLogName );
	if ( it != m_LogFileMap.end() )
	{
		m_CurLogFile = it->second;
	}
}

AIRString DebugTools::GetCurLog( void )
{
	return m_CurLogFile;
}

void DebugTools::OutPutToLog( LogType ml, AIRString szLogName, char* buf, ... )
{
	std::map<AIRString, AIRString>::iterator it = m_LogFileMap.find( szLogName );
	if ( it == m_LogFileMap.end() )
	{
		return;
	}
	AIRString fileName = it->second;

	double curTime = Timer::GetTime();
	AIRString szTime = GetString( "[%.3f] - ", curTime );
	//SYSTEMTIME time;
	//GetSystemTime( &time );
	//szTime = GetString( 
	//	"[%d�� - %d�� - %d�� - %dʱ - %d�� - %d�� - %d����] - ", 
	//	time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds );

	va_list   args;
	int       nBuf;
	char     szBuffer[4096] = { 0 };
	va_start(args, buf);
	nBuf = vsnprintf( szBuffer, sizeof(szBuffer)*sizeof(char), buf, args );
	assert( nBuf > 0 );
	va_end(args);

	std::ofstream fout;
	AIRString   result;
	// ���ļ�
	fout.open( fileName.c_str() , std::ios::app );
	result = szTime;

    result = s_arLogTypeString[ml] + szTime + szBuffer;

	// д���ļ�
	fout<<result.c_str()<<"\n";		
	// ��д����������д���������ڱ����ļ��򿪵�����±����ļ���
	fout<<std::flush;	
	// �ر�	
	fout.close();	

	PrintAndroidLog( ml, result.c_str() );
}


void DebugTools::OutPutToLog( AIRString szLogName, char* buf, ... )
{
	std::map<AIRString, AIRString>::iterator it = m_LogFileMap.find( szLogName );
	if ( it == m_LogFileMap.end() )
	{
		return;
	}
	AIRString fileName = it->second;

	va_list   args;
	int       nBuf;
	char     szBuffer[4096] = { 0 };
	va_start(args, buf);
	nBuf = vsnprintf( szBuffer, sizeof(szBuffer)*sizeof(char), buf, args );
	assert( nBuf > 0 );
	va_end(args);

	std::ofstream fout;
	//AIRString result = szBuffer;
	// ���ļ�
	fout.open( fileName.c_str() , std::ios::app );
	
	// д���ļ�
	fout<<szBuffer<<"\n";		
	// ��д����������д���������ڱ����ļ��򿪵�����±����ļ���
	fout<<std::flush;	
	// �ر�	
	fout.close();	
}


AIRString DebugTools::GenFormatMsg( LogType ml, char* msg, char* szFile, int nLine, char* szFunc )
{
	AIRString szMsg = msg;
	AIRString str("<!=================================================\n");
	if ( ml != LT_NONE )
	{
		str += s_arLogTypeString[ml];
	}
    str += "-------->\n";

	str += GetString( "File:\t%s\nLine:\t%d\nFunc:\t%s\nMsg:\t%s\n", szFile, nLine, szFunc, szMsg.c_str() );
	str += "<!=================================================\n";
	return str;
}


AIRString DebugTools::GenFormatMsg( LogType ml, char* msg )
{
	AIRString str("");
	if ( ml != LT_NONE )
	{
		str += s_arLogTypeString[ml];
	}
    str += msg;
    return str;
}



void DebugTools::OutPutToLog( LogType ml, char* msg, char* szFile, int nLine, char* szFunc )
{
	if ( m_CurLogFile.empty() )
	{
		return;
	}
	std::ofstream fout;
	AIRString   result = GenFormatMsg( ml, msg, szFile, nLine, szFunc );
	// ���ļ�
	fout.open( m_CurLogFile.c_str() , std::ios::app );

	// д���ļ�
	fout<<result.c_str()<<"\n";		
	// ��д����������д���������ڱ����ļ��򿪵�����±����ļ���
	fout<<std::flush;	
	// �ر�	
	fout.close();	

	PrintAndroidLog( ml, result.c_str() );
}


void DebugTools::OutPutToLog( LogType ml, char* msg )
{
    if ( m_CurLogFile.empty() )
    {
        return;
    }
    std::ofstream fout;
    AIRString   result = GenFormatMsg( ml, msg );
    // ���ļ�
    fout.open( m_CurLogFile.c_str() , std::ios::app );
    // д���ļ�
    fout<<result.c_str()<<"\n";		
    // ��д����������д���������ڱ����ļ��򿪵�����±����ļ���
    fout<<std::flush;	
    // �ر�	
    fout.close();		

	PrintAndroidLog( ml, result.c_str() );
}


void DebugTools::OutPutToConsole( LogType ml, char* msg, char* szFile, int nLine, char* szFunc )
{
	AIRString result = GenFormatMsg( ml, msg, szFile, nLine, szFunc );
	std::cout<<result;
	PrintAndroidLog( ml, result.c_str() );
}

void DebugTools::OutPutToConsole( LogType ml, char* msg )
{
	AIRString result = GenFormatMsg( ml, msg );
    std::cout<<result;
	PrintAndroidLog( ml, result.c_str() );
}


void DebugTools::OutPutToDebugString( LogType ml, char* msg, char* szFile, int nLine, char* szFunc )
{
	AIRString str = GenFormatMsg( ml, msg, szFile, nLine, szFunc );
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	::OutputDebugStringA( str.c_str() );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
	PrintAndroidLog( ml, str.c_str() );
#else
	std::cout<<str.c_str()<<"\n";
#endif
}


void DebugTools::OutPutToDebugString( LogType ml, char* msg )
{
    AIRString str = GenFormatMsg( ml, msg );
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	::OutputDebugStringA( str.c_str() );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
	PrintAndroidLog( ml, str.c_str() );
#else
	std::cout<<str.c_str()<<"\n";
#endif
}

void DebugTools::OutPutToMsgBox( LogType ml, char* msg, char* szFile, int nLine, char* szFunc )
{
	AIRString str = GenFormatMsg( ml, msg, szFile, nLine, szFunc );
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	::MessageBoxA( NULL, str.c_str(), s_arLogTypeString[ml].c_str(), MB_OK );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
	PrintAndroidLog( ml, str.c_str() );
#else
	std::cout<<str.c_str()<<"\n";
#endif
	assert( ml < LT_ERROR );
}


void DebugTools::OutPutToMsgBox( LogType ml, char* msg )
{
    AIRString str = GenFormatMsg( ml, msg );
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    ::MessageBoxA( NULL, str.c_str(), s_arLogTypeString[ml].c_str(), MB_OK );
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID
	PrintAndroidLog( ml, str.c_str() );
#else
	std::cout<<str.c_str()<<"\n";
#endif
	assert( ml < LT_ERROR );
}


#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

	HWND	                            DebugTools::m_HWndCaller = NULL;
	DebugConsoleCallBackFunc            DebugTools::m_DebugConsoleCallBackFunc = NULL;

	void DebugTools::StartDebugConsoleWindow( HWND hwndCaller )
	{
		m_HWndCaller = hwndCaller;

		char appName[1024];
	#ifdef _DEBUG
		snprintf(appName, 1024, "DebugConsole_d.exe -%d", (long)hwndCaller);
	#else
		snprintf(appName, 1024, "DebugConsole.exe -%d", (long)hwndCaller);
	#endif

		//���Ҵ˵��Կ���̨�Ƿ��Ѿ�����
		char existAppTitle[1024];
		snprintf(existAppTitle, 1024, "���Կ���̨[%d]", (long)hwndCaller);
		HWND targetHwnd = ::FindWindow(NULL, existAppTitle);
		if(targetHwnd!=NULL)
		{
			ShowWindow( targetHwnd, SW_SHOW );
			//MessageBoxA(NULL,"���Կ���̨�Ѿ�����","WARN",NULL);
			return;
		}
		WinExec(appName, SW_SHOW);
	}


	// �رյ��Կ���̨(�ⲿAPPLICATION����)
	void DebugTools::ShutDebugConsoleWindow( void )
	{
		//���Ҵ˵��Կ���̨�Ƿ��Ѿ�����
		char existAppTitle[1024];
		snprintf( existAppTitle, 1024, "���Կ���̨[%d]", (long)m_HWndCaller );

		HWND targetHwnd = ::FindWindow(NULL, existAppTitle);
		if( targetHwnd != NULL )
		{
			::SendMessage(targetHwnd, WM_CLOSE, 0, 0);
		}
	}


	void DebugTools::OutPutToDebugConsoleWindow( LogType lt, const AIRString& text )
	{
		//���Ҵ˵��Կ���̨�Ƿ��Ѿ�����
		char existAppTitle[1024];
		snprintf( existAppTitle, 1024, "���Կ���̨[%d]", (long)m_HWndCaller );

		HWND targetHwnd = ::FindWindow(NULL, existAppTitle);
		if( targetHwnd != NULL )
		{
			LogInfo logInfo;
			logInfo.type = lt;
			ZeroMemory(logInfo.message, sizeof(logInfo.message));
			strcpy_s(logInfo.message, LOG_INFO_MAX_LENGTH, text.c_str());
			COPYDATASTRUCT cpd;
			cpd.dwData = 0;
			cpd.cbData = sizeof(logInfo);
			cpd.lpData = &logInfo;
			::SendMessage(targetHwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)&cpd);
		}
	}


	void DebugTools::OnCopyData( COPYDATASTRUCT* pCopyDataStruct )
	{
		if ( !pCopyDataStruct )
		{
			return;
		}
		LogInfo* pLogInfo = static_cast<LogInfo*>(pCopyDataStruct->lpData);
		LogType logType = pLogInfo->type;
		CHAR* pMessage = pLogInfo->message;
		if ( m_DebugConsoleCallBackFunc )
		{
			m_DebugConsoleCallBackFunc(pMessage);
		}
	}


	void DebugTools::RegDebugConsoleCallBackFunc( DebugConsoleCallBackFunc func )
	{
		m_DebugConsoleCallBackFunc = func;
	}

#endif

void DebugTools::FailedAssert( char* file, long line, char* func, char* statement )
{
    static bool debug = true;

    if (debug)
    {
        AIRString msg = GetString( "�����쳣: (%s)\n\n�ļ�:   %s\n�к�:   %d\n����:   %s\n\n", statement, file, line, func );

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        if ( IsDebuggerPresent() )
        {
            msg += "����?";
            int res = MessageBoxA( NULL, msg.c_str(), "Assert failed", MB_YESNOCANCEL | MB_ICONERROR );
            if (res == IDYES)
            {
				#if _MSC_VER >= 1400
					__debugbreak();
				#else
					_asm int 0x03;
				#endif
            } 
            else if (res == IDCANCEL)
            {
                debug = false;
            }
        } 
        else 
        {
            msg +="Display more asserts?";
            if (MessageBoxA(NULL, msg.c_str(), "Assert failed", MB_YESNO | MB_ICONERROR | MB_DEFBUTTON2) != IDYES)
            {
                debug = false;
            }
        }
#endif
    }
}

