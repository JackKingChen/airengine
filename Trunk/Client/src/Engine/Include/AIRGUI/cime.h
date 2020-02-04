//****************************************************************************//
//*	作者	:  HJJ                                                           
//*	日期    :  2008-11-28							                 
//* 标题    :  CIME.h                                                 
//* 功能    :  IME输入法                                                        
//* 说明    :                                                           
//*            Copyright (C) 2008 - All Rights Reserved                    
//****************************************************************************//


#ifndef  __CIME_INCLUDE__
#define  __CIME_INCLUDE__

#pragma once
//-------------------------------------------------------------
// 添加头文件
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
	// 名称: CIME
	// 功能: IME输入法, 一次输入不超过 MAX_TEXT 个字符
	//---------------------------------------------------------
	class AIRGuiExport CIME
	{
	public:
		//---------------------------------------------------------
		// 名称:  CIME
		// 功能:  构造函数, 通过DisableIme初始化一些数据
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		CIME();

		//---------------------------------------------------------
		// 名称:  ~CIME
		// 功能:  析构函数
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual ~CIME();


	public:
		//---------------------------------------------------------
		// 名称:  Initialize
		// 功能:  初始化IME输入法
		// 参数:  无
		// 返回:  初始化成功
		//---------------------------------------------------------
		virtual bool Initialize( void );

		//---------------------------------------------------------
		// 名称:  Release
		// 功能:  释放IME输入法单实体类
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void Release( void );



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
		// 名称:  DisableIme
		// 功能:  关闭并禁止输入法，如ime已经打开则关闭，此后玩家
		//        不能用热键呼出ime
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void DisableIme( void );

		//---------------------------------------------------------
		// 名称:  EnableIme
		// 功能:  允许输入法，此后玩家可以用热键呼出ime
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void EnableIme( void );

		//---------------------------------------------------------
		// 名称:  PrevIme
		// 功能:  切换到上一种输入法，必须EnableIme后才有效
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void PrevIme( void );

		//---------------------------------------------------------
		// 名称:  NextIme
		// 功能:  切换到下一种输入法，必须EnableIme后才有效
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void NextIme( void );

		//---------------------------------------------------------
		// 名称:  SharpIme
		// 功能:  切换全角/半角
		// 参数:  窗口HANDLE
		// 返回:  无
		//---------------------------------------------------------
		virtual void SharpIme( HWND hWnd );

		//---------------------------------------------------------
		// 名称:  SymbolIme
		// 功能:  切换中/英文标点
		// 参数:  窗口HANDLE
		// 返回:  无
		//---------------------------------------------------------
		virtual void SymbolIme( HWND hWnd );




		//=========================================================
		//                        状态函数
		//=========================================================

		//---------------------------------------------------------
		// 名称:  GetImeName
		// 功能:  得到输入法名字，如果当前是英文则返回NULL
		// 参数:  无
		// 返回:  得到输入法名字
		//---------------------------------------------------------
		virtual char* GetImeName( void );

		//---------------------------------------------------------
		// 名称:  IsImeSharp
		// 功能:  是否全角
		// 参数:  无
		// 返回:  是否全角
		//---------------------------------------------------------
		virtual bool IsImeSharp( void );

		//---------------------------------------------------------
		// 名称:  IsImeSymbol
		// 功能:  是否中文标点
		// 参数:  无
		// 返回:  是否中文标点
		//---------------------------------------------------------
		virtual bool IsImeSymbol( void );

		//---------------------------------------------------------
		// 名称:  GetImeInput
		// 功能:  得到输入法状态，四个指针任意可为NULL则此状态不回返回
		// 参数:  pszCompStr     - 返回转换后的串
		//        pszCompReadStr - 返回键盘直接输入的串
		//        pnImeCursor    - 返回szCompStr的光标位置
		//        pszCandList    - 返回选字表，每项之间以\t分隔
		// 返回:  无
		//---------------------------------------------------------
		virtual void GetImeInput( 
			char **pszCompStr, 
			char **pszCompReadStr, 
			int  *pnImeCursor, 
			char **pszCandList );

		//---------------------------------------------------------
		// 名称:  GetTextMessage
		// 功能:  获得当前的输入字符串
		// 参数:  无
		// 返回:  当前的输入字符串指针
		//---------------------------------------------------------
		virtual char* GetTextMessage( void );

		//---------------------------------------------------------
		// 名称:  GetTextMessageW
		// 功能:  获得当前的输入字符串
		// 参数:  无
		// 返回:  当前的输入字符串指针
		//---------------------------------------------------------
		virtual wchar_t* CIME::GetTextMessageW( void );

		//---------------------------------------------------------
		// 名称:  SetTextMessage
		// 功能:  设置当前的输入字符串
		// 参数:  szText - 设置字符串
		// 返回:  无
		//---------------------------------------------------------
		virtual void SetTextMessage( char* szText );

		//---------------------------------------------------------
		// 名称:  GetCursorPos
		// 功能:  取得光标的位置
		// 参数:  无
		// 返回:  光标的位置
		//---------------------------------------------------------
		virtual int GetCursorPos( void );

		//---------------------------------------------------------
		// 名称:  GetLastInputMessage
		// 功能:  获得上一次的输入结果
		// 参数:  无
		// 返回:  上一次的输入结果
		//---------------------------------------------------------
		virtual const AIRString& GetLastInputMessage( void );

		//---------------------------------------------------------
		// 名称:  GetLastInputMessageW
		// 功能:  获得上一次的输入结果
		// 参数:  无
		// 返回:  上一次的输入结果
		//---------------------------------------------------------
		//virtual AURO::stringw GetLastInputMessageW( void );

		//---------------------------------------------------------
		// 名称:  GetIMEOpen
		// 功能:  获取当前是否发开IME输入法
		// 参数:  无
		// 返回:  当前是否发开IME输入法
		//---------------------------------------------------------
		virtual bool GetIMEOpen( void );

		//---------------------------------------------------------
		// 名称:  GetTextW
		// 功能:  获得IME输入的字符串信息
		// 参数:  无
		// 返回:  宽字符格式的字符串
		//---------------------------------------------------------
		//virtual AURO::stringw GetTextW( void );

		//---------------------------------------------------------
		// 名称:  ClearBuffer
		// 功能:  清除IME输入的字符缓冲区
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void ClearBuffer( void );



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
		virtual void OnChar(  HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		//---------------------------------------------------------
		// 名称:  OnKeyDown
		// 功能:  处理IME的输入法按键事件
		// 参数:  hWnd - 窗口句柄; message - 消息; 
		//        wParam - 参数; lParam - 参数
		// 返回:  无
		//---------------------------------------------------------
		virtual void OnKeyDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		//---------------------------------------------------------
		// 名称:  OnKeyUp
		// 功能:  按键弹起的消息处理
		// 参数:  hWnd - 窗口句柄; message - 消息; 
		//        wParam - 参数; lParam - 参数
		// 返回:  无
		//---------------------------------------------------------
		virtual void OnKeyUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		//---------------------------------------------------------
		// 名称:  OnWM_INPUTLANGCHANGEREQUEST
		// 功能:  必须在消息中调用的函数，如果返回是true，
		//        则窗口函数应直接返回0，否则应传递给DefWindowProc
		// 参数:  无
		// 返回:  请求改变输入法
		//---------------------------------------------------------
		virtual bool OnWM_INPUTLANGCHANGEREQUEST( void );

		//---------------------------------------------------------
		// 名称:  OnWM_INPUTLANGCHANGE
		// 功能:  改变输入法
		// 参数:  窗口HANDLE
		// 返回:  修改成功?
		//---------------------------------------------------------
		virtual bool OnWM_INPUTLANGCHANGE( HWND hWnd );

		//---------------------------------------------------------
		// 名称:  OnWM_IME_SETCONTEXT
		// 功能:  IME选择内容
		// 参数:  无
		// 返回:  选择成功
		//---------------------------------------------------------
		virtual bool OnWM_IME_SETCONTEXT( void );

		//---------------------------------------------------------
		// 名称:  OnWM_IME_STARTCOMPOSITION
		// 功能:  IME开始合成文字
		// 参数:  无
		// 返回:  TRUE
		//---------------------------------------------------------
		virtual bool OnWM_IME_STARTCOMPOSITION( void );

		//---------------------------------------------------------
		// 名称:  OnWM_IME_ENDCOMPOSITION
		// 功能:  IME结束合成文字
		// 参数:  无
		// 返回:  TRUE
		//---------------------------------------------------------
		virtual bool OnWM_IME_ENDCOMPOSITION( void );

		//---------------------------------------------------------
		// 名称:  OnWM_IME_NOTIFY
		// 功能:  IME修改输入法状态
		// 参数:  hWnd - 窗口句柄;  wParam - WPARAM参数
		// 返回:  总是返回true，防止ime窗口打开
		//---------------------------------------------------------
		virtual bool OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam );

		//---------------------------------------------------------
		// 名称:  OnWM_IME_COMPOSITION
		// 功能:  获得输入法的信息字符串
		// 参数:  hWnd - 窗口句柄;  wParam - WPARAM参数
		// 返回:  总是返回true，防止ime窗口打开
		//---------------------------------------------------------
		virtual bool OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam );



    
	private:

		//---------------------------------------------------------
		// 名称:  ConvertCandList
		// 功能:  将选字表整理成串
		// 参数:  pCandList - 选字列表; pszCandList - 选字表字符串
		// 返回:  无
		//---------------------------------------------------------
		virtual void ConvertCandList( CANDIDATELIST *pCandList, char *pszCandList );

        //不需要，在editbox中实现
        /*
		//---------------------------------------------------------
		// 名称:  MoveCursorLeft
		// 功能:  按下左键, 向左移动光标
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void MoveCursorLeft( void );

		//---------------------------------------------------------
		// 名称:  MoveCursorRight
		// 功能:  按下右键, 向右移动光标
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void MoveCursorRight();

		//---------------------------------------------------------
		// 名称:  ForwardDelete
		// 功能:  删除光标前面的一个字符
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void ForwardDelete( void );

		//---------------------------------------------------------
		// 名称:  BackwardDelete
		// 功能:  删除光标后面的一个字符
		// 参数:  无
		// 返回:  无
		//---------------------------------------------------------
		virtual void BackwardDelete( void );
    */

	private:
		bool           m_bIme;                        // ime允许标志
		char           m_szCompStr[DUB_MAX_TEX];      // 存储转换后的串
		char           m_szCompReadStr[DUB_MAX_TEX];  // 存储输入的串
		char           m_szCandList[DUB_MAX_TEX];     // 存储整理成字符串选字表
		int            m_nImeCursor;                  // 存储转换后的串中的光标位置
		CANDIDATELIST* m_lpCandList;                  // 存储标准的选字表
		char           m_szImeName[ 256 ];            // 存储输入法的名字
		bool           m_bImeSharp;                   // 全角标志
		bool           m_bImeSymbol;                  // 中文标点标志
		int            m_nKeyPrevIME;                 // 上一种输入法的按键
		int            m_nKeyNextIME;                 // 下一个输入法的按键
		int            m_nKeySharpIME;                // 半角/全角切换按键
		int            m_nKeySymbolIME;               // 中英文标点切换按键
		int            m_nKeyEnableIME;               // 开启IME输入法按键
		int            m_nKeyDisableIME;              // 关闭输入法按键
		char           m_szText[DUB_MAX_TEX];         // 保存所有输入的内容
		//char*          m_pszText;                     // 用于修改 m_szText 的指针
		wchar_t        m_wcText[MAX_TEXT];            // 保存所有输入的内容(宽字节串)
		wchar_t*       m_pwcText;                     // 用于修改 m_wcText 的指针
		int            m_nIndex;                      // 光标位置
		AIRString    m_strResult;                   // 保存上一次的输入结果
		//AURO::stringw  m_wszResult;                   // 保存上一次的输入结果

	};//! end class CIME



