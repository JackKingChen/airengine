/********************************************************************
	created:	2009/12/17
	filename: 	d:\My Codes\AIR_DEV\Client\Trunk\Engine\Include\Engine\FileSystem.h
	file path:	d:\My Codes\AIR_DEV\Client\Trunk\Engine\Include\Engine
	author:		liangairan
	
	purpose:	�ļ�ϵͳ����Ҫ�Ǹ����ļ����أ�
                ������Ϊ��IO�߳�Ҫ���ʸ��࣬���Ը���ҲҪ����  2011-7-17
*********************************************************************/
#pragma once
#include "Common.h"
#include "MarkupSTL.h"
#include "EngineConfig.h"
#include "BaseArchive.h"
#include "Singleton.h"
#include "_Mutex.h"

class EngineExport CFileSystem : public Singleton<CFileSystem>
{
    BE_SINGLETON(CFileSystem);
public:
    typedef AIRVector<AIRString> VectorString;
    
    //typedef std::map<AIRString, VectorFileInfo> MAP_FileInfo;
    //typedef MAP_FileInfo::iterator MAP_FileInfo_Iter;
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#if AIR_COMP_VER >= 1600
    typedef AIRHashMap<AIRString, CBaseArchive*, hash<AIRString>> ARCHIVE_MAP;
#else
    typedef AIRHashMap<AIRString, CBaseArchive*> ARCHIVE_MAP;
#endif
#else
	typedef AIRMap<AIRString, CBaseArchive*> ARCHIVE_MAP;
#endif
    typedef ARCHIVE_MAP::iterator ARCHIVE_MAP_ITER;
    typedef ARCHIVE_MAP::const_iterator ARCHIVE_MAP_CONST_ITER;
public:
    ~CFileSystem();

    //����ļ�����·��
    AIRString GetFileFullPath(const char* szFilename);

    //const char* GetFilePath(const char* szFilename);


    //����ļ��ڴ�����
    CDataStream* GetFileStream(const char* szName);

	//����ļ�����
	CDataStream* GetFileStream(const char* szPathName, const char* szName);

    //�ж��ļ��Ƿ����
    bool IsFileExist(const char* szFilename);

	bool IsFileExist(const char* szPathName, const char* szFilename);

    void LoadXMLResource(const char* szXMLFile);


    CBaseArchive* GetArchive(const AIRString& archiveName);

    void Clear();

	//void GenSubFolderArchive();


//private:
    CBaseArchive* AddFileArchive(const AIRString& strFilePath);

    CBaseArchive* AddZipArchive(const AIRString& strFilePath);

	const AIRString& GetProcessPath() const
	{
		return m_strProcessPath;
	}

	bool GetAllFileLst( AIRList<AIRString>& fileLst, eCollectOption opt = eCO_AllFolderFiles );
protected:
private:

    ARCHIVE_MAP  m_mapArchives;   //first���ļ��� second���ļ��б�

    _Mutex_Body_ m_sect;    //�߳����������̰߳�ȫ

	AIRString m_strProcessPath;   //��������Ŀ¼ 
};

#define FILESYSTEM CFileSystem::GetInstance()
