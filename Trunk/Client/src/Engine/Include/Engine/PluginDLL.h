#pragma once 

#include "MemoryAllocatorConfig.h"

class EngineExport PluginDLL //: public EngineAllocator(PluginDLL)
{
	EngineAllocator(PluginDLL)
public:
    PluginDLL(const char* szName);

    virtual ~PluginDLL();
protected:
private:
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    HMODULE m_hInst;
#else
	void*	m_hInst;
#endif
};
