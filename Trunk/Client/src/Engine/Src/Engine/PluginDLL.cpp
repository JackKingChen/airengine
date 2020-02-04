#include "stdafx.h"
#include "PluginDLL.h"
#include <assert.h>

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#define AIR_LIBRARY_HANDLE	HMODULE
	#define AIR_LOAD_LIBRARY(a)	LoadLibrary(a)
	#define AIR_CLOSE_LIBRARY	FreeLibrary
	#define AIR_GET_PROC_ADDR	GetProcAddress
	#define AIR_LIBRARY_EXT		".dll"
    #define AIR_FUNC_HANDLE     FARPROC
#else
#include <dlfcn.h>
	#define AIR_LIBRARY_HANDLE	void*
	#define AIR_LOAD_LIBRARY(a)	dlopen(a,RTLD_NOW)
	#define AIR_CLOSE_LIBRARY	dlclose
	#define AIR_GET_PROC_ADDR	dlsym
	#define AIR_LIBRARY_EXT		".so"
    #define AIR_FUNC_HANDLE     void*
#endif

typedef void (*DLL_START_PLUGIN)(void);

PluginDLL::PluginDLL(const char* szName) : m_hInst(0)
{
    m_hInst = AIR_LOAD_LIBRARY(szName);

	assert( m_hInst );
    if (m_hInst)
    {
        DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)AIR_GET_PROC_ADDR(m_hInst, "PluginStartFunc");

        if (pFunc)
        {
            pFunc();
        }
    }
    
}

PluginDLL::~PluginDLL()
{
	assert( m_hInst );
    if (m_hInst)
    {
        DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)AIR_GET_PROC_ADDR(m_hInst, "PluginEndFunc");

        if (pFunc)
        {
            pFunc();
        }
        AIR_CLOSE_LIBRARY(m_hInst);
    }
}
