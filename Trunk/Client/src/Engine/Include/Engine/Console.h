// ***************************************************************
//  文件: Console.h
//  日期: 2009-12-28
//  作者: huangjunjie
//  说明: 控制台
// ***************************************************************


#pragma once

#include "EngineConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32


#include "EngineConfig.h"
#include <Windows.h>
#include <string>
#include "MemoryAllocatorConfig.h"

enum EPRINT_TYPE
{
    EPT_OUT,
    EPT_IN,
    EPT_COM
};

class EngineExport Console
{
public:
    
    ~Console();

    static Console* GetInstance();

    // 显示控制台
    void ShowConsole(bool b);

    // 最小化控制台
    void MinimumConsole( void );

    // 恢复控制台
    void RestoreConsole( void );

    // 释放控制台
    void FreeConsole( void );

    // 设置控制台的位置
    void SetConsolePos( int x, int y );

    // 输出到控制台
    void Print( const char* szMsg );

    // 处理命令输入
    void ProcInputCmd( void );

    // 获得命令行ID
    const AIRString& GetCmdID( void ) const
    {
        return m_szCmdID;
    }

    // 获得命令行内容
    const AIRString& GetCmdContent( void ) const
    {
        return m_szCmdContent;
    }

    // 每帧更新
    void Update( void );

    void SetHasExecuteCmd( bool b );

    bool IsVisible() const
    {
        return m_bIsShowConsole;
    }
private:
    Console();

    void Init();

private:
    HWND        m_hWnd;
    HANDLE      m_hInput; 
    HANDLE      m_hOutput;
    HANDLE      m_hEoutput;
    AIRString m_szCmdID;
    AIRString m_szCmdContent;
    bool        m_bHasExecuteCmd;
    bool        m_bIsShowConsole;
    bool        m_bInited;

};//! end class Console

#define CONSOLE Console::GetInstance()


#endif 

