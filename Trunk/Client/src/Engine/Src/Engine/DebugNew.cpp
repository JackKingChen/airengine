#include "stdafx.h"
#include "DebugNew.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32



TXT_MEMINFO				g_AllocInfo;
VOID_POINTERINFO		g_PointerInfo;



//����ʹ���ڴ���Ϣ������ļ�
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
		fLog << (itr->first!=NULL?itr->first:"δ������;") << " ռ���ڴ�[" << itr->second.nAllUseMemory << "]�ֽ�" << "  Ŀǰδ�ͷŹ�����[" << itr->second.nExistCount << "]��  ��ʷ������[" << itr->second.nAllocCount << "]��" << std::endl;
	}

	fLog << "ȫ�������ڴ� " << nAllMemory << " Byte ���� " << nAllMemory/(1024*1024) << " MByte" << std::endl;
	fLog << "---------------heapmemoryinfoend---------------" << std::endl;

	fLog.close();
	chdir("../../");
};

#endif