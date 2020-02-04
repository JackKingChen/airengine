/*! \file Common.h
\brief Copyright 2007 umge. All Rights Reserved.\n
作    者: liangairan(liangairan@umge.com)\n
最后修改时间: 2008-05-14\n
功    能: 测试工具，代表一个登录的玩家\n
*/

#pragma once

#include "MemoryAllocatorConfig.h"
#include <vector>
//#include <hash_map>
#include <list>
#include <string>
#include <fstream>
#include <map>
#include <istream>
//#include <Windows.h>
#include <math.h>

#include "AIRTypes.h"
#include "Util.h"

//#define	HASH_MAP			stdext::hash_map
#define HASH_MAP				HashMap

using namespace std;
using namespace AIR;


//无效的句柄
#define INVALID_HANDLE			-1


//无效的ID值
#define INVALID_ID				-1

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
//#define snprintf _snprintf
#else
typedef struct tagPOINT
{
	u32  x;
	u32  y;
} POINT;
#endif