//};//! end namespace AURO


#endif


#endif //! end __CIME_INCLUDE__


// 由于微软拼音实现了很多自己的东西，CIme和它的兼容性有些问题
// 1、在函数ConvertCandList中所说的选字表的问题
// 2、函数GetImeInput返回的szCompReadStr显然经过了加工而不是最初的键盘输入
//    它的每个可组合的输入占以空格补足的8byte，且新的不可组合的输入存为0xa1
//    我们可以在输入法名字中有子串"微软拼音"时，只显示末尾的一组8byte，如果
//    有0xa1就什么都不显示，也可以直接用TextOut显示所有的

// [1] 
// 获得所有的输入系统
// AURO::InputSystem::GetInstancePtr()->AcquireAll();

// [2]
// 消息处理 WndProc()
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
// 取得状态参数,输入字符串
// if( AURO::CIME::GetInstancePtr()->GetImeName() )
// {
//		char str[1000];
//		snprintf( str, 1000, "%s", AURO::CIME::GetInstancePtr()->GetImeName() );
//		AIRString imeInfo = AIRString( str ) + " ";
//		imeInfo += AURO::CIME::GetInstancePtr()->IsImeSharp() ? "全角 " : "半角 ";
//		imeInfo += AURO::CIME::GetInstancePtr()->IsImeSymbol() ? "中文标点 " : "英文标点 ";
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
// 释放IME输入法系统
// AURO::CIME::GetInstancePtr()->Release();