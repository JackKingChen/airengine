#include "stdafx.h"
#include "Console.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32


#include <WinCon.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Util.h"


#define MAX_CMD_LEN 4096

char G_BUF[MAX_CMD_LEN];

#define USE_CONSOLE

#ifdef USE_CONSOLE


Console::Console(void) : m_hWnd(NULL), m_bHasExecuteCmd(true), m_bIsShowConsole(false), m_bInited(false)
	{	

	}


	Console::~Console(void)
	{
		FreeConsole();
        m_szCmdID.clear();
        m_szCmdContent.clear();
	}


	Console* Console::GetInstance()
	{
		static Console s_inst;
		return &s_inst;
	}


	void Console::SetConsolePos( int x, int y )	
	{ 
		::SetWindowPos( m_hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);
		::SendMessage( m_hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0 );
	}


	void Console::ShowConsole( bool b )				
	{ 
        if (!m_bInited)
        {
            if (b)
            {
                Init();
            }
        }
        else
            ::ShowWindow( m_hWnd, b ? SW_SHOW : SW_HIDE );
        m_bIsShowConsole = b;
	}


	void Console::MinimumConsole( void )				
	{ 
		::SendMessage( m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0 ); 
	}


	void Console::RestoreConsole( void )				
	{ 
		::SendMessage( m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0 ); 
	}


	void Console::FreeConsole( void )				
	{ 
		::FreeConsole(); 
	}


    void Console::Print( const char* szMsg )
    {
        if ( szMsg == NULL )
        {
            return;
        }
        u32 m_outFontAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        // 设置控制台输出窗口的字体属性
        ::SetConsoleTextAttribute( m_hOutput,  m_outFontAttribute );
        std::cout.clear();
        std::cin.clear();
        std::cout<<"<< "<<szMsg<<endl;
    }


    void Console::ProcInputCmd( void )
    {
        if ( !m_bIsShowConsole )
        {
            return;
        }
        std::cout.clear();
        std::cin.clear();
        m_bHasExecuteCmd = false;
        u32 m_outFontAttribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        // 设置控制台输出窗口的字体属性
        ::SetConsoleTextAttribute( m_hOutput,  m_outFontAttribute );
        std::cout<<"\n>> ";
        std::cin.getline( G_BUF, MAX_CMD_LEN );

        m_szCmdID.clear();
        m_szCmdContent.clear();
        bool findContent = false;
        for ( int i = 0; i < strlen( G_BUF ); ++i )
        {
            if ( findContent )
            {
                m_szCmdContent.push_back( G_BUF[i] );
            }
            else
            {
                if ( G_BUF[i] == ' ' )
                {
                    findContent = true;
                }
                else
                {
                    m_szCmdID.push_back( G_BUF[i] );
                }
            }
        }
        Print( GetString( "cmd ID      : %s\n", m_szCmdID.c_str() ) );
        Print( GetString( "cmd content : %s\n", m_szCmdContent.c_str() ) );
    }

    // 每帧更新
    void Console::Update( void )
    {
        if ( m_bHasExecuteCmd )
        {
            m_szCmdContent.clear();
            m_szCmdID.clear();
        }
        else
        {
        }
    }

    void Console::SetHasExecuteCmd( bool b )
    {
        m_bHasExecuteCmd = b;
    }

    void Console::Init()
    {
        ::AllocConsole();
        ::CreateConsoleScreenBuffer(
            GENERIC_WRITE|GENERIC_READ,
            0,
            0,
            CONSOLE_TEXTMODE_BUFFER,
            0);

        m_hInput   = ::GetStdHandle(STD_INPUT_HANDLE ); 
        m_hOutput  = ::GetStdHandle(STD_OUTPUT_HANDLE);
        m_hEoutput = ::GetStdHandle(STD_ERROR_HANDLE );

        FILE *fi = _fdopen( _open_osfhandle((long)m_hInput,  _O_TEXT),"r");  
        *stdin=*fi;  
        setvbuf(stdin,NULL,_IONBF,0);

        FILE *fo = _fdopen( _open_osfhandle((long)m_hOutput, _O_TEXT),"w"); 
        *stdout=*fo; 
        setvbuf(stdout,NULL,_IONBF,0);

        FILE *fe = _fdopen( _open_osfhandle((long)m_hEoutput,_O_TEXT),"w");
        *stderr=*fe; 
        setvbuf(stderr, NULL, _IONBF, 0);

        // ENABLE_QUICK_EDIT_MODE 0x0040
        // ENABLE_ECHO_INPUT 0x0004
        ::SetConsoleMode(
            m_hInput,
            ENABLE_ECHO_INPUT|
            ENABLE_PROCESSED_INPUT|
            ENABLE_WINDOW_INPUT|
            ENABLE_MOUSE_INPUT|
            ENABLE_PROCESSED_OUTPUT|
            ENABLE_WRAP_AT_EOL_OUTPUT|
            ENABLE_LINE_INPUT|
            0x0004|
            0x0040);

        m_hWnd = GetConsoleWindow();

        u32 dwExStyle = ::GetWindowLong( m_hWnd, GWL_EXSTYLE );
        if( (dwExStyle&0x80000)!=0x80000 )
        {
            ::SetWindowLong(
                m_hWnd, 
                GWL_EXSTYLE, 
                WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | (dwExStyle^0x80000));
        }
        m_bInited = true;
    }

#else

	Console::Console(void)
	{	

	}


	Console::~Console(void)
	{

	}


	Console* Console::GetInstance()
	{
		static Console s_inst;
		return &s_inst;
	}


	void Console::SetConsolePos( int x, int y )	
	{ 
	}


	void Console::ShowConsole( bool b )				
	{ 
	}


	void Console::MinimumConsole( void )				
	{ 
	}


	void Console::RestoreConsole( void )				
	{ 
	}


	void Console::FreeConsole( void )				
	{ 
	}


	void Console::Print( const char* szMsg )
	{
	}


	void Console::ProcInputCmd( void )
	{
	}

	// 获得命令行ID
	AIRString& Console::GetCmdID( void )
	{
		AIRString sz("");
		return sz;
	}

	// 获得命令行内容
	AIRString& Console::GetCmdContent( void )
	{
		AIRString sz("");
		return sz;
	}

	// 每帧更新
	void Console::Update( void )
	{
	}

	void Console::SetHasExecuteCmd( bool b )
	{
	}

#endif

#endif