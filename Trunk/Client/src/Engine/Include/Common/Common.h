/*! \file Common.h
\brief Copyright 2007 umge. All Rights Reserved.\n
��    ��: liangairan(liangairan@umge.com)\n
����޸�ʱ��: 2008-05-14\n
��    ��: ���Թ��ߣ�����һ����¼�����\n
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


//��Ч�ľ��
#define INVALID_HANDLE			-1


//��Ч��IDֵ
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