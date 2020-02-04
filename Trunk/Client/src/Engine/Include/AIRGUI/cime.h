//****************************************************************************//
//*	����	:  HJJ                                                           
//*	����    :  2008-11-28							                 
//* ����    :  CIME.h                                                 
//* ����    :  IME���뷨                                                        
//* ˵��    :                                                           
//*            Copyright (C) 2008 - All Rights Reserved                    
//****************************************************************************//


#ifndef  __CIME_INCLUDE__
#define  __CIME_INCLUDE__

#pragma once
//-------------------------------------------------------------
// ���ͷ�ļ�
//-------------------------------------------------------------
//#include "Stdafx.h"
#include "EngineConfig.h"


#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32


#include <windows.h>
#include <imm.h>
#include <string>
//#include "AuroWndInfo.h"
//#include "StringUtil.h"
//#include "MathLib.h"
#include "AIRGUI.h"
#include "MemoryAllocatorConfig.h"

#pragma comment ( lib, "imm32.lib" )

//namespace AURO
//{

	#ifndef MAX_TEXT
	#define MAX_TEXT    2048
	#endif

	#ifndef DUB_MAX_TEX
	#define DUB_MAX_TEX 4096
	#endif

	//---------------------------------------------------------
	// ����: CIME
	// ����: IME���뷨, һ�����벻���� MAX_TEXT ���ַ�
	//---------------------------------------------------------
	class AIRGuiExport CIME
	{
	public:
		//---------------------------------------------------------
		// ����:  CIME
		// ����:  ���캯��, ͨ��DisableIme��ʼ��һЩ����
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		CIME();

		//---------------------------------------------------------
		// ����:  ~CIME
		// ����:  ��������
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual ~CIME();


	public:
		//---------------------------------------------------------
		// ����:  Initialize
		// ����:  ��ʼ��IME���뷨
		// ����:  ��
		// ����:  ��ʼ���ɹ�
		//---------------------------------------------------------
		virtual bool Initialize( void );

		//---------------------------------------------------------
		// ����:  Release
		// ����:  �ͷ�IME���뷨��ʵ����
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void Release( void );



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
        /*
		virtual void SetIMEKey( 
			int  nKeyPrevIME    = KEY_F1,
			int  nKeyNextIME    = KEY_F2,
			int  nKeySharpIME   = KEY_F3,
			int  nKeySymbolIME  = KEY_F4,
			int  nKeyEnableIME  = KEY_F5,
			int  nKeyDisableIME = KEY_F6 );
            */

		//---------------------------------------------------------
		// ����:  DisableIme
		// ����:  �رղ���ֹ���뷨����ime�Ѿ�����رգ��˺����
		//        �������ȼ�����ime
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void DisableIme( void );

		//---------------------------------------------------------
		// ����:  EnableIme
		// ����:  �������뷨���˺���ҿ������ȼ�����ime
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void EnableIme( void );

		//---------------------------------------------------------
		// ����:  PrevIme
		// ����:  �л�����һ�����뷨������EnableIme�����Ч
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void PrevIme( void );

		//---------------------------------------------------------
		// ����:  NextIme
		// ����:  �л�����һ�����뷨������EnableIme�����Ч
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void NextIme( void );

		//---------------------------------------------------------
		// ����:  SharpIme
		// ����:  �л�ȫ��/���
		// ����:  ����HANDLE
		// ����:  ��
		//---------------------------------------------------------
		virtual void SharpIme( HWND hWnd );

		//---------------------------------------------------------
		// ����:  SymbolIme
		// ����:  �л���/Ӣ�ı��
		// ����:  ����HANDLE
		// ����:  ��
		//---------------------------------------------------------
		virtual void SymbolIme( HWND hWnd );




		//=========================================================
		//                        ״̬����
		//=========================================================

		//---------------------------------------------------------
		// ����:  GetImeName
		// ����:  �õ����뷨���֣������ǰ��Ӣ���򷵻�NULL
		// ����:  ��
		// ����:  �õ����뷨����
		//---------------------------------------------------------
		virtual char* GetImeName( void );

		//---------------------------------------------------------
		// ����:  IsImeSharp
		// ����:  �Ƿ�ȫ��
		// ����:  ��
		// ����:  �Ƿ�ȫ��
		//---------------------------------------------------------
		virtual bool IsImeSharp( void );

		//---------------------------------------------------------
		// ����:  IsImeSymbol
		// ����:  �Ƿ����ı��
		// ����:  ��
		// ����:  �Ƿ����ı��
		//---------------------------------------------------------
		virtual bool IsImeSymbol( void );

		//---------------------------------------------------------
		// ����:  GetImeInput
		// ����:  �õ����뷨״̬���ĸ�ָ�������ΪNULL���״̬���ط���
		// ����:  pszCompStr     - ����ת����Ĵ�
		//        pszCompReadStr - ���ؼ���ֱ������Ĵ�
		//        pnImeCursor    - ����szCompStr�Ĺ��λ��
		//        pszCandList    - ����ѡ�ֱ�ÿ��֮����\t�ָ�
		// ����:  ��
		//---------------------------------------------------------
		virtual void GetImeInput( 
			char **pszCompStr, 
			char **pszCompReadStr, 
			int  *pnImeCursor, 
			char **pszCandList );

		//---------------------------------------------------------
		// ����:  GetTextMessage
		// ����:  ��õ�ǰ�������ַ���
		// ����:  ��
		// ����:  ��ǰ�������ַ���ָ��
		//---------------------------------------------------------
		virtual char* GetTextMessage( void );

		//---------------------------------------------------------
		// ����:  GetTextMessageW
		// ����:  ��õ�ǰ�������ַ���
		// ����:  ��
		// ����:  ��ǰ�������ַ���ָ��
		//---------------------------------------------------------
		virtual wchar_t* CIME::GetTextMessageW( void );

		//---------------------------------------------------------
		// ����:  SetTextMessage
		// ����:  ���õ�ǰ�������ַ���
		// ����:  szText - �����ַ���
		// ����:  ��
		//---------------------------------------------------------
		virtual void SetTextMessage( char* szText );

		//---------------------------------------------------------
		// ����:  GetCursorPos
		// ����:  ȡ�ù���λ��
		// ����:  ��
		// ����:  ����λ��
		//---------------------------------------------------------
		virtual int GetCursorPos( void );

		//---------------------------------------------------------
		// ����:  GetLastInputMessage
		// ����:  �����һ�ε�������
		// ����:  ��
		// ����:  ��һ�ε�������
		//---------------------------------------------------------
		virtual const AIRString& GetLastInputMessage( void );

		//---------------------------------------------------------
		// ����:  GetLastInputMessageW
		// ����:  �����һ�ε�������
		// ����:  ��
		// ����:  ��һ�ε�������
		//---------------------------------------------------------
		//virtual AURO::stringw GetLastInputMessageW( void );

		//---------------------------------------------------------
		// ����:  GetIMEOpen
		// ����:  ��ȡ��ǰ�Ƿ񷢿�IME���뷨
		// ����:  ��
		// ����:  ��ǰ�Ƿ񷢿�IME���뷨
		//---------------------------------------------------------
		virtual bool GetIMEOpen( void );

		//---------------------------------------------------------
		// ����:  GetTextW
		// ����:  ���IME������ַ�����Ϣ
		// ����:  ��
		// ����:  ���ַ���ʽ���ַ���
		//---------------------------------------------------------
		//virtual AURO::stringw GetTextW( void );

		//---------------------------------------------------------
		// ����:  ClearBuffer
		// ����:  ���IME������ַ�������
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void ClearBuffer( void );



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
		virtual void OnChar(  HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		//---------------------------------------------------------
		// ����:  OnKeyDown
		// ����:  ����IME�����뷨�����¼�
		// ����:  hWnd - ���ھ��; message - ��Ϣ; 
		//        wParam - ����; lParam - ����
		// ����:  ��
		//---------------------------------------------------------
		virtual void OnKeyDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		//---------------------------------------------------------
		// ����:  OnKeyUp
		// ����:  �����������Ϣ����
		// ����:  hWnd - ���ھ��; message - ��Ϣ; 
		//        wParam - ����; lParam - ����
		// ����:  ��
		//---------------------------------------------------------
		virtual void OnKeyUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		//---------------------------------------------------------
		// ����:  OnWM_INPUTLANGCHANGEREQUEST
		// ����:  ��������Ϣ�е��õĺ��������������true��
		//        �򴰿ں���Ӧֱ�ӷ���0������Ӧ���ݸ�DefWindowProc
		// ����:  ��
		// ����:  ����ı����뷨
		//---------------------------------------------------------
		virtual bool OnWM_INPUTLANGCHANGEREQUEST( void );

		//---------------------------------------------------------
		// ����:  OnWM_INPUTLANGCHANGE
		// ����:  �ı����뷨
		// ����:  ����HANDLE
		// ����:  �޸ĳɹ�?
		//---------------------------------------------------------
		virtual bool OnWM_INPUTLANGCHANGE( HWND hWnd );

		//---------------------------------------------------------
		// ����:  OnWM_IME_SETCONTEXT
		// ����:  IMEѡ������
		// ����:  ��
		// ����:  ѡ��ɹ�
		//---------------------------------------------------------
		virtual bool OnWM_IME_SETCONTEXT( void );

		//---------------------------------------------------------
		// ����:  OnWM_IME_STARTCOMPOSITION
		// ����:  IME��ʼ�ϳ�����
		// ����:  ��
		// ����:  TRUE
		//---------------------------------------------------------
		virtual bool OnWM_IME_STARTCOMPOSITION( void );

		//---------------------------------------------------------
		// ����:  OnWM_IME_ENDCOMPOSITION
		// ����:  IME�����ϳ�����
		// ����:  ��
		// ����:  TRUE
		//---------------------------------------------------------
		virtual bool OnWM_IME_ENDCOMPOSITION( void );

		//---------------------------------------------------------
		// ����:  OnWM_IME_NOTIFY
		// ����:  IME�޸����뷨״̬
		// ����:  hWnd - ���ھ��;  wParam - WPARAM����
		// ����:  ���Ƿ���true����ֹime���ڴ�
		//---------------------------------------------------------
		virtual bool OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam );

		//---------------------------------------------------------
		// ����:  OnWM_IME_COMPOSITION
		// ����:  ������뷨����Ϣ�ַ���
		// ����:  hWnd - ���ھ��;  wParam - WPARAM����
		// ����:  ���Ƿ���true����ֹime���ڴ�
		//---------------------------------------------------------
		virtual bool OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam );



    
	private:

		//---------------------------------------------------------
		// ����:  ConvertCandList
		// ����:  ��ѡ�ֱ�����ɴ�
		// ����:  pCandList - ѡ���б�; pszCandList - ѡ�ֱ��ַ���
		// ����:  ��
		//---------------------------------------------------------
		virtual void ConvertCandList( CANDIDATELIST *pCandList, char *pszCandList );

        //����Ҫ����editbox��ʵ��
        /*
		//---------------------------------------------------------
		// ����:  MoveCursorLeft
		// ����:  �������, �����ƶ����
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void MoveCursorLeft( void );

		//---------------------------------------------------------
		// ����:  MoveCursorRight
		// ����:  �����Ҽ�, �����ƶ����
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void MoveCursorRight();

		//---------------------------------------------------------
		// ����:  ForwardDelete
		// ����:  ɾ�����ǰ���һ���ַ�
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void ForwardDelete( void );

		//---------------------------------------------------------
		// ����:  BackwardDelete
		// ����:  ɾ���������һ���ַ�
		// ����:  ��
		// ����:  ��
		//---------------------------------------------------------
		virtual void BackwardDelete( void );
    */

	private:
		bool           m_bIme;                        // ime�����־
		char           m_szCompStr[DUB_MAX_TEX];      // �洢ת����Ĵ�
		char           m_szCompReadStr[DUB_MAX_TEX];  // �洢����Ĵ�
		char           m_szCandList[DUB_MAX_TEX];     // �洢������ַ���ѡ�ֱ�
		int            m_nImeCursor;                  // �洢ת����Ĵ��еĹ��λ��
		CANDIDATELIST* m_lpCandList;                  // �洢��׼��ѡ�ֱ�
		char           m_szImeName[ 256 ];            // �洢���뷨������
		bool           m_bImeSharp;                   // ȫ�Ǳ�־
		bool           m_bImeSymbol;                  // ���ı���־
		int            m_nKeyPrevIME;                 // ��һ�����뷨�İ���
		int            m_nKeyNextIME;                 // ��һ�����뷨�İ���
		int            m_nKeySharpIME;                // ���/ȫ���л�����
		int            m_nKeySymbolIME;               // ��Ӣ�ı���л�����
		int            m_nKeyEnableIME;               // ����IME���뷨����
		int            m_nKeyDisableIME;              // �ر����뷨����
		char           m_szText[DUB_MAX_TEX];         // �����������������
		//char*          m_pszText;                     // �����޸� m_szText ��ָ��
		wchar_t        m_wcText[MAX_TEXT];            // �����������������(���ֽڴ�)
		wchar_t*       m_pwcText;                     // �����޸� m_wcText ��ָ��
		int            m_nIndex;                      // ���λ��
		AIRString    m_strResult;                   // ������һ�ε�������
		//AURO::stringw  m_wszResult;                   // ������һ�ε�������

	};//! end class CIME



