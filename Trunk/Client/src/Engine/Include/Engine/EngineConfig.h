/********************************************************************
	created:	2009/10/20
	created:	20:10:2009   17:01
	filename: 	d:\My Codes\3DGraphic\Include\EngineConfig.h
	file path:	d:\My Codes\3DGraphic\Include
	file base:	EngineConfig
	file ext:	h
	author:		liangairan
	
	purpose:	
*********************************************************************/

#pragma once 

#include "PlatformConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	// 如果必须将位于下面指定平台之前的平台作为目标，请修改下列定义。
	// 有关不同平台对应值的最新信息，请参考 MSDN。
	#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
	#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
	#endif

	#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
	#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
	#endif

	#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
	#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以指定将 Windows Me 或更高版本作为目标。
	#endif

	#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
	#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。
	#endif

	//#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
	// Windows 头文件:

	#include <windows.h>

#endif


//#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//	#ifdef	GRAPHIC_EXPORTS
//		#define GRAPHIC_API __declspec(dllexport)
//	#else
//		#define GRAPHIC_API __declspec(dllimport)
//	#endif
//#else
//	#define GRAPHIC_API
//#endif
//
//#define EngineExport GRAPHIC_API
#define EngineExport AIR_API

// TODO: 在此处引用程序需要的其他头文件
//#define MEM_LEAK_CHECK
#ifdef _DEBUG
//#ifdef MEM_LEAK_CHECK
//#include <vld.h>  //内存泄漏检测
//#endif
#endif

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	#pragma warning(disable: 4251)
	#pragma warning(disable: 4244)
#else
	typedef struct tagRECT
	{
		int    left;
		int    top;
		int    right;
		int    bottom;
	} RECT;
#endif
