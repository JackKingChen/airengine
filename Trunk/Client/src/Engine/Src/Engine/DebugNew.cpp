#include "stdafx.h"
#include "DebugNew.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32



TXT_MEMINFO				g_AllocInfo;
VOID_POINTERINFO		g_PointerInfo;



//将堆使用内存信息输出至文件
void OutHeapMemoryInfoToFile(const char* szFileName)
{
	char szDate[256] = {0};
	char szTime[256] = {0};
	char szFile[256] = {0};


	SYSTEMTIME systime;
	GetLocalTime( &systime );

	_snprintf(szDate, sizeof(szDate)-1, "%04d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
	_snprintf(szTime, sizeof(szTime)-1, "%02d:%02d:%02d", systime.wHour, systime.wMinute, systime.wSecond);

	_mkdir("Log");
	chdir("Log");
	_mkdir(szDate);
	chdir(szDate);

	std::ofstream fLog(szFileName, std::ios::app);
	if (!fLog.is_open())
	{
		return;
	}

	fLog << szDate << " " << szTime << std::endl;
	fLog << "---------------heapmemoryinfostart---------------" << std::endl;

	unsigned int nAllMemory = 0;
	for (map<const char*, ALLOCMEMORYINFO>::iterator itr = g_AllocInfo.begin();
		itr != g_AllocInfo.end(); itr++)
	{
		nAllMemory += itr->second.nAllUseMemory;
		fLog << (itr->first!=NULL?itr->first:"未定义用途") << " 占用内存[" << itr->second.nAllUseMemory << "]字节" << "  目前未释放共分配[" << itr->second.nExistCount << "]次  历史共申请[" << itr->second.nAllocCount << "]次" << std::endl;
	}

	fLog << "全部申请内存 " << nAllMemory << " Byte 共计 " << nAllMemory/(1024*1024) << " MByte" << std::endl;
	fLog << "---------------heapmemoryinfoend---------------" << std::endl;

	fLog.close();
	chdir("../../");
};

#endif