//};//! end namespace AURO


#endif


#endif //! end __CIME_INCLUDE__


// ����΢��ƴ��ʵ���˺ܶ��Լ��Ķ�����CIme�����ļ�������Щ����
// 1���ں���ConvertCandList����˵��ѡ�ֱ������
// 2������GetImeInput���ص�szCompReadStr��Ȼ�����˼ӹ�����������ļ�������
//    ����ÿ������ϵ�����ռ�Կո����8byte�����µĲ�����ϵ������Ϊ0xa1
//    ���ǿ��������뷨���������Ӵ�"΢��ƴ��"ʱ��ֻ��ʾĩβ��һ��8byte�����
//    ��0xa1��ʲô������ʾ��Ҳ����ֱ����TextOut��ʾ���е�

// [1] 
// ������е�����ϵͳ
// AURO::InputSystem::GetInstancePtr()->AcquireAll();

// [2]
// ��Ϣ���� WndProc()
// case WM_KEYDOWN:			      // 0x0100
//		AURO::CIME::GetInstancePtr()->OnKeyDown( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_KEYUP:				      // 0x0101
//		AURO::CIME::GetInstancePtr()->OnKeyUp( hWnd, uMsg, wParam, lParam );
//		break;	
// case WM_CHAR:                     // 0x0102
//		AURO::CIME::GetInstancePtr()->OnChar( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_IME_SETCONTEXT:         // 0x0281
//		return AURO::CIME::GetInstancePtr()->OnWM_IME_SETCONTEXT()                ? 0 : DefWindowProc( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_INPUTLANGCHANGEREQUEST: // 0x0050
//		return AURO::CIME::GetInstancePtr()->OnWM_INPUTLANGCHANGEREQUEST()        ? 0 : DefWindowProc( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_INPUTLANGCHANGE:        // 0x0051
//		return AURO::CIME::GetInstancePtr()->OnWM_INPUTLANGCHANGE( hWnd )         ? 0 : DefWindowProc( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_IME_STARTCOMPOSITION:   // 0x010d
//		return AURO::CIME::GetInstancePtr()->OnWM_IME_STARTCOMPOSITION()          ? 0 : DefWindowProc( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_IME_ENDCOMPOSITION:     // 0x010e
//		return AURO::CIME::GetInstancePtr()->OnWM_IME_ENDCOMPOSITION()            ? 0 : DefWindowProc( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_IME_NOTIFY:             // 0x0282
//		return AURO::CIME::GetInstancePtr()->OnWM_IME_NOTIFY( hWnd, wParam )      ? 0 : DefWindowProc( hWnd, uMsg, wParam, lParam );
//		break;
// case WM_IME_COMPOSITION:        // 0x010f
//		return AURO::CIME::GetInstancePtr()->OnWM_IME_COMPOSITION( hWnd, lParam ) ? 0 : DefWindowProc( hWnd, uMsg, wParam, lParam );
//		break;

