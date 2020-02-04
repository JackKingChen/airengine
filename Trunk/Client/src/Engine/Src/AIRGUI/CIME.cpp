//#include "Stdafx.h"
#include "CIME.h"
//#include "InputSystem.h"
#include "Util.h"
//#include "AuroWndInfo.h"
//#include "SystemMgr.h"

#include "Engine.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

//namespace AURO
//{

	//---------------------------------------------------------
	// ����:  CIME
	// ����:  ���캯��, ͨ��DisableIme��ʼ��һЩ����
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	CIME::CIME() : m_lpCandList( NULL )
	{ 
		//DisableIme(); 
		//m_szText[0] = 0;
		//m_pszText   = m_szText;
		memset(m_szText, 0, sizeof(m_szText));
        memset(m_wcText, 0, sizeof(m_wcText));
		m_pwcText   = m_wcText;
		m_nIndex    = 0;
		m_strResult.clear();
		//SetIMEKey();
	}

	//---------------------------------------------------------
	// ����:  ~CIME
	// ����:  ��������
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	CIME::~CIME()
	{
		DisableIme();
		if( m_lpCandList )
		{
			GlobalFree( (HANDLE)m_lpCandList );
			m_lpCandList = NULL;
		}
		m_nIndex = 0;
	}


	//---------------------------------------------------------
	// ����:  Initialize
	// ����:  ��ʼ��IME���뷨
	// ����:  ��
	// ����:  ��ʼ���ɹ�
	//---------------------------------------------------------
	bool CIME::Initialize( void )
	{
		m_lpCandList = NULL;
		DisableIme(); 
		//SetIMEKey();
		EnableIme();
		//m_szText[0] = 0;
		//m_pszText   = m_szText;
        memset(m_szText, 0, sizeof(m_szText));
        memset(m_wcText, 0, sizeof(m_wcText));
		m_pwcText   = m_wcText;
		m_nIndex    = 0;
		return true;
	}

	//---------------------------------------------------------
	// ����:  Release
	// ����:  �ͷ�IME���뷨��ʵ����
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::Release( void )
	{
		DisableIme();
		if( m_lpCandList )
		{
			GlobalFree( (HANDLE)m_lpCandList );
			m_lpCandList = NULL;
		}
		m_nIndex = 0;
	}

	//=========================================================
	//                        ���ƺ���
	//=========================================================

	//---------------------------------------------------------
	// ����:  SetIMEKey
	// ����:  ����IME���ܰ���
	// ����:  nKeyNextIME - ��һ�����뷨
	//        nKeyPrevIME - ��һ�����뷨
	//        nKeySharpIME - ���/ȫ��
	//        nKeySymbolIME - ��Ӣ�ı��
	//        nKeyEnableIME - ��IME���뷨
	//        nKeyDisableIME - �ر�IME���뷨
	// ����:  ��
	//---------------------------------------------------------
    
	//void CIME::SetIMEKey(
	//	int nKeyPrevIME /* = KEY_F1 */, 
	//	int nKeyNextIME /* = KEY_F2 */, 
	//	int nKeySharpIME /* = KEY_F3 */, 
	//	int nKeySymbolIME /* = KEY_F4 */, 
	//	int nKeyEnableIME /* = KEY_F5 */, 
	//	int nKeyDisableIME /* = KEY_F6  */ )
	//{
	//	this->m_nKeyPrevIME = nKeyPrevIME;
	//	this->m_nKeyDisableIME = nKeyDisableIME;
	//	this->m_nKeyEnableIME = nKeyEnableIME;
	//	this->m_nKeyNextIME = nKeyNextIME;
	//	this->m_nKeySharpIME = nKeySharpIME;
	//	this->m_nKeySymbolIME = nKeySymbolIME;
	//}

	//---------------------------------------------------------
	// ����:  DisableIme
	// ����:  �رղ���ֹ���뷨����ime�Ѿ�����رգ��˺����
	//        �������ȼ�����ime
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::DisableIme( void )
	{
        bool disableIMESuccess = true;
        char firstname[256] = {0};
        HIMC hIMC = ImmGetContext( ENGINE_INST->GetHWnd() );
        ImmEscape( GetKeyboardLayout( 0 ), hIMC, IME_ESC_IME_NAME, firstname );
        ImmReleaseContext( ENGINE_INST->GetHWnd(), hIMC );

		// ���ime��ͨ��ѭ���л�����һ���ر�
        // ���Ȳ�ѯ��ǰ��������뷨��Ϣ�Ƿ�IME���뷨
        while( ImmIsIME( GetKeyboardLayout( 0 )))
        {
            HIMC hIMC = ImmGetContext( ENGINE_INST->GetHWnd() );
            // ȡ�������뷨����
            ImmEscape( GetKeyboardLayout( 0 ), hIMC, IME_ESC_IME_NAME, m_szImeName );
            ImmReleaseContext( ENGINE_INST->GetHWnd(), hIMC );

            if ( AIRString(firstname) == AIRString(m_szImeName) )
            {
                disableIMESuccess = false;
                break;
            }

            // �����ǰ�����뷨��IME���뷨,���л���һ�����뷨
	        ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );
        }

        if ( disableIMESuccess )
        {
            m_bIme               = false;
            m_szImeName[ 0 ]     = 0;
            m_szCompStr[ 0 ]     = 0;
            m_szCompReadStr[ 0 ] = 0;
            m_szCandList[ 0 ]    = 0;
            m_nImeCursor         = 0;
        }
	}

	//---------------------------------------------------------
	// ����:  EnableIme
	// ����:  �������뷨���˺���ҿ������ȼ�����ime
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::EnableIme()
	{
		m_bIme = true;
	}

	//---------------------------------------------------------
	// ����:  PrevIme
	// ����:  �л�����һ�����뷨������EnableIme�����Ч
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::PrevIme( void )
	{
		if( m_bIme )
			ActivateKeyboardLayout( (HKL)HKL_PREV, 0 );
	}

	//---------------------------------------------------------
	// ����:  NextIme
	// ����:  �л�����һ�����뷨������EnableIme�����Ч
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::NextIme()
	{
		if( m_bIme )
			ActivateKeyboardLayout( (HKL)HKL_NEXT, 0 );
	}

	//---------------------------------------------------------
	// ����:  SharpIme
	// ����:  �л�ȫ��/���
	// ����:  ����HANDLE
	// ����:  ��
	//---------------------------------------------------------
	void CIME::SharpIme( HWND hWnd )
	{
		ImmSimulateHotKey( hWnd, IME_CHOTKEY_SHAPE_TOGGLE );
	}

	//---------------------------------------------------------
	// ����:  SymbolIme
	// ����:  �л���/Ӣ�ı��
	// ����:  ����HANDLE
	// ����:  ��
	//---------------------------------------------------------
	void CIME::SymbolIme( HWND hWnd )
	{
		ImmSimulateHotKey( hWnd, IME_CHOTKEY_SYMBOL_TOGGLE );
	}

	//=========================================================
	//                        ״̬����
	//=========================================================

	//---------------------------------------------------------
	// ����:  GetImeName
	// ����:  �õ����뷨���֣������ǰ��Ӣ���򷵻�NULL
	// ����:  ��
	// ����:  �õ����뷨����
	//---------------------------------------------------------
	char* CIME::GetImeName()
	{
		return m_szImeName[ 0 ]? m_szImeName : NULL;
	}

	//---------------------------------------------------------
	// ����:  IsImeSharp
	// ����:  �Ƿ�ȫ��
	// ����:  ��
	// ����:  �Ƿ�ȫ��
	//---------------------------------------------------------
	bool CIME::IsImeSharp()
	{
		return m_bImeSharp;
	}

	//---------------------------------------------------------
	// ����:  IsImeSymbol
	// ����:  �Ƿ����ı��
	// ����:  ��
	// ����:  �Ƿ����ı��
	//---------------------------------------------------------
	bool CIME::IsImeSymbol()
	{
		return m_bImeSymbol;
	}

	//---------------------------------------------------------
	// ����:  GetImeInput
	// ����:  �õ����뷨״̬���ĸ�ָ�������ΪNULL���״̬���ط���
	// ����:  pszCompStr     - ����ת����Ĵ�
	//        pszCompReadStr - ���ؼ���ֱ������Ĵ�
	//        pnImeCursor    - ����szCompStr�Ĺ��λ��
	//        pszCandList    - ����ѡ�ֱ�ÿ��֮����\t�ָ�
	// ����:  ��
	//---------------------------------------------------------
	void CIME::GetImeInput( 
		char **pszCompStr, 
		char **pszCompReadStr, 
		int  *pnImeCursor, 
		char **pszCandList )
	{
		if( pszCompStr )     *pszCompStr     = m_szCompStr;
		if( pszCompReadStr ) *pszCompReadStr = m_szCompReadStr;
		if( pnImeCursor )    *pnImeCursor    = m_nImeCursor;
		if( pszCandList )    *pszCandList    = m_szCandList;
	}

	//---------------------------------------------------------
	// ����:  GetLastInputMessageW
	// ����:  �����һ�ε�������
	// ����:  ��
	// ����:  ��һ�ε�������
	//---------------------------------------------------------
	//stringw CIME::GetLastInputMessageW( void )
	//{
	//	return m_wszResult;
	//}

	bool CIME::GetIMEOpen( void )
	{
		return m_bIme;
	}

    /*
	stringw CIME::GetTextW( void )
	{
		stringw buf(m_wcText);
		m_strResult.clear();
		m_strResult = AIRString( m_szText );
		m_wszResult.append( m_wcText );
		m_szText[0] = 0;
		m_wcText[0] = 0;
		m_pwcText   = m_wcText;
		//m_pszText   = m_szText;
		m_nIndex    = 0;
		return buf;
	}
    */

	void CIME::ClearBuffer( void )
	{
		m_strResult.clear();
		m_szText[0] = 0;
		m_wcText[0] = 0;
		m_pwcText   = m_wcText;
		//m_pszText   = m_szText;
		m_nIndex    = 0;
	}


	//=========================================================
	//                     ��Ϣ����
	//=========================================================

	//---------------------------------------------------------
	// ����:  OnChar
	// ����:  �����ַ�����
	// ����:  hWnd - ���ھ��; message - ��Ϣ; 
	//        wParam - ����; lParam - ����
	// ����:  ��
	//---------------------------------------------------------
	void CIME::OnChar(  HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
        if( !m_bIme )
            return;

        // ����Ǻ��˼���ɾ������ֱ�ӷ���
        if( wParam ==  VK_BACK || wParam == VK_DELETE || wParam == VK_LEFT || wParam == VK_RETURN)
            return;

        /*
		//================================================
		//! add by hjj 2008-12-21 begin: <<<<<<<
		//! �����ʱ��CTRL������, ��ֱ�ӷ���
		if( INPUTSYSTEM->KeyDown( DIK_LCONTROL ) ||
			INPUTSYSTEM->KeyDown( DIK_RCONTROL ) )
			return;
		//! add by hjj 2008-12-21 end    >>>>>>>
		//================================================

		// ����Ǻ��˼���ɾ������ֱ�ӷ���
		if( wParam ==  KEY_BACK )
			return;

		// �����RETURN, ������, �����ʱ�ַ���
		if( wParam == KEY_RETURN )
		{
			//m_strResult.clear();
			//m_strResult = AIRString( m_szText );
			//m_wszResult.append( m_wcText );

			//m_szText[0] = 0;
			//m_wcText[0] = 0;
			//m_pwcText   = m_wcText;
			//m_pszText   = m_szText;
			//m_nIndex    = 0;
			//return;
			wParam = '\n';
		}

		// ������ֽڴ��ĳ���
		size_t len = strlen( m_szText );
		size_t LEN = wcslen( m_wcText );
		if( LEN >= MAX_TEXT - 1 )
		{
			char buf[100];
			snprintf( buf, 100, "��������ַ��������ܳ���%d!", MAX_TEXT-1 );
			ERRORMSG( buf );
			return;
		}

		// ������ȴ���0
		if( len )
		{
			// �Ӳ����괦�����ַ�
			for( char* p = m_szText + len + 1; p != m_pszText; --p )
			{
				*p = *(p-1);
			}
			// �ƶ�������
			*m_pszText++ = (char)wParam;
		}
		// ������ֽڴ�����Ϊ0
		else
		{
			// ����㴦��ʼ����
			*m_pszText++ = (char)wParam;
			*m_pszText = 0;
		}

		// ������ֽڴ���Ӧ�Ŀ��ֽڴ�����
		int WCStrLen = wcslen( m_wcText );
		// �������ַ���Ķ��ֽڴ�ת��Ϊ���ֽڴ�
		MByteToWChar( m_szText, m_wcText, sizeof(m_szText) );
		// ���������ٸ����ַ�,Ȼ���ƶ����
		m_nIndex += ( wcslen(m_wcText) - WCStrLen );

		// �����Ǹ��ݶ��ֽڴ��Ĳ���ָ�� m_pszText 
		// ��ͬ��У����Ӧ�Ŀ��ֽڴ��Ĳ���ָ�� m_pwcText
		wchar_t wcstr[MAX_TEXT];
		char cstr[DUB_MAX_TEX];
		char *p = m_szText;
		int i = 0;
		while( p != m_pszText )
		{
			cstr[i++] = *p++;
		}
		cstr[i] = 0;
		MByteToWChar( cstr, wcstr, sizeof(cstr) );
		m_pwcText = m_wcText + wcslen(wcstr);
        */
/*((char)wParam >= 48 && (char)wParam <= 57) || ((char)wParam >= 65 && (char)wParam <= 90) ||
            ((char)wParam >= 97 && (char)wParam <= 122) || (char)wParam == ' '*/
        if ((char)wParam >= 33 && (char)wParam < 127 || (char)wParam == ' ')
        {
            m_szText[0] = (char)wParam;
            MByteToWChar( m_szText, m_wcText, sizeof(m_szText) );
            m_wcText[2] = 0;
        }
        
	}

	//---------------------------------------------------------
	// ����:  OnKeyDown
	// ����:  ����IME�����뷨�����¼�
	// ����:  hWnd - ���ھ��; message - ��Ϣ; 
	//        wParam - ����; lParam - ����
	// ����:  ��
	//---------------------------------------------------------
	void CIME::OnKeyDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		// ��һ�����뷨
		if( wParam == m_nKeyPrevIME )
			PrevIme();
		// ��һ�����뷨
		else if( wParam == m_nKeyNextIME )
			NextIme();
		// ȫ��/���ת��
		else if( wParam == m_nKeySharpIME )
			SharpIme( hWnd/*WND_INFO_PTR->GetHWND()*/);
		// ��/Ӣ�ı��ת��
		else if( wParam == m_nKeySymbolIME )
			SymbolIme( hWnd/*WND_INFO_PTR->GetHWND()*/);
		// ����IME���뷨
		else if( wParam == m_nKeyEnableIME )
			EnableIme();
		// �ر�IME���뷨
		else if( wParam == m_nKeyDisableIME )
			DisableIme();
		//// ����������
		//else if( KEY_LEFT == wParam )
		//	MoveCursorLeft();
		//// ���ƹ��
		//else if( KEY_RIGHT == wParam )
		//	MoveCursorRight();
		//// ǰ��ɾ��
		//else if( KEY_BACK == wParam )
		//	ForwardDelete();
		//// ����ɾ��
		//else if( KEY_DELETE == wParam )
		//	BackwardDelete();
	}

	//---------------------------------------------------------
	// ����:  OnKeyUp
	// ����:  �����������Ϣ����
	// ����:  hWnd - ���ھ��; message - ��Ϣ; 
	//        wParam - ����; lParam - ����
	// ����:  ��
	//---------------------------------------------------------
	void CIME::OnKeyUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{

	}

	//---------------------------------------------------------
	// ����:  OnWM_INPUTLANGCHANGEREQUEST
	// ����:  ��������Ϣ�е��õĺ��������������true��
	//        �򴰿ں���Ӧֱ�ӷ���0������Ӧ���ݸ�DefWindowProc
	// ����:  ��
	// ����:  ����ı����뷨
	//---------------------------------------------------------
	bool CIME::OnWM_INPUTLANGCHANGEREQUEST()
	{
		// �����ֹime�򷵻�false����ʱ���ں���Ӧ����0��
		// ����DefWindowProc������뷨
		return !m_bIme;
	}

	//---------------------------------------------------------
	// ����:  OnWM_INPUTLANGCHANGE
	// ����:  �ı����뷨
	// ����:  ����HANDLE
	// ����:  �޸ĳɹ�?
	//---------------------------------------------------------
	bool CIME::OnWM_INPUTLANGCHANGE( HWND hWnd )
	{
		// ime�ı�
		HKL hKL = GetKeyboardLayout( 0 );
		// ��������
		if( ImmIsIME( hKL ))
		{
			HIMC hIMC = ImmGetContext( hWnd );
			// ȡ�������뷨����
			ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, m_szImeName );
			DWORD dwConversion, dwSentence;
			ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
			// ȡ��ȫ�Ǳ�־
			m_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
			// ȡ�����ı���־
			m_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
			ImmReleaseContext( hWnd, hIMC );
		}
		// Ӣ������
		else
		{
			//m_szImeName[ 0 ] = 0;
			snprintf( m_szImeName, 256, "EN" );
		}
		// ���Ƿ���false����Ϊ��Ҫ���ں�������DefWindowProc��������
		return false;
	}

	//---------------------------------------------------------
	// ����:  OnWM_IME_SETCONTEXT
	// ����:  IMEѡ������
	// ����:  ��
	// ����:  ѡ��ɹ�
	//---------------------------------------------------------
	bool CIME::OnWM_IME_SETCONTEXT()
	{ 
		return true; 
	}

	//---------------------------------------------------------
	// ����:  OnWM_IME_STARTCOMPOSITION
	// ����:  IME��ʼ�ϳ�����
	// ����:  ��
	// ����:  TRUE
	//---------------------------------------------------------
	bool CIME::OnWM_IME_STARTCOMPOSITION()
	{ 
		return true; 
	}

	//---------------------------------------------------------
	// ����:  OnWM_IME_ENDCOMPOSITION
	// ����:  IME�����ϳ�����
	// ����:  ��
	// ����:  TRUE
	//---------------------------------------------------------
	bool CIME::OnWM_IME_ENDCOMPOSITION()
	{ 
		return true; 
	}

	//---------------------------------------------------------
	// ����:  OnWM_IME_NOTIFY
	// ����:  IME�޸����뷨״̬
	// ����:  hWnd - ���ھ��;  wParam - WPARAM����
	// ����:  ���Ƿ���true����ֹime���ڴ�
	//---------------------------------------------------------
	bool CIME::OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam )
	{
		HIMC  hIMC;
		u32 dwSize;
		DWORD dwConversion, dwSentence;
		switch( wParam )
		{
			// ȫ��/��ǣ���/Ӣ�ı��ı�
		case IMN_SETCONVERSIONMODE:
			hIMC = ImmGetContext( hWnd );
			ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
			m_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
			m_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
			ImmReleaseContext( hWnd, hIMC );
			break;

			// ����ѡ��״̬
		case IMN_OPENCANDIDATE:

			// ѡ�ֱ�ҳ
		case IMN_CHANGECANDIDATE:
			hIMC = ImmGetContext( hWnd );
			// �ͷ���ǰ��ѡ�ֱ�
			if( m_lpCandList )
			{
				GlobalFree( (HANDLE)m_lpCandList );
				m_lpCandList = NULL;
			}
			// �õ��µ�ѡ�ֱ�
			if( dwSize = ImmGetCandidateList( hIMC, 0, NULL, 0 ))
			{
				m_lpCandList = (LPCANDIDATELIST)GlobalAlloc( GPTR, dwSize );
				if( m_lpCandList )
					ImmGetCandidateList( hIMC, 0, m_lpCandList, dwSize );
			}
			ImmReleaseContext( hWnd, hIMC );
			// ѡ�ֱ�����ɴ�
			if( m_lpCandList )
            {
                ConvertCandList( m_lpCandList, m_szCandList );
            }
			break;

			// �ر�ѡ�ֱ�
		case IMN_CLOSECANDIDATE:
			//�ͷ�
			if( m_lpCandList )
			{
				GlobalFree( (HANDLE)m_lpCandList );
				m_lpCandList = NULL;
			}
			m_szCandList[ 0 ] = 0;
			break;
		}

		// ���Ƿ���true����ֹime���ڴ�
		return true;
	}

	//---------------------------------------------------------
	// ����:  OnWM_IME_COMPOSITION
	// ����:  ������뷨����Ϣ�ַ���
	// ����:  hWnd - ���ھ��;  wParam - WPARAM����
	// ����:  ���Ƿ���true����ֹime���ڴ�
	//---------------------------------------------------------
	bool CIME::OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam )
	{
		HIMC  hIMC;
		u32 dwSize;
		hIMC = ImmGetContext( hWnd );

        bool bComplete = false;

		// ȡ��szCompStr
		if( lParam & GCS_COMPSTR )
		{
			dwSize = ImmGetCompositionString( hIMC, GCS_COMPSTR, (void*)m_szCompStr, sizeof( m_szCompStr ));
			m_szCompStr[ dwSize ] = 0;
		}

		// ȡ��szCompReadStr
		if( lParam & GCS_COMPREADSTR )
		{
			dwSize = ImmGetCompositionString( hIMC, GCS_COMPREADSTR, (void*)m_szCompReadStr, sizeof( m_szCompReadStr ));
			m_szCompReadStr[ dwSize ] = 0;
		}

		// ȡ��nImeCursor
		if( lParam & GCS_CURSORPOS )
		{
			m_nImeCursor = 0xffff & ImmGetCompositionString( hIMC, GCS_CURSORPOS, NULL, 0 );
		}

		// ȡ�ú������봮
		if( lParam & GCS_RESULTSTR )
		{
			//unsigned char str[ MAX_PATH ];
			dwSize = ImmGetCompositionString( hIMC, GCS_RESULTSTR, (void*)m_szText, sizeof( m_szText ));
			m_szText[ dwSize ] = 0;
            memset(m_wcText, 0, sizeof(m_wcText));
            MByteToWChar( m_szText, m_wcText, sizeof(m_szText) );

            bComplete = true;
            //m_wcText[dwSize] = 0;
			//unsigned char *p = str;

			// ת��WM_CHAR��Ϣ
			//while( *p )
			//	PostMessage( hWnd, WM_CHAR, (WPARAM)(*p++), 1 );
		}

		ImmReleaseContext( hWnd, hIMC );

		// ���Ƿ���true����ֹime���ڴ�
		return bComplete;
	}

	//---------------------------------------------------------
	// ����:  GetTextMessage
	// ����:  ��õ�ǰ�������ַ���
	// ����:  ��
	// ����:  ��ǰ�������ַ���ָ��
	//---------------------------------------------------------
	char* CIME::GetTextMessage( void )
	{
		return m_szText;
	}

	//---------------------------------------------------------
	// ����:  GetTextMessageW
	// ����:  ��õ�ǰ�������ַ���
	// ����:  ��
	// ����:  ��ǰ�������ַ���ָ��
	//---------------------------------------------------------
	wchar_t* CIME::GetTextMessageW( void )
	{
		return m_wcText;
	}

	//---------------------------------------------------------
	// ����:  SetTextMessage
	// ����:  ���õ�ǰ�������ַ���
	// ����:  szText - �����ַ���
	// ����:  ��
	//---------------------------------------------------------
	void CIME::SetTextMessage( char* szText )
	{
		if( NULL == szText )
			m_szText[0] = 0;
		snprintf( m_szText, DUB_MAX_TEX, "%s", szText );
	}

	//---------------------------------------------------------
	// ����:  GetCursorPos
	// ����:  ȡ�ù���λ��
	// ����:  ��
	// ����:  ����λ��
	//---------------------------------------------------------
	int CIME::GetCursorPos( void )
	{
		return m_nIndex;
	}

	//---------------------------------------------------------
	// ����:  GetLastInputMessage
	// ����:  �����һ�ε�������
	// ����:  ��
	// ����:  ��һ�ε�������
	//---------------------------------------------------------
	const AIRString& CIME::GetLastInputMessage( void )
	{
		return m_strResult;
	}


	//---------------------------------------------------------
	// ����:  ConvertCandList
	// ����:  ��ѡ�ֱ�����ɴ�
	// ����:  pCandList - ѡ���б�; pszCandList - ѡ�ֱ��ַ���
	// ����:  ��
	//---------------------------------------------------------
	void CIME::ConvertCandList( CANDIDATELIST *pCandList, char *pszCandList )
	{
		// ת��CandidateList��һ������\t�ָ�ÿһ��
		unsigned int i;
		if( pCandList->dwCount < pCandList->dwSelection )
		{
			pszCandList[ 0 ] = 0;
			return;
		}

		// ��ѡ����ų���������΢��ƴ���ڶ��ε�ѡ�ֱ����һҳ��
		// �ٰ�PageDown�����������������һ��˳�ѡ��״̬����ʼ
		// һ���µ�����.
		//
		// ��΢��ƴ���Լ���ime���ڿ��Խ��������⣬����΢��ƴ��
		// ʵ���˸���Ľӿڣ�����ʹ�������ֲ�̫��׼������
		// 
		// �������޷����������⣬����ʵ��ʹ����Ҳ������������
		// �£�����������׼���뷨������������bug
		// 
		// �Ǳ�׼���뷨����ʵ�ֵĽӿڱȽ��٣�����Ӧ��Ҳ��������
		// ����bug
		for( i = 0; 
			( i < pCandList->dwCount - pCandList->dwSelection ) && ( i < pCandList->dwPageSize );
			i++ )
		{
			// ÿ���Ӧ�����ּ�
			*pszCandList++ = ( i % 10 != 9 )? i % 10 + '1' : '0';
			// ��'.'�ָ�
			*pszCandList++ = '.';
			// ÿ��ʵ�ʵ�����
			strcpy( pszCandList, (char*)pCandList
				+ pCandList->dwOffset[ pCandList->dwSelection + i ] );
			pszCandList += strlen( pszCandList );
			// ��֮����'\t'�ָ�
			*pszCandList++ = '\t';
			*pszCandList++ = ' ';
		}
		// ��β�����������һ��'\t'
		*( pszCandList - 1 )= 0;
	}

    /*
	//---------------------------------------------------------
	// ����:  MoveCursorLeft
	// ����:  �������, �����ƶ����
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::MoveCursorLeft( void )
	{
		// �����괦����ͷ��, ֱ�ӷ���
		if( m_pwcText == m_wcText )
			return;
		// ����,���λ�ü�һ
		--m_pwcText;
		// ͬ��У�����ֽڴ���Ĳ���ָ��
		wchar_t wcstr[MAX_TEXT];
		char cstr[DUB_MAX_TEX];
		wchar_t *p = m_wcText;
		int i = 0;
		while( p != m_pwcText )
		{
			wcstr[i++] = *p++;
		}
		wcstr[i] = 0;
		WCharToMByte( wcstr, cstr, sizeof(wcstr) );
		//m_pszText = m_szText + strlen(cstr);
		// ���λ�ü�һ
		m_nIndex--;
	}

	//---------------------------------------------------------
	// ����:  MoveCursorRight
	// ����:  �����Ҽ�, �����ƶ����
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::MoveCursorRight()
	{
		// �����괦������, ֱ�ӷ���
		size_t len = wcslen( m_wcText );
		if( m_pwcText == m_wcText+len )
			return;
		// ���ֽڴ�����ָ��λ�ü�1
		++m_pwcText;
		// ͬ��У�����ֽڴ���Ĳ���ָ��
		wchar_t wcstr[MAX_TEXT];
		char cstr[DUB_MAX_TEX];
		wchar_t *p = m_wcText;
		int i = 0;
		while( p != m_pwcText )
		{
			wcstr[i++] = *p++;
		}
		wcstr[i] = 0;
		WCharToMByte( wcstr, cstr, sizeof(wcstr) );
		//m_pszText = m_szText + strlen(cstr);
		// ���λ�ü�1
		m_nIndex++;
	}

	//---------------------------------------------------------
	// ����:  ForwardDelete
	// ����:  ɾ�����ǰ���һ���ַ�
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::ForwardDelete( void )
	{
		if( m_nIndex == 0 )
			return;
		--m_nIndex;
		wchar_t* p = --m_pwcText;
		int WCSLen = wcslen( m_wcText );
		while( p != m_wcText+WCSLen )
		{
			*p = *(p+1);
			++p;
		}
		*p = 0;
		WCharToMByte( m_wcText, m_szText, sizeof(m_wcText) );

		// ͬ��У�����ֽڴ���Ĳ���ָ��
		wchar_t wcstr[MAX_TEXT];
		char cstr[DUB_MAX_TEX];
		p = m_wcText;
		int i = 0;
		while( p != m_pwcText )
		{
			wcstr[i++] = *p++;
		}
		wcstr[i] = 0;
		WCharToMByte( wcstr, cstr, sizeof(wcstr) );
		m_pszText = m_szText + strlen(cstr);
	}

	//---------------------------------------------------------
	// ����:  BackwardDelete
	// ����:  ɾ���������һ���ַ�
	// ����:  ��
	// ����:  ��
	//---------------------------------------------------------
	void CIME::BackwardDelete( void )
	{
		if( m_nIndex == wcslen( m_wcText ) )
			return;
		wchar_t* p = m_pwcText;
		int WCSLen = wcslen( m_wcText );
		while( p != m_wcText+WCSLen )
		{
			*p = *(p+1);
			++p;
		}
		*p = 0;
		WCharToMByte( m_wcText, m_szText, sizeof(m_wcText) );

		// ͬ��У�����ֽڴ���Ĳ���ָ��
		wchar_t wcstr[MAX_TEXT];
		char cstr[DUB_MAX_TEX];
		p = m_wcText;
		int i = 0;
		while( p != m_pwcText )
		{
			wcstr[i++] = *p++;
		}
		wcstr[i] = 0;
		WCharToMByte( wcstr, cstr, sizeof(wcstr) );
		m_pszText = m_szText + strlen(cstr);
	}
    */

//}


#endif


