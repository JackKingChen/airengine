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
	// 名称:  CIME
	// 功能:  构造函数, 通过DisableIme初始化一些数据
	// 参数:  无
	// 返回:  无
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
	// 名称:  ~CIME
	// 功能:  析构函数
	// 参数:  无
	// 返回:  无
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
	// 名称:  Initialize
	// 功能:  初始化IME输入法
	// 参数:  无
	// 返回:  初始化成功
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
	// 名称:  Release
	// 功能:  释放IME输入法单实体类
	// 参数:  无
	// 返回:  无
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
	//                        控制函数
	//=========================================================

	//---------------------------------------------------------
	// 名称:  SetIMEKey
	// 功能:  设置IME功能按键
	// 参数:  nKeyNextIME - 下一种输入法
	//        nKeyPrevIME - 上一种输入法
	//        nKeySharpIME - 半角/全角
	//        nKeySymbolIME - 中英文标点
	//        nKeyEnableIME - 打开IME输入法
	//        nKeyDisableIME - 关闭IME输入法
	// 返回:  无
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
	// 名称:  DisableIme
	// 功能:  关闭并禁止输入法，如ime已经打开则关闭，此后玩家
	//        不能用热键呼出ime
	// 参数:  无
	// 返回:  无
	//---------------------------------------------------------
	void CIME::DisableIme( void )
	{
        bool disableIMESuccess = true;
        char firstname[256] = {0};
        HIMC hIMC = ImmGetContext( ENGINE_INST->GetHWnd() );
        ImmEscape( GetKeyboardLayout( 0 ), hIMC, IME_ESC_IME_NAME, firstname );
        ImmReleaseContext( ENGINE_INST->GetHWnd(), hIMC );

		// 如果ime打开通过循环切换到下一个关闭
        // 首先查询当前激活的输入法信息是否IME输入法
        while( ImmIsIME( GetKeyboardLayout( 0 )))
        {
            HIMC hIMC = ImmGetContext( ENGINE_INST->GetHWnd() );
            // 取得新输入法名字
            ImmEscape( GetKeyboardLayout( 0 ), hIMC, IME_ESC_IME_NAME, m_szImeName );
            ImmReleaseContext( ENGINE_INST->GetHWnd(), hIMC );

            if ( AIRString(firstname) == AIRString(m_szImeName) )
            {
                disableIMESuccess = false;
                break;
            }

            // 如果当前的输入法是IME输入法,就切换下一个输入法
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
	// 名称:  EnableIme
	// 功能:  允许输入法，此后玩家可以用热键呼出ime
	// 参数:  无
	// 返回:  无
	//---------------------------------------------------------
	void CIME::EnableIme()
	{
		m_bIme = true;
	}

	//---------------------------------------------------------
	// 名称:  PrevIme
	// 功能:  切换到上一种输入法，必须EnableIme后才有效
	// 参数:  无
	// 返回:  无
	//---------------------------------------------------------
	void CIME::PrevIme( void )
	{
		if( m_bIme )
			ActivateKeyboardLayout( (HKL)HKL_PREV, 0 );
	}

	//---------------------------------------------------------
	// 名称:  NextIme
	// 功能:  切换到下一种输入法，必须EnableIme后才有效
	// 参数:  无
	// 返回:  无
	//---------------------------------------------------------
	void CIME::NextIme()
	{
		if( m_bIme )
			ActivateKeyboardLayout( (HKL)HKL_NEXT, 0 );
	}

	//---------------------------------------------------------
	// 名称:  SharpIme
	// 功能:  切换全角/半角
	// 参数:  窗口HANDLE
	// 返回:  无
	//---------------------------------------------------------
	void CIME::SharpIme( HWND hWnd )
	{
		ImmSimulateHotKey( hWnd, IME_CHOTKEY_SHAPE_TOGGLE );
	}

	//---------------------------------------------------------
	// 名称:  SymbolIme
	// 功能:  切换中/英文标点
	// 参数:  窗口HANDLE
	// 返回:  无
	//---------------------------------------------------------
	void CIME::SymbolIme( HWND hWnd )
	{
		ImmSimulateHotKey( hWnd, IME_CHOTKEY_SYMBOL_TOGGLE );
	}

	//=========================================================
	//                        状态函数
	//=========================================================

	//---------------------------------------------------------
	// 名称:  GetImeName
	// 功能:  得到输入法名字，如果当前是英文则返回NULL
	// 参数:  无
	// 返回:  得到输入法名字
	//---------------------------------------------------------
	char* CIME::GetImeName()
	{
		return m_szImeName[ 0 ]? m_szImeName : NULL;
	}

	//---------------------------------------------------------
	// 名称:  IsImeSharp
	// 功能:  是否全角
	// 参数:  无
	// 返回:  是否全角
	//---------------------------------------------------------
	bool CIME::IsImeSharp()
	{
		return m_bImeSharp;
	}

	//---------------------------------------------------------
	// 名称:  IsImeSymbol
	// 功能:  是否中文标点
	// 参数:  无
	// 返回:  是否中文标点
	//---------------------------------------------------------
	bool CIME::IsImeSymbol()
	{
		return m_bImeSymbol;
	}

	//---------------------------------------------------------
	// 名称:  GetImeInput
	// 功能:  得到输入法状态，四个指针任意可为NULL则此状态不回返回
	// 参数:  pszCompStr     - 返回转换后的串
	//        pszCompReadStr - 返回键盘直接输入的串
	//        pnImeCursor    - 返回szCompStr的光标位置
	//        pszCandList    - 返回选字表，每项之间以\t分隔
	// 返回:  无
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
	// 名称:  GetLastInputMessageW
	// 功能:  获得上一次的输入结果
	// 参数:  无
	// 返回:  上一次的输入结果
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
	//                     消息函数
	//=========================================================

	//---------------------------------------------------------
	// 名称:  OnChar
	// 功能:  处理字符输入
	// 参数:  hWnd - 窗口句柄; message - 消息; 
	//        wParam - 参数; lParam - 参数
	// 返回:  无
	//---------------------------------------------------------
	void CIME::OnChar(  HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
        if( !m_bIme )
            return;

        // 如果是后退键或删除键，直接返回
        if( wParam ==  VK_BACK || wParam == VK_DELETE || wParam == VK_LEFT || wParam == VK_RETURN)
            return;

        /*
		//================================================
		//! add by hjj 2008-12-21 begin: <<<<<<<
		//! 如果这时有CTRL键按下, 则直接返回
		if( INPUTSYSTEM->KeyDown( DIK_LCONTROL ) ||
			INPUTSYSTEM->KeyDown( DIK_RCONTROL ) )
			return;
		//! add by hjj 2008-12-21 end    >>>>>>>
		//================================================

		// 如果是后退键或删除键，直接返回
		if( wParam ==  KEY_BACK )
			return;

		// 如果是RETURN, 保存结果, 清空临时字符串
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

		// 计算多字节串的长度
		size_t len = strlen( m_szText );
		size_t LEN = wcslen( m_wcText );
		if( LEN >= MAX_TEXT - 1 )
		{
			char buf[100];
			snprintf( buf, 100, "你输入的字符个数不能超过%d!", MAX_TEXT-1 );
			ERRORMSG( buf );
			return;
		}

		// 如果长度大于0
		if( len )
		{
			// 从插入光标处插入字符
			for( char* p = m_szText + len + 1; p != m_pszText; --p )
			{
				*p = *(p-1);
			}
			// 移动插入光标
			*m_pszText++ = (char)wParam;
		}
		// 如果多字节串长度为0
		else
		{
			// 从起点处开始插入
			*m_pszText++ = (char)wParam;
			*m_pszText = 0;
		}

		// 计算多字节串对应的宽字节串长度
		int WCStrLen = wcslen( m_wcText );
		// 将插入字符后的多字节串转化为宽字节串
		MByteToWChar( m_szText, m_wcText, sizeof(m_szText) );
		// 计算插入多少个新字符,然后移动光标
		m_nIndex += ( wcslen(m_wcText) - WCStrLen );

		// 下面是根据多字节串的插入指针 m_pszText 
		// 来同步校正相应的宽字节串的插入指针 m_pwcText
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
	// 名称:  OnKeyDown
	// 功能:  处理IME的输入法按键事件
	// 参数:  hWnd - 窗口句柄; message - 消息; 
	//        wParam - 参数; lParam - 参数
	// 返回:  无
	//---------------------------------------------------------
	void CIME::OnKeyDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		// 上一种输入法
		if( wParam == m_nKeyPrevIME )
			PrevIme();
		// 下一种输入法
		else if( wParam == m_nKeyNextIME )
			NextIme();
		// 全角/半角转换
		else if( wParam == m_nKeySharpIME )
			SharpIme( hWnd/*WND_INFO_PTR->GetHWND()*/);
		// 中/英文标点转换
		else if( wParam == m_nKeySymbolIME )
			SymbolIme( hWnd/*WND_INFO_PTR->GetHWND()*/);
		// 启用IME输入法
		else if( wParam == m_nKeyEnableIME )
			EnableIme();
		// 关闭IME输入法
		else if( wParam == m_nKeyDisableIME )
			DisableIme();
		//// 左移输入光标
		//else if( KEY_LEFT == wParam )
		//	MoveCursorLeft();
		//// 右移光标
		//else if( KEY_RIGHT == wParam )
		//	MoveCursorRight();
		//// 前向删除
		//else if( KEY_BACK == wParam )
		//	ForwardDelete();
		//// 后向删除
		//else if( KEY_DELETE == wParam )
		//	BackwardDelete();
	}

	//---------------------------------------------------------
	// 名称:  OnKeyUp
	// 功能:  按键弹起的消息处理
	// 参数:  hWnd - 窗口句柄; message - 消息; 
	//        wParam - 参数; lParam - 参数
	// 返回:  无
	//---------------------------------------------------------
	void CIME::OnKeyUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{

	}

	//---------------------------------------------------------
	// 名称:  OnWM_INPUTLANGCHANGEREQUEST
	// 功能:  必须在消息中调用的函数，如果返回是true，
	//        则窗口函数应直接返回0，否则应传递给DefWindowProc
	// 参数:  无
	// 返回:  请求改变输入法
	//---------------------------------------------------------
	bool CIME::OnWM_INPUTLANGCHANGEREQUEST()
	{
		// 如果禁止ime则返回false，此时窗口函数应返回0，
		// 否则DefWindowProc会打开输入法
		return !m_bIme;
	}

	//---------------------------------------------------------
	// 名称:  OnWM_INPUTLANGCHANGE
	// 功能:  改变输入法
	// 参数:  窗口HANDLE
	// 返回:  修改成功?
	//---------------------------------------------------------
	bool CIME::OnWM_INPUTLANGCHANGE( HWND hWnd )
	{
		// ime改变
		HKL hKL = GetKeyboardLayout( 0 );
		// 中文输入
		if( ImmIsIME( hKL ))
		{
			HIMC hIMC = ImmGetContext( hWnd );
			// 取得新输入法名字
			ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, m_szImeName );
			DWORD dwConversion, dwSentence;
			ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
			// 取得全角标志
			m_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
			// 取得中文标点标志
			m_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
			ImmReleaseContext( hWnd, hIMC );
		}
		// 英文输入
		else
		{
			//m_szImeName[ 0 ] = 0;
			snprintf( m_szImeName, 256, "EN" );
		}
		// 总是返回false，因为需要窗口函数调用DefWindowProc继续处理
		return false;
	}

	//---------------------------------------------------------
	// 名称:  OnWM_IME_SETCONTEXT
	// 功能:  IME选择内容
	// 参数:  无
	// 返回:  选择成功
	//---------------------------------------------------------
	bool CIME::OnWM_IME_SETCONTEXT()
	{ 
		return true; 
	}

	//---------------------------------------------------------
	// 名称:  OnWM_IME_STARTCOMPOSITION
	// 功能:  IME开始合成文字
	// 参数:  无
	// 返回:  TRUE
	//---------------------------------------------------------
	bool CIME::OnWM_IME_STARTCOMPOSITION()
	{ 
		return true; 
	}

	//---------------------------------------------------------
	// 名称:  OnWM_IME_ENDCOMPOSITION
	// 功能:  IME结束合成文字
	// 参数:  无
	// 返回:  TRUE
	//---------------------------------------------------------
	bool CIME::OnWM_IME_ENDCOMPOSITION()
	{ 
		return true; 
	}

	//---------------------------------------------------------
	// 名称:  OnWM_IME_NOTIFY
	// 功能:  IME修改输入法状态
	// 参数:  hWnd - 窗口句柄;  wParam - WPARAM参数
	// 返回:  总是返回true，防止ime窗口打开
	//---------------------------------------------------------
	bool CIME::OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam )
	{
		HIMC  hIMC;
		u32 dwSize;
		DWORD dwConversion, dwSentence;
		switch( wParam )
		{
			// 全角/半角，中/英文标点改变
		case IMN_SETCONVERSIONMODE:
			hIMC = ImmGetContext( hWnd );
			ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
			m_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
			m_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
			ImmReleaseContext( hWnd, hIMC );
			break;

			// 进入选字状态
		case IMN_OPENCANDIDATE:

			// 选字表翻页
		case IMN_CHANGECANDIDATE:
			hIMC = ImmGetContext( hWnd );
			// 释放以前的选字表
			if( m_lpCandList )
			{
				GlobalFree( (HANDLE)m_lpCandList );
				m_lpCandList = NULL;
			}
			// 得到新的选字表
			if( dwSize = ImmGetCandidateList( hIMC, 0, NULL, 0 ))
			{
				m_lpCandList = (LPCANDIDATELIST)GlobalAlloc( GPTR, dwSize );
				if( m_lpCandList )
					ImmGetCandidateList( hIMC, 0, m_lpCandList, dwSize );
			}
			ImmReleaseContext( hWnd, hIMC );
			// 选字表整理成串
			if( m_lpCandList )
            {
                ConvertCandList( m_lpCandList, m_szCandList );
            }
			break;

			// 关闭选字表
		case IMN_CLOSECANDIDATE:
			//释放
			if( m_lpCandList )
			{
				GlobalFree( (HANDLE)m_lpCandList );
				m_lpCandList = NULL;
			}
			m_szCandList[ 0 ] = 0;
			break;
		}

		// 总是返回true，防止ime窗口打开
		return true;
	}

	//---------------------------------------------------------
	// 名称:  OnWM_IME_COMPOSITION
	// 功能:  获得输入法的信息字符串
	// 参数:  hWnd - 窗口句柄;  wParam - WPARAM参数
	// 返回:  总是返回true，防止ime窗口打开
	//---------------------------------------------------------
	bool CIME::OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam )
	{
		HIMC  hIMC;
		u32 dwSize;
		hIMC = ImmGetContext( hWnd );

        bool bComplete = false;

		// 取得szCompStr
		if( lParam & GCS_COMPSTR )
		{
			dwSize = ImmGetCompositionString( hIMC, GCS_COMPSTR, (void*)m_szCompStr, sizeof( m_szCompStr ));
			m_szCompStr[ dwSize ] = 0;
		}

		// 取得szCompReadStr
		if( lParam & GCS_COMPREADSTR )
		{
			dwSize = ImmGetCompositionString( hIMC, GCS_COMPREADSTR, (void*)m_szCompReadStr, sizeof( m_szCompReadStr ));
			m_szCompReadStr[ dwSize ] = 0;
		}

		// 取得nImeCursor
		if( lParam & GCS_CURSORPOS )
		{
			m_nImeCursor = 0xffff & ImmGetCompositionString( hIMC, GCS_CURSORPOS, NULL, 0 );
		}

		// 取得汉字输入串
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

			// 转成WM_CHAR消息
			//while( *p )
			//	PostMessage( hWnd, WM_CHAR, (WPARAM)(*p++), 1 );
		}

		ImmReleaseContext( hWnd, hIMC );

		// 总是返回true，防止ime窗口打开
		return bComplete;
	}

	//---------------------------------------------------------
	// 名称:  GetTextMessage
	// 功能:  获得当前的输入字符串
	// 参数:  无
	// 返回:  当前的输入字符串指针
	//---------------------------------------------------------
	char* CIME::GetTextMessage( void )
	{
		return m_szText;
	}

	//---------------------------------------------------------
	// 名称:  GetTextMessageW
	// 功能:  获得当前的输入字符串
	// 参数:  无
	// 返回:  当前的输入字符串指针
	//---------------------------------------------------------
	wchar_t* CIME::GetTextMessageW( void )
	{
		return m_wcText;
	}

	//---------------------------------------------------------
	// 名称:  SetTextMessage
	// 功能:  设置当前的输入字符串
	// 参数:  szText - 设置字符串
	// 返回:  无
	//---------------------------------------------------------
	void CIME::SetTextMessage( char* szText )
	{
		if( NULL == szText )
			m_szText[0] = 0;
		snprintf( m_szText, DUB_MAX_TEX, "%s", szText );
	}

	//---------------------------------------------------------
	// 名称:  GetCursorPos
	// 功能:  取得光标的位置
	// 参数:  无
	// 返回:  光标的位置
	//---------------------------------------------------------
	int CIME::GetCursorPos( void )
	{
		return m_nIndex;
	}

	//---------------------------------------------------------
	// 名称:  GetLastInputMessage
	// 功能:  获得上一次的输入结果
	// 参数:  无
	// 返回:  上一次的输入结果
	//---------------------------------------------------------
	const AIRString& CIME::GetLastInputMessage( void )
	{
		return m_strResult;
	}


	//---------------------------------------------------------
	// 名称:  ConvertCandList
	// 功能:  将选字表整理成串
	// 参数:  pCandList - 选字列表; pszCandList - 选字表字符串
	// 返回:  无
	//---------------------------------------------------------
	void CIME::ConvertCandList( CANDIDATELIST *pCandList, char *pszCandList )
	{
		// 转换CandidateList到一个串，\t分隔每一项
		unsigned int i;
		if( pCandList->dwCount < pCandList->dwSelection )
		{
			pszCandList[ 0 ] = 0;
			return;
		}

		// 待选字序号超出总数，微软拼音第二次到选字表最后一页后
		// 再按PageDown会出现这种情况，并且会退出选字状态，开始
		// 一个新的输入.
		//
		// 但微软拼音自己的ime窗口可以解决这个问题，估计微软拼音
		// 实现了更多的接口，所以使用了这种不太标准的数据
		// 
		// 我现在无法解决这个问题，而且实际使用中也很少遇到这种
		// 事，而且其它标准输入法不会引起这种bug
		// 
		// 非标准输入法估计实现的接口比较少，所以应该也不会引起
		// 这种bug
		for( i = 0; 
			( i < pCandList->dwCount - pCandList->dwSelection ) && ( i < pCandList->dwPageSize );
			i++ )
		{
			// 每项对应的数字键
			*pszCandList++ = ( i % 10 != 9 )? i % 10 + '1' : '0';
			// 用'.'分隔
			*pszCandList++ = '.';
			// 每项实际的内容
			strcpy( pszCandList, (char*)pCandList
				+ pCandList->dwOffset[ pCandList->dwSelection + i ] );
			pszCandList += strlen( pszCandList );
			// 项之间以'\t'分隔
			*pszCandList++ = '\t';
			*pszCandList++ = ' ';
		}
		// 串尾，并覆盖最后一个'\t'
		*( pszCandList - 1 )= 0;
	}

    /*
	//---------------------------------------------------------
	// 名称:  MoveCursorLeft
	// 功能:  按下左键, 向左移动光标
	// 参数:  无
	// 返回:  无
	//---------------------------------------------------------
	void CIME::MoveCursorLeft( void )
	{
		// 如果光标处于最头端, 直接返回
		if( m_pwcText == m_wcText )
			return;
		// 否则,光标位置减一
		--m_pwcText;
		// 同步校正多字节串里的插入指针
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
		// 光标位置减一
		m_nIndex--;
	}

	//---------------------------------------------------------
	// 名称:  MoveCursorRight
	// 功能:  按下右键, 向右移动光标
	// 参数:  无
	// 返回:  无
	//---------------------------------------------------------
	void CIME::MoveCursorRight()
	{
		// 如果光标处于最后端, 直接返回
		size_t len = wcslen( m_wcText );
		if( m_pwcText == m_wcText+len )
			return;
		// 宽字节串插入指针位置加1
		++m_pwcText;
		// 同步校正多字节串里的插入指针
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
		// 光标位置加1
		m_nIndex++;
	}

	//---------------------------------------------------------
	// 名称:  ForwardDelete
	// 功能:  删除光标前面的一个字符
	// 参数:  无
	// 返回:  无
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

		// 同步校正多字节串里的插入指针
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
	// 名称:  BackwardDelete
	// 功能:  删除光标后面的一个字符
	// 参数:  无
	// 返回:  无
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

		// 同步校正多字节串里的插入指针
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


