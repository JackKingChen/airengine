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
		// ���ÿ���̨���λ��
		::SetConsoleCursorPosition( m_hOutput, coordPos );
		// �ͷſ���̨
		::FreeConsole();
	}


	void ConsoleWindow::Initial( int nWidth, int nHeight, const char *pszCaption )
	{
		// ��������̨
		::AllocConsole();

		m_hInput	= ::GetStdHandle(STD_INPUT_HANDLE);  // ��ȡ��׼�����豸���
		m_hOutput	= ::GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ��׼����豸���
		m_hError	= ::GetStdHandle(STD_ERROR_HANDLE);  // ��ȡ��׼�����豸���

		// ����̨����
		m_rcConsole.Left	= 0;
		m_rcConsole.Top		= 0;
		m_rcConsole.Right	= nWidth;
		m_rcConsole.Bottom	= nHeight;

		// ��������
		m_rcInput.Left		= 2;
		m_rcInput.Right		= nWidth  - 1;
		m_rcInput.Top		= nHeight - 3;
		m_rcInput.Bottom	= nHeight - 2;

		// �������
		m_rcOutput.Left		= 0;
		m_rcOutput.Right	= nWidth  - 1;
		m_rcOutput.Top		= 2;
		m_rcOutput.Bottom	= nHeight - 4;

		// ���ô��ڴ�С
		ResizeConBufAndWindow( nWidth, nHeight );

		// ��������
		m_usCapsAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

		// ��������
		m_usFontAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

		// ���ÿ���̨������ڵ���������
		::SetConsoleTextAttribute( m_hOutput, m_usFontAttribute );

		// ��տ���̨��Ļ
		ClearScreen();

		// ��Ⱦ�����м�ı���
		RenderCaption( pszCaption );

		int i = 0, nRegion = (nWidth % 2) == 0 ? nWidth / 2 : nWidth / 2 + 1;
		COORD coordPos = {m_rcConsole.Left, m_rcConsole.Top + 1};

		// ������ķֽ���
		for( i = 0; i < nRegion; ++i )
		{
			// ���ÿ���̨������ڵ�����
			::FillConsoleOutputAttribute( m_hOutput, m_usCapsAttribute, 2, coordPos, NULL );
			// ����ַ�
			::WriteConsoleOutputCharacter( m_hOutput, "��", 2, coordPos, NULL );
			coordPos.X += 2;
		}

		// ������ķֽ���
		coordPos.X = m_rcConsole.Left;
		coordPos.Y = m_rcConsole.Bottom - 4;
		for( i = 0; i < nRegion; ++i )
		{
			// ���ÿ���̨������ڵ�����
			::FillConsoleOutputAttribute( m_hOutput, m_usCapsAttribute, 2, coordPos, NULL );
			// ����ַ�
			::WriteConsoleOutputCharacter( m_hOutput, "��", 2, coordPos, NULL );
			coordPos.X += 2;
		}

		// ��ʾ���ǰ׺��
		coordPos.X = m_rcConsole.Left;
		coordPos.Y = m_rcConsole.Bottom - 3;
		::WriteConsoleOutputCharacter( m_hOutput, ">>", 2, coordPos, NULL );

		// ���ÿ���̨������ڹ��λ��
		coordPos.X += 2;
		::SetConsoleCursorPosition( m_hOutput, coordPos );

		m_usCharPos = 0;
		m_szInput[0] = '\0';
		m_nStatusCount = 0;

		// ����״̬��
		RenderStatus();

		// ��������̨�߳�
		DWORD dwThreadId;
		m_hThread = ::CreateThread( NULL, 0, ConsoleThread, this, 0, &dwThreadId );
		if( m_hThread == NULL )
		{
			DT_TO_MSGBOX_FMT( LT_ERROR, "��������̨�߳�ʧ��!" );
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
			// ˯��20����
			Sleep( 20 );

			// ��ȡ����̨�������ַ����Լ���Ϣ����
			ReadConsoleInput( m_hInput, &inputBuffer, 1, &dwInputEvents );

			// �����¼�
			if( inputBuffer.EventType == KEY_EVENT )
			{
				OnKeyEvent( &(inputBuffer.Event.KeyEvent) );
			}
			// ����¼�
			if( inputBuffer.EventType == MOUSE_EVENT )
			{
				if( inputBuffer.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED )
				{
					// �������·�ҳ
				}
			}
		}
	}


	void ConsoleWindow::OnKeyEvent( KEY_EVENT_RECORD *pKeyEvent )
	{
		m_lockConsole.Lock();

		// ���¼��̰���
		if( pKeyEvent->bKeyDown )
		{
			// ȡ���ַ�
			char cText = pKeyEvent->uChar.AsciiChar;
			wchar_t wcText = pKeyEvent->uChar.UnicodeChar;
			// �ϳ��ַ���
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

		// ����Ƿ������ַ��������Ƿ��Ѿ�����
		if( chPos.Y > m_rcInput.Bottom && chPos.X > usWidth )
			bFullBuffer = true;

		// �س���
		if( cText == 0x0D ) 
		{
			if( m_usCharPos == 0 )
				return;

			m_szInput[m_usCharPos] = '\0';

			// ����ִ�������¼�
			SendEvent( cet_InputEnter, m_szInput );
			// OnOutputString( m_szInput, cta_Purple );

			// ������뻺��
			ClearInputText( m_szInput );
			return;
		}
		// ��ascii�ַ�
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

			// ת�ɿ��ֽ�
			s_szAsciiChar[1] = cText;
			::MultiByteToWideChar( CP_ACP, 0, s_szAsciiChar, 2, &wcWideChar, 1 );

			// ���ù��λ��
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// д���ַ�����������
			::WriteConsoleOutputCharacterW( m_hOutput, &wcWideChar, 1, chPos, NULL );
			::WriteConsoleOutputAttribute( m_hOutput, &m_usFontAttribute, 1, chPos, &dwWritten );

			m_szInput[m_usCharPos] = s_szAsciiChar[0];
			m_szInput[m_usCharPos+1] = s_szAsciiChar[1];

			chPos.X += 2;
			m_usCharPos += 2;
			s_bFirstChar = false;
		}
		// ɾ����
		else if( cText == 0x08 ) 
		{
			if( chPos.X == m_rcInput.Left && chPos.Y == m_rcInput.Top )
				return;

			cText = ' ';
			m_usCharPos -= 1;

			// �������������ǰ�ƶ�һ��
			if( m_szInput[m_usCharPos] & 0x80 )
				m_usCharPos -= 1;

			chPos.X = m_rcInput.Left;
			chPos.Y = m_rcInput.Top;
			chPos.X += m_usCharPos % usWidth;
			chPos.Y += m_usCharPos / usWidth;

			// ���ù��λ��
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// ɾ���ַ�
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
		// ascii�ַ�
		else
		{
			if( (cText < 0x20) || (cText > 0x7e) || bFullBuffer )
				return;

			if( chPos.X + 1 > m_rcInput.Right )
			{
				chPos.X = m_rcInput.Left;
				chPos.Y++;
			}

			// ���ù��λ��
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// д���ַ�����������
			::WriteConsoleOutputCharacter( m_hOutput, &cText, 1, chPos, NULL );
			::WriteConsoleOutputAttribute( m_hOutput, &m_usFontAttribute, 1, chPos, &dwWritten );

			chPos.X++;
			m_szInput[m_usCharPos] = cText;
			m_usCharPos += 1;
		}

		// ����Ƶ�����һ��
		if( chPos.X > m_rcInput.Right )
		{
			chPos.X = m_rcInput.Left;
			chPos.Y++;
		}

		// ���ù��λ��
		::SetConsoleCursorPosition( m_hOutput, chPos );
	}


	void ConsoleWindow::Print( const char *pszString, WORD usAttribute )
	{
		m_lockConsole.Lock();
		// ��ӡ�ַ�
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

		// ������Ļ
		if( chPos.Y > m_rcOutput.Bottom - 1 )
		{
			m_rcOutput.Top = m_rcConsole.Top + 1;
			DeleteTopLine( m_rcOutput );
			chPos.Y = m_rcOutput.Bottom - 1;
			m_rcOutput.Top = chPos.Y;
		}

		// ��ʾ��ǰʱ��
		RenderTimes( chPos );

		while( *pszString )
		{
			// ���ù��λ��
			::SetConsoleCursorPosition( m_hOutput, chPos );

			// �����з�
			if( *pszString == '\n' )
			{
                // ȡ��һ���ַ�
				++pszString;
                // ���������λ��
				chPos.X = m_rcOutput.Left;
				chPos.Y += 1;
				m_rcOutput.Top = chPos.Y;
				continue;
			}

            // ����tab�ַ�
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

			// ����
			if( *pszString & 0x80 )	
			{
				if( !bFirstText )
				{
					szAsciiChar[0] = *pszString++;
					bFirstText = true;
					continue;
				}

				szAsciiChar[1] = *pszString++;

				// ת�ɿ��ֽ�
				::MultiByteToWideChar( CP_ACP, 0, szAsciiChar, 2, &wcWideChar, 1 );

				::WriteConsoleOutputCharacterW( m_hOutput, &wcWideChar, 1, chPos, NULL );
				WriteAttribute( usAttribute, 2, chPos );
				bFirstText = false;
			}
			// Ӣ��
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

			// ������Ļ
			if( chPos.Y > m_rcOutput.Bottom - 1 )
			{
				m_rcOutput.Top = m_rcConsole.Top + 1;
				DeleteTopLine( m_rcOutput );
				chPos.Y = m_rcOutput.Bottom - 1;
				m_rcOutput.Top = chPos.Y;
			}
		}

		// �ƶ�����һ��
		m_rcOutput.Top += 1;

		// �ָ���굽�����λ��
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
			sprintf( szBuffer, "����״̬����������(%d)Խ��, �������(%d).", nIndex, m_nStatusCount -1 );
			Print( szBuffer, /*ConsoleWindow*/eConsoleTextAttribute::cta_Red );
			return;
		}

		*(m_vecStatusText.begin() + nIndex) = pszText;

		// ����״̬��
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
			// ���ù��λ��
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

		// д�ı�
		::FillConsoleOutputAttribute( m_hOutput, m_usCapsAttribute, nLength, posText, NULL );
		::WriteConsoleOutputCharacter( m_hOutput, pszCaption, nLength, posText, NULL );

		// �ָ�ԭ��������
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

		::SetConsoleTextAttribute( m_hOutput, csbInfo.wAttributes ); // �ָ�ԭ��������
		::SetConsoleCursorPosition( m_hOutput, csbInfo.dwCursorPosition ); // �ָ�ԭ���Ĺ��λ��
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
			// ���ù��λ��
			::SetConsoleCursorPosition( m_hOutput, chPos ); 

			// ����
			if( *pszString & 0x80 )	
			{
				::WriteConsoleOutputCharacter( m_hOutput, &cSpace, 1, chPos, NULL );
				::WriteConsoleOutputCharacter( m_hOutput, &cSpace, 1, chPos, NULL );

				pszString += 2;
				chPos.X += 2;
			}
			// Ӣ��
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
		// ��¼��ǰ����̨������Ϣ
		CONSOLE_SCREEN_BUFFER_INFO csbi; 
		BOOL bSuccess;
		// ��¼�¿���̨�Ĵ�С
		SMALL_RECT srWindowRect; 
		COORD coordScreen;

		bSuccess = ::GetConsoleScreenBufferInfo(m_hOutput, &csbi);

		// ��ÿ���̨�Ĵ�С�����ֵ
		coordScreen = ::GetLargestConsoleWindowSize(m_hOutput);

		// �����¿���̨���ڴ�С�Լ�������λ��
		srWindowRect.Right = (SHORT)(min(xSize, coordScreen.X) - 1);
		srWindowRect.Bottom = (SHORT)(min(ySize, coordScreen.Y) - 1);
		srWindowRect.Left = srWindowRect.Top = (SHORT) 0;

		// �����¿���̨�����С
		coordScreen.X = xSize;
		coordScreen.Y = ySize;
		// �����ǰ����Ĵ�С��������Ҫ���õĻ����С,���������ô��ڴ�С,�����û����С
		if ((u32) csbi.dwSize.X * csbi.dwSize.Y > (u32) xSize * ySize)
		{
			bSuccess = ::SetConsoleWindowInfo(m_hOutput, TRUE, &srWindowRect);
			bSuccess = ::SetConsoleScreenBufferSize(m_hOutput, coordScreen);
		}
		// �����ǰ����Ĵ�СС������Ҫ���õĻ����С,���������û����С,�����ô��ڴ�С
		if ((u32) csbi.dwSize.X * csbi.dwSize.Y < (u32) xSize * ySize)
		{
			bSuccess = ::SetConsoleScreenBufferSize(m_hOutput, coordScreen);
			bSuccess = ::SetConsoleWindowInfo(m_hOutput, TRUE, &srWindowRect);
		}
		return;
	}

#endif