// [3]
// ȡ��״̬����,�����ַ���
// if( AURO::CIME::GetInstancePtr()->GetImeName() )
// {
//		char str[1000];
//		snprintf( str, 1000, "%s", AURO::CIME::GetInstancePtr()->GetImeName() );
//		AIRString imeInfo = AIRString( str ) + " ";
//		imeInfo += AURO::CIME::GetInstancePtr()->IsImeSharp() ? "ȫ�� " : "��� ";
//		imeInfo += AURO::CIME::GetInstancePtr()->IsImeSymbol() ? "���ı�� " : "Ӣ�ı�� ";
//		m_FMTImeInfo.SetText( imeInfo, 12 );
//		m_FMTImeInfo.Render();
//
//		char *szCompStr, *szCompReadStr, *szCandList;
//		AURO::CIME::GetInstancePtr()->GetImeInput( &szCompStr, &szCompReadStr, NULL, &szCandList );
//		m_FMTCompStr.SetText( szCompStr, 12 );
//		m_FMTCompStr.Render();
//
//		m_FMTCompReadStr.SetText( szCompReadStr, 12 );
//		m_FMTCompReadStr.Render();
//
//		m_FMTCandList.SetText( szCandList, 12 );
//		m_FMTCandList.Render();
// }
//
// m_FMTMsg.SetText( AURO::CIME::GetInstancePtr()->GetTextMessage(), 12 );
// m_FMTMsg.Render();
// m_FMTMsg.DrawCursor();

// [4]
// �ͷ�IME���뷨ϵͳ
// AURO::CIME::GetInstancePtr()->Release();