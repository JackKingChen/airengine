// ***************************************************************
//  文件: DebugNew.h
//  日期: 2010-7-21
//  作者: huangjunjie
//  说明: 对所有new出来的内存进行监视
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


//每个用途的内存使用信息
typedef struct AllocMemoryInfo
{
	unsigned int nExistCount;       //当前未释放的申请次数
	unsigned int nAllocCount;       //此用途的内存共申请了多少次
	unsigned int nAllUseMemory;     //此用途的内存所占用的内存大小
}ALLOCMEMORYINFO;

typedef struct PointerInfo
{
	unsigned int nSize;
	unsigned int nAllocCount;       //此指针申请的相关类型的个数（用于判断删除时是用 delete p 还是 delete [] p）
	const char* szNote;             //此指针的相关用途
}POINTERINFO;



typedef map<const char*, ALLOCMEMORYINFO>	TXT_MEMINFO;
typedef	map<void*, POINTERINFO>				VOID_POINTERINFO;



extern TXT_MEMINFO							g_AllocInfo;
extern VOID_POINTERINFO						g_PointerInfo;



//使用者必须使用CNewWithDebug<typename>::Alloc
template <typename T>
class CNewWithDebug
{
public:
	CNewWithDebug();
	virtual ~CNewWithDebug();

	static T* Alloc (unsigned int nCount = 1, const char* szNote = NULL)
	{
		//标准c++中如果new数组，即使数组元素为0，也会申请为1
		//new int[0] 等同于 new int[1];
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
	//如果定义成多线程,则启用
	static CRITICAL_SECTION m_cs;
#endif
};


template <typename T>
CRITICAL_SECTION CNewWithDebug<T>::m_cs;


//将堆使用内存信息输出至文件
void OutHeapMemoryInfoToFile(const char* szFileName);

#endif

#endif //! __DebugNew_H__