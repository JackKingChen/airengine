// AIRGUI.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"
#include "AIRGUI.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32


#ifdef _MANAGED
#pragma managed(push, off)
#endif

#include <Windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#endif