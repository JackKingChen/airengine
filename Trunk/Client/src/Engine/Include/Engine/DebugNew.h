// ***************************************************************
//  �ļ�: DebugNew.h
//  ����: 2010-7-21
//  ����: huangjunjie
//  ˵��: ������new�������ڴ���м���
// ***************************************************************

#pragma once

#ifndef __DebugNew_H__
#define __DebugNew_H__

#include "EngineConfig.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32

#include <map>
#include <fstream>
#include <direct.h>
#include <time.h>
#include <windows.h>

using std::map;
using std::pair;


//ÿ����;���ڴ�ʹ����Ϣ
typedef struct AllocMemoryInfo
{
	unsigned int nExistCount;       //��ǰδ�ͷŵ��������
	unsigned int nAllocCount;       //����;���ڴ湲�����˶��ٴ�
	unsigned int nAllUseMemory;     //����;���ڴ���ռ�õ��ڴ��С
}ALLOCMEMORYINFO;

typedef struct PointerInfo
{
	unsigned int nSize;
	unsigned int nAllocCount;       //��ָ�������������͵ĸ����������ж�ɾ��ʱ���� delete p ���� delete [] p��
	const char* szNote;             //��ָ��������;
}POINTERINFO;



typedef map<const char*, ALLOCMEMORYINFO>	TXT_MEMINFO;
typedef	map<void*, POINTERINFO>				VOID_POINTERINFO;



extern TXT_MEMINFO							g_AllocInfo;
extern VOID_POINTERINFO						g_PointerInfo;



//ʹ���߱���ʹ��CNewWithDebug<typename>::Alloc
template <typename T>
class CNewWithDebug
{
public:
	CNewWithDebug();
	virtual ~CNewWithDebug();

	static T* Alloc (unsigned int nCount = 1, const char* szNote = NULL)
	{
		//��׼c++�����new���飬��ʹ����Ԫ��Ϊ0��Ҳ������Ϊ1
		//new int[0] ��ͬ�� new int[1];
		if (nCount == 0)
		{
			nCount = 1;
		}

		T* p = NULL;
		if (nCount > 1)
		{
			p = new T[nCount];
		}
		else
		{
			p = new T;
		}

		POINTERINFO pinfo = {0};
		pinfo.nAllocCount = nCount;
		pinfo.nSize = sizeof(T)*nCount;
		pinfo.szNote = szNote;
		g_PointerInfo[p] = pinfo;

		map<const char*, ALLOCMEMORYINFO>::iterator itr = g_AllocInfo.find(szNote);

		if (itr == g_AllocInfo.end())
		{
			ALLOCMEMORYINFO ainfo = {0};
			ainfo.nAllocCount = 1;
			ainfo.nExistCount = 1;
			ainfo.nAllUseMemory += pinfo.nSize;
			g_AllocInfo.insert(pair<const char*, ALLOCMEMORYINFO>(szNote, ainfo));
		}
		else
		{
			itr->second.nAllocCount ++;
			itr->second.nExistCount ++;
			itr->second.nAllUseMemory += pinfo.nSize;
		}

		return p;
	}

	static void Free(T* p)
	{
		if (p == NULL)
		{
			return;
		}
		map<void*, POINTERINFO>::iterator itr = g_PointerInfo.find(p);
		if (itr != g_PointerInfo.end())
		{
			map<const char*, ALLOCMEMORYINFO>::iterator allinfoitr = g_AllocInfo.find(itr->second.szNote);
			if (allinfoitr != g_AllocInfo.end())
			{
				allinfoitr->second.nExistCount --;
				allinfoitr->second.nAllUseMemory -= itr->second.nSize;
			}

			if (itr->second.nAllocCount >1)
			{
				delete [] p;
			}
			else
			{
				delete p;
			}
		}
	}

protected:

#ifdef _MT
	//�������ɶ��߳�,������
	static CRITICAL_SECTION m_cs;
#endif
};


template <typename T>
CRITICAL_SECTION CNewWithDebug<T>::m_cs;


//����ʹ���ڴ���Ϣ������ļ�
void OutHeapMemoryInfoToFile(const char* szFileName);

#endif

#endif //! __DebugNew_H__