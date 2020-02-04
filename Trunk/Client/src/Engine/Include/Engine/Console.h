// ***************************************************************
//  �ļ�: Console.h
//  ����: 2009-12-28
//  ����: huangjunjie
//  ˵��: ����̨
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

    // ��ʾ����̨
    void ShowConsole(bool b);

    // ��С������̨
    void MinimumConsole( void );

    // �ָ�����̨
    void RestoreConsole( void );

    // �ͷſ���̨
    void FreeConsole( void );

    // ���ÿ���̨��λ��
    void SetConsolePos( int x, int y );

    // ���������̨
    void Print( const char* szMsg );

    // ������������
    void ProcInputCmd( void );

    // ���������ID
    const AIRString& GetCmdID( void ) const
    {
        return m_szCmdID;
    }

    // �������������
    const AIRString& GetCmdContent( void ) const
    {
        return m_szCmdContent;
    }

    // ÿ֡����
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

