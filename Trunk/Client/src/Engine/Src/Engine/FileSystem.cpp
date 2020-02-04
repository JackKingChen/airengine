#include "stdafx.h"
#include "FileSystem.h"
#include "Util.h"
#include <stdio.h>   
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif
#include "CfgReader.h"
#include "FileArchive.h"
#include "ZipArchive.h"
#include "StringUtil.h"
#include "DebugTools.h"
#include "Engine.h"

//CFileSystem* CFileSystem::m_pInstance = NULL;

CFileSystem::CFileSystem()
{
    //std::locale::global(std::locale(""));  //能够读中文
	char* buffer;

	// Get the current working directory: 
	if( (buffer = getcwd( NULL, 0 )) == NULL )
	{
		//perror( "_getcwd error" );
	}

	if (buffer)
	{
		m_strProcessPath = buffer;
	}
}

CFileSystem::~CFileSystem()
{
    //SafeDelete(m_pInstance);
}

/*
void CFileSystem::LoadCFGResource(const char* szCFGFile)
{
    ConfigReader reader;
    if (!reader.ReadFile(szCFGFile))
    {
        return;
    }

    char* szValue = reader.GetString("FileSystem");
    while (szValue)
    {
        AddFileLocation(string(szValue));
        szValue = reader.GetString("FileSystem");
    }
}
*/

//void CFileSystem::GenSubFolderArchive()
//{
//	ARCHIVE_MAP::iterator it = m_mapArchives.begin(), ite = m_mapArchives.end();
//	AIRList<CBaseArchive*> archiveLst;
//	for ( ; it != ite; ++it )
//	{
//		CFileArchive* pFArchive = (CFileArchive*)it->second;
//		if ( pFArchive )
//		{
//			archiveLst.push_back(pFArchive);
//		}
//	}
//	AIRList<CBaseArchive*>::iterator lit = archiveLst.begin(), lite = archiveLst.end();
//	for ( ; lit != lite; ++ lit )
//	{
//		CBaseArchive* pArchive = *lit;
//		{
//			AIRVector<AIRString> folders;
//			pArchive->GetSubFolders(folders);
//			for ( int i = 0; i < folders.size(); ++i )
//			{
//				CBaseArchive* pSubArchive = FILESYSTEM->GetArchive(folders[i]);
//				if ( !pSubArchive )
//				{
//					pSubArchive = FILESYSTEM->AddFileArchive(folders[i]);
//				}		
//			}
//		}
//	}
//}

void CFileSystem::LoadXMLResource(const char* szXMLFile)
{
    CMarkupSTL xml;
    if (!xml.Load(szXMLFile))
    {
        return;
    }

    //xml.IntoElem();

    AIRString strNode;

    if (xml.FindElem("FileSystem"))
    {
        xml.IntoElem();
        while (xml.FindElem())
        {
            strNode = xml.GetTagName().c_str();
            AIRString szpath = xml.GetData().c_str();
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
            szpath = ENGINE_INST->GetResourcePath() + szpath;
#endif
            if (strNode == "Path")
            {
                AddFileArchive(szpath.c_str());
            }
            else if (strNode == "Zip")
            {
                AddZipArchive(szpath.c_str());
            }
        }
        
        xml.OutOfElem();
    }
    //xml.OutOfElem();
}

/*
AIRString CFileSystem::GetFileAbsolutePath(const char* szName)
{
    if (IsFileExist(szName))
    {
        return szName;
    }
    for (FileInfo_Iter it = m_vtFiles.begin(); it != m_vtFiles.end(); it++)
    {
        AIRFILEINFO* pInfo = &(*it);
        if (strcmp(pInfo->strFileName.c_str(), szName) == 0)
        {
            return pInfo->strFullName;
        }
    }
    return AIRString("");
}


AIRString CFileSystem::GetFileAbsolutePath(const AIRString& strName)
{
    if (IsFileExist(strName.c_str()))
    {
        return strName;
    }
    for (FileInfo_Iter it = m_vtFiles.begin(); it != m_vtFiles.end(); it++)
    {
        AIRFILEINFO* pInfo = &(*it);
        if (strcmp(pInfo->strFileName.c_str(), strName.c_str()) == 0)
        {
            return pInfo->strFullName;
        }
    }
    return AIRString("");
}
*/

AIRString CFileSystem::GetFileFullPath(const char* szFilename)
{
    _Mutex_Help _sc(&m_sect);
#ifdef _WIN32
    DT_TO_DBGSTR(LT_NONE, GetString("GetFileFullPath [%s] in thread [%d]\n", szFilename, ::GetCurrentThreadId()));
#endif // _WIN32
	
    for (ARCHIVE_MAP_ITER it = m_mapArchives.begin(); it != m_mapArchives.end(); it++)
    {
		AIRString fullPath;
        CBaseArchive* pArchive = it->second;
        if (pArchive->Exist(szFilename, fullPath))
        {
            //return pArchive->GetFileFullPath(szFilename);
            //return pArchive->Open(szName);
			return fullPath;
        }
    }

    return "";
}
//
//const char* CFileSystem::GetFilePath(const char* szFilename)
//{
//    _Mutex_Help _sc(&m_sect);
//    for (ARCHIVE_MAP_CONST_ITER it = m_mapArchives.begin(); it != m_mapArchives.end(); it++)
//    {
//        CBaseArchive* pArchive = it->second;
//		AIRString fullPath;
//        if (pArchive->Exist(szFilename,fullPath))
//        {
//            return pArchive->GetName().c_str();
//            //return pArchive->Open(szName);
//        }
//    }
//
//    return 0;
//}

