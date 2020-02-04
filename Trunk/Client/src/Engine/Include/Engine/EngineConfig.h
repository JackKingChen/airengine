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
	// ������뽫λ������ָ��ƽ̨֮ǰ��ƽ̨��ΪĿ�꣬���޸����ж��塣
	// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
	#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
	#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
	#endif

	#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
	#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
	#endif

	#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
	#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ����ָ���� Windows Me ����߰汾��ΪĿ�ꡣ
	#endif

	#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
	#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
	#endif

	//#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
	// Windows ͷ�ļ�:

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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
//#define MEM_LEAK_CHECK
#ifdef _DEBUG
//#ifdef MEM_LEAK_CHECK
//#include <vld.h>  //�ڴ�й©���
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
