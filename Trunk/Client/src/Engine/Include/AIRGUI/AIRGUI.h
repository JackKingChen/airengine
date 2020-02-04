#pragma once 

#include "EngineConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
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