CDataStream* CFileSystem::GetFileStream(const char* szName)
{
    _Mutex_Help _sc(&m_sect);
    for (ARCHIVE_MAP_ITER it = m_mapArchives.begin(); it != m_mapArchives.end(); it++)
    {
        CBaseArchive* pArchive = it->second;
		AIRString fullPath;
        if (pArchive->Exist(szName,fullPath))
        {
            return pArchive->Open(szName);
        }
    }

    return NULL;
}

CDataStream* CFileSystem::GetFileStream(const char* szPathName, const char* szName)
{
	_Mutex_Help _sc(&m_sect);
	AIRString pathDir = szPathName;
	if( pathDir[pathDir.size()-1] != '/' )
		pathDir += "/";

	for (ARCHIVE_MAP_ITER it = m_mapArchives.begin(); it != m_mapArchives.end(); it++)
	{
		CBaseArchive* pArchive = it->second;
		AIRString fullPath;
		if (pArchive->Exist(pathDir, szName, fullPath))
		{
			return pArchive->Open(fullPath);
		}
	}

	return NULL;
}

bool CFileSystem::IsFileExist(const char* szFilename)
{
    return access(szFilename, 0) == 0;
}

bool CFileSystem::IsFileExist(const char* szPathName, const char* szFilename)
{
	AIRString fullpath = AIRString(szPathName) + AIRString("/") + AIRString(szFilename);
	return IsFileExist( fullpath.c_str());
}

CBaseArchive* CFileSystem::GetArchive(const AIRString& archiveName)
{
    _Mutex_Help _sc(&m_sect);
    AIRString strName = archiveName;
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
    CStringUtil::ToLowerCase(strName);
#endif
    ARCHIVE_MAP_ITER it = m_mapArchives.find(strName);
    if (it != m_mapArchives.end())
    {
        return it->second;
    }

    return NULL;
}

CBaseArchive*  CFileSystem::AddFileArchive(const AIRString& strFilePath)
{
    _Mutex_Help _sc(&m_sect);
    CBaseArchive* pArchive = GetArchive(strFilePath);

    if (pArchive == NULL)
    {
        pArchive = new CFileArchive(strFilePath);
        AIRString str = strFilePath;
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
        CStringUtil::ToLowerCase(str);
#endif
        m_mapArchives.insert(std::make_pair(str, pArchive));
    }

	return pArchive;
    /*
    VectorString vtString;
    long lHandle, res;
    struct _finddata_t tagData;
    string  strCurr = strFilePath + "\\*.*"; 

    lHandle = _findfirst(strCurr.c_str(), &tagData);
    res = 0;

    while (lHandle != -1 && res != -1)
    {
        if (!(tagData.attrib & _A_SUBDIR) && strcmp(tagData.name, ".") != 0 && strcmp(tagData.name, "..") != 0)   //不是子目录
        {
            AIRFILEINFO info;
            info.nSize = tagData.size;
            info.strFullName = strFilePath + "/" + tagData.name;
            info.strFileName = tagData.name;
            m_vtFiles.push_back(info);
        }

        res = _findnext(lHandle, &tagData);
    }

    _findclose(lHandle);
    */
}

CBaseArchive*  CFileSystem::AddZipArchive(const AIRString& strFilePath)
{
    _Mutex_Help _sc(&m_sect);
    CBaseArchive* pArchive = GetArchive(strFilePath);

    if (pArchive == NULL)
    {
        pArchive = new CZipArchive(strFilePath);
        AIRString str = strFilePath;
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
        CStringUtil::ToLowerCase(str);
#endif
        m_mapArchives.insert(std::make_pair(strFilePath, pArchive));
    }
	return pArchive;
}

void CFileSystem::Clear()
{
    _Mutex_Help _sc(&m_sect);
    for (ARCHIVE_MAP_ITER it = m_mapArchives.begin(); it != m_mapArchives.end(); ++it)
    {
        CBaseArchive* pArchive = it->second;
        pArchive->Clear();
        SafeDelete(pArchive);
    }

    m_mapArchives.clear();
}

bool CFileSystem::GetAllFileLst( AIRList<AIRString>& fileLst, eCollectOption opt )
{
	_Mutex_Help _sc(&m_sect);
	for (ARCHIVE_MAP_ITER it = m_mapArchives.begin(); it != m_mapArchives.end(); ++it)
	{
		CBaseArchive* pArchive = it->second;
		
		CFileArchive* pFA = dynamic_cast<CFileArchive*>(pArchive);
		if( pFA )
			pArchive->GetFileList(fileLst, opt);
	}
	return true;
}