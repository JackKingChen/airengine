#pragma once 

#include "EngineConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif		



#ifdef AIRGUI_EXPORTS
#define AIRGuiExport __declspec(dllexport)
#else
#define AIRGuiExport __declspec(dllimport)
#endif

#else

#ifdef  AIRGUI_EXPORTS
#define AIRGuiExport  __attribute__((visibility("default")))
#else
#define AIRGuiExport
#endif 


#endif