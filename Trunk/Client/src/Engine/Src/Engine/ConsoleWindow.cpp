#include "stdafx.h"
#include "ConsoleWindow.h"
#include "DebugTools.h"
#include <stdio.h>

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32


	ConsoleWindow::ConsoleWindow() : m_pfnConsoleEvent(NULL)
	{
	}


	ConsoleWindow::~ConsoleWindow()
	{
		COORD coordPos = {0, 0};
		// 设置控制台光标位置
		::SetConsoleCursorPosition( m_hOutput, coordPos );
		// 释放控制台
		::FreeConsole();
	}


	void ConsoleWindow::Initial( int nWidth, int nHeight, const char *pszCaption )
	{
		// 创建控制台
		::AllocConsole();

		m_hInput	= ::GetStdHandle(STD_INPUT_HANDLE);  // 获取标准输入设备句柄
		m_hOutput	= ::GetStdHandle(STD_OUTPUT_HANDLE); // 获取标准输出设备句柄
		m_hError	= ::GetStdHandle(STD_ERROR_HANDLE);  // 获取标准出错设备句柄

		// 控制台区域
		m_rcConsole.Left	= 0;
		m_rcConsole.Top		= 0;
		m_rcConsole.Right	= nWidth;
		m_rcConsole.Bottom	= nHeight;

		// 输入区域
		m_rcInput.Left		= 2;
		m_rcInput.Right		= nWidth  - 1;
		m_rcInput.Top		= nHeight - 3;
		m_rcInput.Bottom	= nHeight - 2;

		// 输出区域
		m_rcOutput.Left		= 0;
		m_rcOutput.Right	= nWidth  - 1;
		m_rcOutput.Top		= 2;
		m_rcOutput.Bottom	= nHeight - 4;

		// 设置窗口大小
		ResizeConBufAndWindow( nWidth, nHeight );

		// 标题属性
		m_usCapsAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

		// 字体属性
		m_usFontAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

		// 设置控制台输出窗口的字体属性
		::SetConsoleTextAttribute( m_hOutput, m_usFontAttribute );

		// 清空控制台屏幕
		ClearScreen();

		// 渲染窗口中间的标题
		RenderCaption( pszCaption );

		int i = 0, nRegion = (nWidth % 2) == 0 ? nWidth / 2 : nWidth / 2 + 1;
		COORD coordPos = {m_rcConsole.Left, m_rcConsole.Top + 1};

		// 最上面的分界线
		for( i = 0; i < nRegion; ++i )
		{
			// 设置控制台输出窗口的属性
			::FillConsoleOutputAttribute( m_hOutput, m_usCapsAttribute, 2, coordPos, NULL );
			// 输出字符
			::WriteConsoleOutputCharacter( m_hOutput, "─", 2, coordPos, NULL );
			coordPos.X += 2;
		}

		// 最下面的分界线
		coordPos.X = m_rcConsole.Left;
		coordPos.Y = m_rcConsole.Bottom - 4;
		for( i = 0; i < nRegion; ++i )
		{
			// 设置控制台输出窗口的属性
			::FillConsoleOutputAttribute( m_hOutput, m_usCapsAttribute, 2, coordPos, NULL );
			// 输出字符
			::WriteConsoleOutputCharacter( m_hOutput, "─", 2, coordPos, NULL );
			coordPos.X += 2;
		}

		// 显示输出前缀符
		coordPos.X = m_rcConsole.Left;
		coordPos.Y = m_rcConsole.Bottom - 3;
		::WriteConsoleOutputCharacter( m_hOutput, ">>", 2, coordPos, NULL );

		// 设置控制台输出窗口光标位置
		coordPos.X += 2;
		::SetConsoleCursorPosition( m_hOutput, coordPos );

		m_usCharPos = 0;
		m_szInput[0] = '\0';
		m_nStatusCount = 0;

		// 更新状态栏
		RenderStatus();

		// 创建控制台线程
		DWORD dwThreadId;
		m_hThread = ::CreateThread( NULL, 0, ConsoleThread, this, 0, &dwThreadId );
		if( m_hThread == NULL )
		{
			DT_TO_MSGBOX_FMT( LT_ERROR, "创建控制台线程失败!" );
			return;
		}
	}


	void ConsoleWindow::Shutdown()
	{
		::CloseHandle( m_hInput );
		::CloseHandle( m_hOutput );
		::CloseHandle( m_hError );
	}


	DWORD ConsoleWindow::ConsoleThread( LPVOID pParam )
	{
		ConsoleWindow *pThis = (ConsoleWindow *)pParam;
		pThis->Execute();

		return 0;
	}


	void ConsoleWindow::Execute()
	{
		INPUT_RECORD inputBuffer;
		DWORD dwState = 0, dwInputEvents;

		for( ;; )
		{
			// 睡眠20毫秒
			Sleep( 20 );

			// 读取控制台的输入字符串以及消息类型
			ReadConsoleInput( m_hInput, &inputBuffer, 1, &dwInputEvents );

			// 键盘事件
			if( inputBuffer.EventType == KEY_EVENT )
			{
				OnKeyEvent( &(inputBuffer.Event.KeyEvent) );
			}
			// 鼠标事件
			if( inputBuffer.EventType == MOUSE_EVENT )
			{
				if( inputBuffer.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED )
				{
					// 处理上下翻页
				}
			}
		}
	}


	void ConsoleWindow::OnKeyEvent( KEY_EVENT_RECORD *pKeyEvent )
	{
		m_lockConsole.Lock();

		// 按下键盘按键
		if( pKeyEvent->bKeyDown )
		{
			// 取得字符
			char cText = pKeyEvent->uChar.AsciiChar;
			wchar_t wcText = pKeyEvent->uChar.UnicodeChar;
			// 合成字符串
			OnCharProcess( cText, wcText );
		}

		m_lockConsole.Unlock();
	}


	void ConsoleWindow::OnCharProcess( char cText, wchar_t wcText )
	{
		static bool s_bFirstChar = false;
		static char s_szAsciiChar[2];

		DWORD dwWritten;
		COORD chPos = {m_rcInput.Left, m_rcInput.Top};

		bool bFullBuffer = false;
		WORD usWidth = m_rcInput.Right - m_rcInput.Left;
		chPos.X += m_usCharPos % usWidth;
		chPos.Y += m_usCharPos / usWidth;

		// 检查是否输入字符串缓冲是否已经满了
		if( chPos.Y > m_rcInput.Bottom && chPos.X > usWidth )
			bFullBuffer = true;

		// 回车键
		if( cText == 0x0D ) 
		{
			if( m_usCharPos == 0 )
				return;

			m_szInput[m_usCharPos] = '\0';

			// 发送执行命令事件
			SendEvent( cet_InputEnter, m_szInput );
			// OnOutputString( m_szInput, cta_Purple );

			// 清空输入缓冲
			ClearInputText( m_szInput );
			return;
		}
		// 非ascii字符
		else if( wcText > 128 )
		{
			if( bFullBuffer )
				return;

			if( !s_bFirstChar )
			{
				s_szAsciiChar[0] = cText;
				s_bFirstChar = true;
				return;
			}

			if( chPos.X + 2 > m_rcInput.Right )
			{
				m_szInput[m_usCharPos] = ' ';
				m_usCharPos += 1;
				chPos.X = m_rcInput.Left;
				chPos.Y += 1;
			}

			wchar_t wcWideChar;

			// 转成宽字节
			s_szAsciiChar[1] = cText;
			::MultiByteToWideChar( CP_ACP, 0, s_szAsciiChar, 2, &wcWideChar, 1 );

			// 设置光标位置
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// 写入字符并设置属性
			::WriteConsoleOutputCharacterW( m_hOutput, &wcWideChar, 1, chPos, NULL );
			::WriteConsoleOutputAttribute( m_hOutput, &m_usFontAttribute, 1, chPos, &dwWritten );

			m_szInput[m_usCharPos] = s_szAsciiChar[0];
			m_szInput[m_usCharPos+1] = s_szAsciiChar[1];

			chPos.X += 2;
			m_usCharPos += 2;
			s_bFirstChar = false;
		}
		// 删除键
		else if( cText == 0x08 ) 
		{
			if( chPos.X == m_rcInput.Left && chPos.Y == m_rcInput.Top )
				return;

			cText = ' ';
			m_usCharPos -= 1;

			// 如果是中文再向前移动一步
			if( m_szInput[m_usCharPos] & 0x80 )
				m_usCharPos -= 1;

			chPos.X = m_rcInput.Left;
			chPos.Y = m_rcInput.Top;
			chPos.X += m_usCharPos % usWidth;
			chPos.Y += m_usCharPos / usWidth;

			// 设置光标位置
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// 删除字符
			if( m_szInput[m_usCharPos+1] & 0x80 )
			{
				::WriteConsoleOutputCharacter( m_hOutput, &cText, 1, chPos, NULL );

				chPos.X += 1;
				::WriteConsoleOutputCharacter( m_hOutput, &cText, 1, chPos, NULL );
				chPos.X -= 1;
			}
			else
			{
				::WriteConsoleOutputCharacter( m_hOutput, &cText, 1, chPos, NULL );
			}
		}
		// ascii字符
		else
		{
			if( (cText < 0x20) || (cText > 0x7e) || bFullBuffer )
				return;

			if( chPos.X + 1 > m_rcInput.Right )
			{
				chPos.X = m_rcInput.Left;
				chPos.Y++;
			}

			// 设置光标位置
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// 写入字符并设置属性
			::WriteConsoleOutputCharacter( m_hOutput, &cText, 1, chPos, NULL );
			::WriteConsoleOutputAttribute( m_hOutput, &m_usFontAttribute, 1, chPos, &dwWritten );

			chPos.X++;
			m_szInput[m_usCharPos] = cText;
			m_usCharPos += 1;
		}

		// 光标移到到下一行
		if( chPos.X > m_rcInput.Right )
		{
			chPos.X = m_rcInput.Left;
			chPos.Y++;
		}

		// 设置光标位置
		::SetConsoleCursorPosition( m_hOutput, chPos );
	}


	void ConsoleWindow::Print( const char *pszString, WORD usAttribute )
	{
		m_lockConsole.Lock();
		// 打印字符
		OnOutputString( pszString, usAttribute );
		m_lockConsole.Unlock();
	}


	void ConsoleWindow::OnOutputString( const char *pszString, WORD usAttribute )
	{
		char szAsciiChar[2];
		bool bFirstText = false;
		DWORD dwWritten;
		wchar_t wcWideChar;	
		COORD chPos = {m_rcOutput.Left, m_rcOutput.Top};

		// 滚动屏幕
		if( chPos.Y > m_rcOutput.Bottom - 1 )
		{
			m_rcOutput.Top = m_rcConsole.Top + 1;
			DeleteTopLine( m_rcOutput );
			chPos.Y = m_rcOutput.Bottom - 1;
			m_rcOutput.Top = chPos.Y;
		}

		// 显示当前时间
		RenderTimes( chPos );

		while( *pszString )
		{
			// 设置光标位置
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// 处理换行符
			if( *pszString == '\n' )
			{
                // 取下一个字符
				++pszString;
                // 计算光标的新位置
				chPos.X = m_rcOutput.Left;
				chPos.Y += 1;
				m_rcOutput.Top = chPos.Y;
				continue;
			}

            // 处理tab字符
            if ( *pszString == '\t' )
            {
                ++pszString;
                int count = (chPos.X-m_rcOutput.Left)/8;
                int offset = (chPos.X-m_rcOutput.Left)%8;
                (offset > 0 ? ++count : count);
                chPos.X = m_rcOutput.Left + count*8;
                if ( chPos.X > m_rcOutput.Right )
                {
                    chPos.X = m_rcOutput.Left;
                    chPos.Y += 1;
                    m_rcOutput.Top = chPos.Y;
                }
            }

			// 中文
			if( *pszString & 0x80 )	
			{
				if( !bFirstText )
				{
					szAsciiChar[0] = *pszString++;
					bFirstText = true;
					continue;
				}

				szAsciiChar[1] = *pszString++;

				// 转成宽字节
				::MultiByteToWideChar( CP_ACP, 0, szAsciiChar, 2, &wcWideChar, 1 );

				::WriteConsoleOutputCharacterW( m_hOutput, &wcWideChar, 1, chPos, NULL );
				WriteAttribute( usAttribute, 2, chPos );
				bFirstText = false;
			}
			// 英文
			else	
			{
				::WriteConsoleOutputCharacter( m_hOutput, pszString, 1, chPos, NULL );
				::WriteConsoleOutputAttribute( m_hOutput, &usAttribute, 1, chPos, &dwWritten );

				pszString += 1;
				chPos.X += 1;
			}

			if( chPos.X >= m_rcOutput.Right )
			{
				chPos.X = m_rcOutput.Left;
				chPos.Y += 1;
				m_rcOutput.Top += 1;
			}

			// 滚动屏幕
			if( chPos.Y > m_rcOutput.Bottom - 1 )
			{
				m_rcOutput.Top = m_rcConsole.Top + 1;
				DeleteTopLine( m_rcOutput );
				chPos.Y = m_rcOutput.Bottom - 1;
				m_rcOutput.Top = chPos.Y;
			}
		}

		// 移动到下一行
		m_rcOutput.Top += 1;

		// 恢复光标到输入的位置
		WORD usWidth = m_rcInput.Right - m_rcInput.Left;
		chPos.X = m_rcInput.Left + m_usCharPos % usWidth;
		chPos.Y = m_rcInput.Top + m_usCharPos / usWidth;

		::SetConsoleCursorPosition( m_hOutput, chPos );
	}


	void ConsoleWindow::WriteAttribute( WORD usAttribute, int nCount, COORD &coordPos )
	{
		int i = 0;
		DWORD dwWritten = 0;

		for( i = 0; i < nCount; ++i )
		{
			::WriteConsoleOutputAttribute( m_hOutput, &usAttribute, 1, coordPos, &dwWritten );
			coordPos.X += 1;
		}
	}


	void ConsoleWindow::SetStatusInfo( int nCount )
	{
		m_nStatusCount = nCount;
		for( int i=0; i < m_nStatusCount; ++i )
		{
			m_vecStatusText.push_back( AIRString("") );
		}
	}


	void ConsoleWindow::SetStatusText( int nIndex, const char *pszText )
	{
		if( nIndex < 0 || nIndex >= m_nStatusCount )
		{
			char szBuffer[128];
			sprintf( szBuffer, "设置状态栏数据索引(%d)越界, 最大索引(%d).", nIndex, m_nStatusCount -1 );
			Print( szBuffer, /*ConsoleWindow*/eConsoleTextAttribute::cta_Red );
			return;
		}

		*(m_vecStatusText.begin() + nIndex) = pszText;

		// 更新状态栏
		RenderStatus();
	}


	void ConsoleWindow::SetConsoleWindowTitle( const char *pszTitle )
	{
		::SetConsoleTitleA( pszTitle );
	}


	void ConsoleWindow::SetConsoleCallback( ConsoleEventCallback *pfnConsoleEvent )
	{
		m_pfnConsoleEvent = pfnConsoleEvent;
	}


	void ConsoleWindow::SendEvent( DWORD dwEventType, const char *pszString )
	{
		if( m_pfnConsoleEvent != NULL )
			m_pfnConsoleEvent( dwEventType, pszString );
	}


	void ConsoleWindow::RenderTimes( COORD &coordPos )
	{
		SYSTEMTIME st;
		::GetLocalTime( &st );

		DWORD dwWritten;
		char szBuffer[64];
		sprintf( szBuffer, "[%04d/%02d/%02d %02d:%02d:%02d.%03d] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );

		int i, nLength = (int)strlen(szBuffer);
		for( i = 0; i < nLength; ++i )
		{
			// 设置光标位置
			::SetConsoleCursorPosition( m_hOutput, coordPos );
			::WriteConsoleOutputCharacter( m_hOutput, &szBuffer[i], 1, coordPos, NULL );
			::WriteConsoleOutputAttribute( m_hOutput, &m_usCapsAttribute, 1, coordPos, &dwWritten );
			coordPos.X += 1;
		}
	}


	void ConsoleWindow::RenderCaption( const char *pszCaption )
	{
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		::GetConsoleScreenBufferInfo( m_hOutput, &csbInfo );

		int nLength = (int)strlen(pszCaption);
		COORD posText = {0, 0};
		posText.X = (csbInfo.dwSize.X - nLength) / 2 - 2;

		// 写文本
		::FillConsoleOutputAttribute( m_hOutput, m_usCapsAttribute, nLength, posText, NULL );
		::WriteConsoleOutputCharacter( m_hOutput, pszCaption, nLength, posText, NULL );

		// 恢复原来的属性
		::SetConsoleTextAttribute( m_hOutput, csbInfo.wAttributes );
	}


	void ConsoleWindow::RenderStatus()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		::GetConsoleScreenBufferInfo( m_hOutput, &csbInfo );

		COORD coordBak = {0, csbInfo.dwSize.Y-1};
		WORD usBakAttr  = BACKGROUND_INTENSITY ;
		WORD usTextAttr = FOREGROUND_RED | BACKGROUND_INTENSITY;

		::FillConsoleOutputAttribute( m_hOutput, usBakAttr, csbInfo.dwSize.X, coordBak, NULL );
		::FillConsoleOutputCharacter( m_hOutput, ' ', csbInfo.dwSize.X, coordBak, NULL );

		::SetConsoleTextAttribute( m_hOutput, usTextAttr );

		COORD coordPos = {1, csbInfo.dwSize.Y-1};
		AIRVector<AIRString>::iterator Iter = m_vecStatusText.begin();

		for( ; Iter != m_vecStatusText.end(); ++Iter )
		{
			::SetConsoleCursorPosition( m_hOutput, coordPos );
			::WriteConsole( m_hOutput, (*Iter).c_str(), (u32)(*Iter).size(), NULL, NULL );
			coordPos.X += (WORD)(*Iter).size() + 1;
		}

		::SetConsoleTextAttribute( m_hOutput, csbInfo.wAttributes ); // 恢复原来的属性
		::SetConsoleCursorPosition( m_hOutput, csbInfo.dwCursorPosition ); // 恢复原来的光标位置
	}


	void ConsoleWindow::DeleteTopLine( SMALL_RECT rc )
	{
		COORD crDest;
		CHAR_INFO chFill;
		SMALL_RECT rcClip = rc;

		rcClip.Top++;
		rcClip.Bottom--;

		crDest.X = rcClip.Left;
		crDest.Y = rcClip.Top - 1;

		CONSOLE_SCREEN_BUFFER_INFO bInfo;
		::GetConsoleScreenBufferInfo( m_hOutput, &bInfo ); 
		chFill.Attributes = bInfo.wAttributes;
		chFill.Char.AsciiChar = ' ';
		::ScrollConsoleScreenBuffer( m_hOutput, &rcClip, &rcClip, crDest, &chFill) ;
	}


	void ConsoleWindow::ClearInputText( const char *pszString )
	{
		char cSpace = ' ';
		COORD chPos = {m_rcInput.Left, m_rcInput.Top};

		while( *pszString )
		{
			// 设置光标位置
			::SetConsoleCursorPosition( m_hOutput, chPos ); 

			// 中文
			if( *pszString & 0x80 )	
			{
				::WriteConsoleOutputCharacter( m_hOutput, &cSpace, 1, chPos, NULL );
				::WriteConsoleOutputCharacter( m_hOutput, &cSpace, 1, chPos, NULL );

				pszString += 2;
				chPos.X += 2;
			}
			// 英文
			else	
			{
				::WriteConsoleOutputCharacter( m_hOutput, &cSpace, 1, chPos, NULL );

				pszString += 1;
				chPos.X += 1;
			}

			if( chPos.X >= m_rcInput.Right )
			{
				chPos.X = m_rcInput.Left;
				chPos.Y += 1;
			}
		}

		m_usCharPos = 0;

		COORD cursorPos = {m_rcInput.Left, m_rcInput.Top};
		::SetConsoleCursorPosition( m_hOutput, cursorPos );
	}


	void ConsoleWindow::ClearScreen()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		::GetConsoleScreenBufferInfo( m_hOutput, &csbInfo ); 

		COORD coordHome = {0, 0};
		WORD usAttribute = csbInfo.wAttributes;
		unsigned long uSize = csbInfo.dwSize.X * csbInfo.dwSize.Y;

		::FillConsoleOutputAttribute( m_hOutput, usAttribute, uSize, coordHome, NULL );
		::FillConsoleOutputCharacter( m_hOutput, ' ', uSize, coordHome, NULL );
	}


	void ConsoleWindow::ResizeConBufAndWindow( short xSize, short ySize )
	{
		// 记录当前控制台缓冲信息
		CONSOLE_SCREEN_BUFFER_INFO csbi; 
		BOOL bSuccess;
		// 记录新控制台的大小
		SMALL_RECT srWindowRect; 
		COORD coordScreen;

		bSuccess = ::GetConsoleScreenBufferInfo(m_hOutput, &csbi);

		// 获得控制台的大小的最大值
		coordScreen = ::GetLargestConsoleWindowSize(m_hOutput);

		// 定义新控制台窗口大小以及滚动块位置
		srWindowRect.Right = (SHORT)(min(xSize, coordScreen.X) - 1);
		srWindowRect.Bottom = (SHORT)(min(ySize, coordScreen.Y) - 1);
		srWindowRect.Left = srWindowRect.Top = (SHORT) 0;

		// 定义新控制台缓冲大小
		coordScreen.X = xSize;
		coordScreen.Y = ySize;
		// 如果当前缓冲的大小大于我们要设置的缓冲大小,先重新设置窗口大小,再设置缓冲大小
		if ((u32) csbi.dwSize.X * csbi.dwSize.Y > (u32) xSize * ySize)
		{
			bSuccess = ::SetConsoleWindowInfo(m_hOutput, TRUE, &srWindowRect);
			bSuccess = ::SetConsoleScreenBufferSize(m_hOutput, coordScreen);
		}
		// 如果当前缓冲的大小小于我们要设置的缓冲大小,先重新设置缓冲大小,再设置窗口大小
		if ((u32) csbi.dwSize.X * csbi.dwSize.Y < (u32) xSize * ySize)
		{
			bSuccess = ::SetConsoleScreenBufferSize(m_hOutput, coordScreen);
			bSuccess = ::SetConsoleWindowInfo(m_hOutput, TRUE, &srWindowRect);
		}
		return;
	}

#endif