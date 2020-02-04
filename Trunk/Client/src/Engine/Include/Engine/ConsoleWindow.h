#ifndef __Console_Window_h__
#define __Console_Window_h__

#include "EngineConfig.h"
#include <stdlib.h>
//#include <windows.h>
#include <string>
#include <vector>
#include "CCS.h"
#include "MemoryAllocatorConfig.h"


// ����̨�¼�����
enum eConsoleEventType
{
	cet_InputEnter	///< �س�������, �������������
};

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
// ����̨��������
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

// ����̨����
class EngineExport ConsoleWindow
{
public:
	// ����̨�¼��ص�����
	typedef void ConsoleEventCallback( DWORD dwEventType, const char *pszString );


public:
	ConsoleWindow();
	~ConsoleWindow();

	// ��ʼ��
	void Initial( int nWidth, int nHeight, const char *pszCaption );

	// �ر�
	void Shutdown( void );

	// ִ��
	void Execute( void );

	// �������Ļ
	void Print( const char *pszString, WORD usAttribute );

	// ���ÿ���̨�ص�����
	void SetConsoleCallback( ConsoleEventCallback *pfnConsoleEvent );

	// �����Ļ
	void ClearScreen( void );

	// ���ÿ���̨���ڱ���
	void SetConsoleWindowTitle( const char *pszTitle );

	// ����״̬��Ϣ
	void SetStatusInfo( int nCount );

	// ����״̬����
	void SetStatusText( int nIndex, const char *pszText );

	// ����̨�߳�
	static DWORD WINAPI ConsoleThread( LPVOID pParam );

private:
	// ����״̬��
	void RenderStatus( void );

	// ��Ⱦ�����м�ı���
	void RenderCaption( const char *pszCaption );

	// ��Ⱦʱ��
	void RenderTimes( COORD &coordPos );

	// ������������
	void ClearInputText( const char *pszString );

	// �������ÿ���̨�����Լ����ڴ�С
	void ResizeConBufAndWindow( short xSize, short ySize );

	// д������
	void WriteAttribute( WORD usAttribute, int nCount, COORD &coordPos );

	// ��������¼�
	void OnKeyEvent( KEY_EVENT_RECORD *pKeyEvent );

	// �����ַ�������
	void OnCharProcess( char cText, wchar_t wcText );

	// ����ַ���
	void OnOutputString( const char *pszString, WORD usAttribute );

	// ������Ļ
	void DeleteTopLine( SMALL_RECT rc );

	// �����¼�
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