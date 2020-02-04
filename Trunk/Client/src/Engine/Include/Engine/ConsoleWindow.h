#ifndef __Console_Window_h__
#define __Console_Window_h__

#include "EngineConfig.h"
#include <stdlib.h>
//#include <windows.h>
#include <string>
#include <vector>
#include "CCS.h"
#include "MemoryAllocatorConfig.h"


// 控制台事件类型
enum eConsoleEventType
{
	cet_InputEnter	///< 回车键按下, 处理输入的数据
};

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
// 控制台文字属性
enum eConsoleTextAttribute
{
	cta_Red		= FOREGROUND_RED	| FOREGROUND_INTENSITY,
	cta_Green	= FOREGROUND_GREEN	| FOREGROUND_INTENSITY,
	cta_Blue	= FOREGROUND_BLUE	| FOREGROUND_INTENSITY,
	cta_Purple	= FOREGROUND_RED	| FOREGROUND_BLUE	| FOREGROUND_INTENSITY,
	cta_Yellow	= FOREGROUND_RED	| FOREGROUND_GREEN	| FOREGROUND_INTENSITY,
	cta_Aqua	= FOREGROUND_GREEN	| FOREGROUND_BLUE	| FOREGROUND_INTENSITY,
	cta_White	= FOREGROUND_RED	| FOREGROUND_GREEN	| FOREGROUND_BLUE	| FOREGROUND_INTENSITY
};

// 控制台窗口
class EngineExport ConsoleWindow
{
public:
	// 控制台事件回调函数
	typedef void ConsoleEventCallback( DWORD dwEventType, const char *pszString );


public:
	ConsoleWindow();
	~ConsoleWindow();

	// 初始化
	void Initial( int nWidth, int nHeight, const char *pszCaption );

	// 关闭
	void Shutdown( void );

	// 执行
	void Execute( void );

	// 输出到屏幕
	void Print( const char *pszString, WORD usAttribute );

	// 设置控制台回调函数
	void SetConsoleCallback( ConsoleEventCallback *pfnConsoleEvent );

	// 清空屏幕
	void ClearScreen( void );

	// 设置控制台窗口标题
	void SetConsoleWindowTitle( const char *pszTitle );

	// 设置状态信息
	void SetStatusInfo( int nCount );

	// 设置状态文字
	void SetStatusText( int nIndex, const char *pszText );

	// 控制台线程
	static DWORD WINAPI ConsoleThread( LPVOID pParam );

private:
	// 更新状态栏
	void RenderStatus( void );

	// 渲染窗口中间的标题
	void RenderCaption( const char *pszCaption );

	// 渲染时间
	void RenderTimes( COORD &coordPos );

	// 清空输入的文字
	void ClearInputText( const char *pszString );

	// 重新设置控制台缓冲以及窗口大小
	void ResizeConBufAndWindow( short xSize, short ySize );

	// 写入属性
	void WriteAttribute( WORD usAttribute, int nCount, COORD &coordPos );

	// 处理键盘事件
	void OnKeyEvent( KEY_EVENT_RECORD *pKeyEvent );

	// 处理字符串输入
	void OnCharProcess( char cText, wchar_t wcText );

	// 输出字符串
	void OnOutputString( const char *pszString, WORD usAttribute );

	// 滚动屏幕
	void DeleteTopLine( SMALL_RECT rc );

	// 发送事件
	void SendEvent( DWORD dwEventType, const char *pszString );

private:
	char						m_szInput[512];
	WORD						m_usCharPos;
	WORD						m_usCapsAttribute;
	WORD						m_usFontAttribute;
	int							m_nStatusCount;
	AIRVector<AIRString>	m_vecStatusText;
	HANDLE						m_hInput;
	HANDLE						m_hOutput;
	HANDLE						m_hError;
	HANDLE						m_hThread;
	SMALL_RECT					m_rcConsole;
	SMALL_RECT					m_rcInput;
	SMALL_RECT					m_rcOutput;
	CCS							m_lockConsole;
	ConsoleEventCallback *		m_pfnConsoleEvent;
};
#endif


#endif