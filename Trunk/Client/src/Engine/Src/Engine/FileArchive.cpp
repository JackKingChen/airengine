#include "stdafx.h"
#include "FileArchive.h"
#include <stdio.h>  
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#include <io.h>
#else
#include <dirent.h>
#endif
#include <sys/stat.h>
#include "FileDataStream.h"
#include <assert.h>
#include "StringUtil.h"
#include "DebugTools.h"
#include "FileSystem.h"
#include "Engine.h"

CFileArchive::CFileArchive(const AIRString& strName)
	: CBaseArchive(strName)
{
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
    CStringUtil::ToLowerCase(m_strName);
#endif
    
	m_strName = CStringUtil::Replace( m_strName, "\\", "/" );

	u32 last = m_strName.length()-1;
	if ( last >= 0 && m_strName[last] != '/' )
	{
		m_strName += "/";
	}
	//m_pFolderInfo = AIR_NEW_T(FolderInfo);
	m_pFolderInfo = new FolderInfo;
	m_pFolderInfo->m_szFolderFullPath = m_strName;
	m_pFolderInfo->m_szFolderName = m_strName;

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	m_pFolderInfo->m_strAbsoluteFullPath = CStringUtil::CombineFilePath(FILESYSTEM->GetProcessPath(), m_strName); 
#elif AIR_TARGET_PLATFORM == AIR_PLATFORM_ANDROID || AIR_TARGET_PLATFORM == AIR_PLATFORM_IOS
	m_pFolderInfo->m_strAbsoluteFullPath = CStringUtil::CombineFilePath(ENGINE_INST->GetResourcePath(), m_strName);
#endif
    
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
	CStringUtil::ToLowerCase(m_pFolderInfo->m_strAbsoluteFullPath);
#endif
    
    GenarateFolderInfo( m_pFolderInfo );

	m_pFolderInfo->DebugFolderInfo();
}

CFileArchive::~CFileArchive()
{
	Clear();
}

bool CFileArchive::Exist(const AIRString& filename, AIRString& fullPath) const
{
    AIRString strLowerCaseFilename = filename;
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
    CStringUtil::ToLowerCase(strLowerCaseFilename);
#endif
	LPAIRFILEINFO pFileInfo = m_pFolderInfo->FindFileInfo( strLowerCaseFilename );
	if ( pFileInfo )
	{
		fullPath = pFileInfo->strFullName;
		return true;
	}
	return false;
}

bool CFileArchive::Exist(const AIRString& strPath, const AIRString& filename, AIRString& fullPath) const
{
	AIRString strLowerCasePath = strPath;
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
	CStringUtil::ToLowerCase(strLowerCasePath);
#endif
	LPFOLDERINFO pFolderInfo = m_pFolderInfo->FindFolderInfo(strLowerCasePath);
	if (pFolderInfo)
	{
		AIRString strLowerCaseFilename = filename;
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
		CStringUtil::ToLowerCase(strLowerCaseFilename);
#endif
		LPAIRFILEINFO pFileInfo = m_pFolderInfo->FindFileInfo( strLowerCaseFilename );
		if ( pFileInfo )
		{
			fullPath = pFileInfo->strFullName;
			return true;
		}
	}
	
	return false;
}

CDataStream* CFileArchive::Open(const AIRString& filename)
{
#ifdef _WIN32
    DT_TO_DBGSTR(LT_NONE, GetString("Open file [%s] in thread [%d]\n", filename.c_str(), ::GetCurrentThreadId()));
#endif // _WIN32
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

    AIRString full_path("");
    full_path = GetFileFullPath(filename.c_str());
	if (full_path.length() == 0)
	{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		std::locale::global( std::locale(loc1) );
#endif
		return NULL;
	}

    // Always open in binary mode
    std::ifstream *origStream = AIR_NEW_T(std::ifstream);
    origStream->open(full_path.c_str(), std::ios::in | std::ios::binary);

    // Should check ensure open succeeded, in case fail for some reason.
    if (origStream->fail())
    {
		//SafeDelete(origStream);
        AIR_DELETE_T(origStream, std::ifstream);
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		std::locale::global( std::locale(loc1) );
#endif
        return NULL;
    }

    /// Construct return stream, tell it to delete on destroy
    CFileDataStream* stream = new CFileDataStream(filename, origStream);  //OGRE_NEW FileStreamDataStream(filename, origStream, tagStat.st_size, true);
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
	return stream;
}

bool CFileArchive::IsAbsolutePath(const char* szFilename) const
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

    struct stat tagStat;
    int ret = stat(szFilename, &tagStat);
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
    return ret == 0;
}

AIRString CFileArchive::GetFileFullPath(const char* szFilename)
{
	AIRString fullPath("");
	if ( IsAbsolutePath(szFilename) )
	{
		fullPath = szFilename;
		return fullPath;
	}	
	if ( this->Exist( szFilename, fullPath ) )
	{
		return fullPath;
	}
	return fullPath;
}

void CFileArchive::Clear()
{
    //AIR_SAFE_DELETE_T(m_pFolderInfo, FolderInfo);
	SafeDelete( m_pFolderInfo );
}

void CFileArchive::GetFileList(AIRList<AIRString>& lstFiles, eCollectOption opt)
{
	m_pFolderInfo->GetFileList( lstFiles, opt );
}
/*
void CFileArchive::GetSubFolders( AIRList<AIRString>& folders )
{
	m_pFolderInfo->GetSubFolderList( folders, true );
}
*/
void CFileArchive::GenarateFolderInfo( FolderInfo* pCurFolder )
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    long lHandle, res;
    struct _finddata_t tagData;
    AIRString  strCurr = pCurFolder->m_szFolderFullPath + "*.*"; 

    lHandle = _findfirst(strCurr.c_str(), &tagData);
    res = 0;

    while (lHandle != -1 && res != -1)
    {
		if ( strcmp(tagData.name, ".") != 0 && strcmp(tagData.name, "..") != 0 )
		{
			const AIRString fullpath = pCurFolder->m_szFolderFullPath + tagData.name;
			// «◊”ƒø¬º
			if ( tagData.attrib & _A_SUBDIR )   
			{
				if (!(tagData.attrib & _A_HIDDEN)/* && strcmp( tagData.name, ".svn")*/ )
				{
					//FolderInfo* pSubFolder = AIR_NEW_T(FolderInfo);
					FolderInfo* pSubFolder = new FolderInfo;
					pSubFolder->m_pParentFolder = pCurFolder;
					pSubFolder->m_szFolderName = tagData.name;
					pSubFolder->m_szFolderFullPath = fullpath + "/";
					pSubFolder->m_strAbsoluteFullPath = FILESYSTEM->GetProcessPath() + AIRString("/") + tagData.name;

                    CStringUtil::ToLowerCase(pSubFolder->m_szFolderName);
                    CStringUtil::ToLowerCase(pSubFolder->m_szFolderFullPath);

					//pCurFolder->m_SubFolderMap[ pSubFolder->m_szFolderFullPath ] = pSubFolder;
                    AIR_FOLDER_MAP::iterator it = pCurFolder->m_SubFolderMap.find(pSubFolder->m_szFolderFullPath);
                    if (it != pCurFolder->m_SubFolderMap.end())
                    {
                        SafeDelete(pSubFolder);
                        DT_TO_MSGBOX_FMT(LT_ERROR, "CFileArchive::GenarateFolderInfo filename[%s] already exist!", pFileInfo->strFileName);
                    }
                    else
                    {
                        pCurFolder->m_SubFolderMap.insert(std::make_pair(pSubFolder->m_szFolderFullPath, pSubFolder));
                    }
				}
			}
			//◊”Œƒº˛
			else
			{
				LPAIRFILEINFO pFileInfo = AIR_NEW_T(AIRFILEINFO);
				//LPAIRFILEINFO pFileInfo = new AIRFILEINFO;
				pFileInfo->compressedSize = tagData.size;
				pFileInfo->uncompressedSize = tagData.size;
				pFileInfo->strFileName = tagData.name;
				pFileInfo->strFullName = fullpath;
				pFileInfo->strPath = pCurFolder->m_szFolderFullPath;

                CStringUtil::ToLowerCase(pFileInfo->strFileName);
                CStringUtil::ToLowerCase(pFileInfo->strFullName);
                CStringUtil::ToLowerCase(pFileInfo->strPath);

				//pCurFolder->m_FileMap[ pFileInfo->strFileName ] = pFileInfo;
                AIR_FILE_MAP::iterator it = pCurFolder->m_FileMap.find(pFileInfo->strFileName);
                
                if (it != pCurFolder->m_FileMap.end())
                {
                    AIR_DELETE_T(pFileInfo, AIRFILEINFO);
                    DT_TO_MSGBOX_FMT(LT_ERROR, "CFileArchive::GenarateFolderInfo filename[%s] already exist!", pFileInfo->strFileName);
                }
                else
                {
                    pCurFolder->m_FileMap.insert(std::make_pair(pFileInfo->strFileName, pFileInfo));
                }
			}
		}

        res = _findnext(lHandle, &tagData);
    }

    _findclose(lHandle);
#else
	AIRString  file_folder = pCurFolder->m_szFolderFullPath; 

	DIR *pDir = NULL;
	struct dirent *dmsg;
	struct stat stat_buf;
	char szFileName[256];
	char szFolderName[256];

	strcpy(szFolderName, file_folder.c_str());
	int lastch = file_folder.length()-1;
	if( lastch >= 0 && (file_folder[lastch] == '/' || file_folder[lastch] == '\\') )
		strcat(szFolderName, "%s");
	else
		strcat(szFolderName, "/%s");
	if ((pDir = opendir(file_folder.c_str())) != NULL)
	{
		// ±È¿˙ƒø¬º
		while ((dmsg = readdir(pDir)) != NULL)
		{
			if (strcmp(dmsg->d_name, ".") != 0 && strcmp(dmsg->d_name, "..") != 0)
			{
				const AIRString fullpath = pCurFolder->m_szFolderFullPath + dmsg->d_name;
				// «◊”ƒø¬º
				if ( dmsg->d_type & DT_DIR )   
				{
					//if (!(tagData.attrib & _A_HIDDEN)/* && strcmp( tagData.name, ".svn")*/ )
					{
						//FolderInfo* pSubFolder = AIR_NEW_T(FolderInfo);
						FolderInfo* pSubFolder = new FolderInfo;
						pSubFolder->m_pParentFolder = pCurFolder;
						pSubFolder->m_szFolderName = dmsg->d_name;
						pSubFolder->m_szFolderFullPath = fullpath + "/";
						pSubFolder->m_strAbsoluteFullPath = FILESYSTEM->GetProcessPath() + AIRString("/") + dmsg->d_name;

#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
						CStringUtil::ToLowerCase(pSubFolder->m_szFolderName);
						CStringUtil::ToLowerCase(pSubFolder->m_szFolderFullPath);
#endif
                        
						//pCurFolder->m_SubFolderMap[ pSubFolder->m_szFolderFullPath ] = pSubFolder;
						AIR_FOLDER_MAP::iterator it = pCurFolder->m_SubFolderMap.find(pSubFolder->m_szFolderFullPath);
						if (it != pCurFolder->m_SubFolderMap.end())
						{
							SafeDelete(pSubFolder);
							//DT_TO_MSGBOX_FMT(LT_ERROR, "CFileArchive::GenarateFolderInfo filename[%s] already exist!", pFileInfo->strFileName);
						}
						else
						{
							pCurFolder->m_SubFolderMap.insert(std::make_pair(pSubFolder->m_szFolderFullPath, pSubFolder));
						}
					}
				}
				//◊”Œƒº˛
				else
				{
					lstat( fullpath.c_str(), &stat_buf );

					LPAIRFILEINFO pFileInfo = AIR_NEW_T(AIRFILEINFO);
					//LPAIRFILEINFO pFileInfo = new AIRFILEINFO;
					pFileInfo->compressedSize = stat_buf.st_size;
					pFileInfo->uncompressedSize = stat_buf.st_size;
					pFileInfo->strFileName = dmsg->d_name;
					pFileInfo->strFullName = fullpath;
					pFileInfo->strPath = pCurFolder->m_szFolderFullPath;

#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
					CStringUtil::ToLowerCase(pFileInfo->strFileName);
					CStringUtil::ToLowerCase(pFileInfo->strFullName);
					CStringUtil::ToLowerCase(pFileInfo->strPath);
#endif

					//pCurFolder->m_FileMap[ pFileInfo->strFileName ] = pFileInfo;
					AIR_FILE_MAP::iterator it = pCurFolder->m_FileMap.find(pFileInfo->strFileName);

					if (it != pCurFolder->m_FileMap.end())
					{
						AIR_DELETE_T(pFileInfo, AIRFILEINFO);
						//DT_TO_MSGBOX_FMT(LT_ERROR, "CFileArchive::GenarateFolderInfo filename[%s] already exist!", pFileInfo->strFileName);
					}
					else
					{
						pCurFolder->m_FileMap.insert(std::make_pair(pFileInfo->strFileName, pFileInfo));
					}
				}
			}
		}
	}

	if (pDir != NULL)
	{
		closedir(pDir);
	}


#endif

	//…˙≥…◊”ƒø¬ºµƒ–≈œ¢
	if ( pCurFolder->m_SubFolderMap.size() )
	{
		AIR_FOLDER_MAP::iterator itFolder = pCurFolder->m_SubFolderMap.begin();
		AIR_FOLDER_MAP::iterator itFolderEnd = pCurFolder->m_SubFolderMap.end();
		for ( ; itFolder != itFolderEnd; ++itFolder )
		{
			GenarateFolderInfo( itFolder->second );
		}
	}
}



//
//CFileArchive::CFileArchive(const AIRString& strName) : CBaseArchive(strName)
//{
//    CStringUtil::ToLowerCase(m_strName);
//    /*
//    VectorString vtString;
//    long lHandle, res;
//    struct _finddata_t tagData;
//    string  strCurr = strName + "\\*.*"; 
//
//    lHandle = _findfirst(strCurr.c_str(), &tagData);
//    res = 0;
//
//    while (lHandle != -1 && res != -1)
//    {
//        if (!(tagData.attrib & _A_SUBDIR) && strcmp(tagData.name, ".") != 0 && strcmp(tagData.name, "..") != 0)   //≤ª «◊”ƒø¬º
//        {
//            AIRFILEINFO info;
//            info.nSize = tagData.size;
//            info.strFullName = strName + "/" + tagData.name;
//            info.strFileName = tagData.name;
//            m_vtFiles.push_back(info);
//        }
//
//        res = _findnext(lHandle, &tagData);
//    }
//
//    _findclose(lHandle);
//    */
//    GetSubFolders(m_strName);
//}
//
//CFileArchive::~CFileArchive()
//{
//    m_vtFiles.clear();
//}
//
//void CFileArchive::Unload()
//{
//
//}
//
//bool CFileArchive::Exist(const AIRString& filename, AIRString& fullPath) const
//{
//    //_Mutex_Help _sc(&m_sect);
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//    
//    //AIRString full_path = m_strName + "/" + filename;
//    /*
//    struct stat tagStat;
//    bool ret = (stat(filename.c_str(), &tagStat) == 0);
//
//    if (!ret)
//    {
//        ret = (stat(full_path.c_str(), &tagStat) == 0);
//    }
//    */
//	fullPath = m_strName + "/" + filename;
//	struct stat tagStat;
//	bool ret = (stat(filename.c_str(), &tagStat) == 0);
//	if (!ret)
//	{
//		ret = (stat(fullPath.c_str(), &tagStat) == 0);
//	}
//	return ret;
//    //return GetFullPath(fullPath, filename);
//}
//
//CDataStream* CFileArchive::Open(const AIRString& filename) const
//{
//#ifdef _WIN32
//    DT_TO_DBGSTR(LT_NONE, GetString("Open file [%s] in thread [%d]\n", filename.c_str(), ::GetCurrentThreadId()));
//#endif // _WIN32
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    AIRString full_path;
//
//    /*
//    struct stat tagStat;
//    int ret = stat(filename.c_str(), &tagStat);
//    if (ret == 0)
//    {
//        full_path = filename;
//    }
//    else
//        full_path = m_strName + "/" + filename;
//
//    // Use filesystem to determine size 
//    // (quicker than streaming to the end and back)
//    
//    ret = stat(full_path.c_str(), &tagStat);
//
//    if (ret != 0)
//    {
//    }
//    */
//    //assert(ret == 0 && "Problem getting file size" );
//
//    if (!GetFullPath(full_path, filename))
//    {
//        return NULL;
//    }
//
//    // Always open in binary mode
//    std::ifstream *origStream = AIR_NEW_T(std::ifstream);
//    origStream->open(full_path.c_str(), std::ios::in | std::ios::binary);
//
//    // Should check ensure open succeeded, in case fail for some reason.
//    if (origStream->fail())
//    {
//		//SafeDelete(origStream);
//        AIR_DELETE_T(origStream, std::ifstream);
//        return NULL;
//    }
//
//    /// Construct return stream, tell it to delete on destroy
//    CFileDataStream* stream = new CFileDataStream(filename, origStream);  //OGRE_NEW FileStreamDataStream(filename, origStream, tagStat.st_size, true);
//    return stream;
//}
//
//bool CFileArchive::IsAbsolutePath(const char* szFilename) const
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    struct stat tagStat;
//    int ret = stat(szFilename, &tagStat);
//
//    return ret == 0;
//}
//
//AIRString CFileArchive::GetFileFullPath(const char* szFilename)
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    if (IsAbsolutePath(szFilename))
//    {
//        return AIRString(szFilename);
//    }
//    //return m_strName + "/" + szFilename;
//	AIRString fullPath;
//	GetFullPath( fullPath, szFilename );
//	return fullPath;
//}
//
//void CFileArchive::Clear()
//{
//    m_vtFiles.clear();
//}
//
//void CFileArchive::GetFileList(AIRList<AIRString>& lstFiles)
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    long lHandle, res;
//    struct _finddata_t tagData;
//    AIRString  strCurr = m_strName + "\\*.*"; 
//
//    lHandle = _findfirst(strCurr.c_str(), &tagData);
//    res = 0;
//
//    while (lHandle != -1 && res != -1)
//    {
//        if (!(tagData.attrib & _A_SUBDIR) && strcmp(tagData.name, ".") != 0 && strcmp(tagData.name, "..") != 0)   //≤ª «◊”ƒø¬º
//        {
//            AIRFILEINFO info;
//            info.nSize = tagData.size;
//            info.strFullName = m_strName + "/" + tagData.name;
//            info.strFileName = tagData.name;
//            lstFiles.push_back(info.strFileName);
//            //m_vtFiles.push_back(info);
//        }
//
//        res = _findnext(lHandle, &tagData);
//    }
//
//    _findclose(lHandle);
//}
//
//void CFileArchive::GetSubFolders( AIRVector<AIRString>& folders )
//{
//	if( m_vecSubFolders.size() == 0 )
//	{
//		AIRString str = m_strName;
//		GetSubFolders(str);
//	}
//	for ( int i = m_vecSubFolders.size()-1; i >= 0; --i )
//	{
//		folders.push_back( m_vecSubFolders[i] );
//	}
//}
//
//void CFileArchive::GetSubFolders(const AIRString& strFolder)
//{
//    long lHandle, res;
//    struct _finddata_t tagData;
//    AIRString  strCurr = strFolder + "\\*.*"; 
//
//    lHandle = _findfirst(strCurr.c_str(), &tagData);
//    res = 0;
//
//    while (lHandle != -1 && res != -1)
//    {
//        if ((tagData.attrib & _A_SUBDIR) && strcmp(tagData.name, ".") != 0 && strcmp(tagData.name, "..") != 0)   // «◊”ƒø¬º
//        {
//			if ( strcmp( tagData.name, ".svn") )
//			{
//				AIRString strSubFolder = strFolder + "/" + tagData.name;
//				m_vecSubFolders.push_back(strSubFolder);
//				GetSubFolders(strSubFolder);
//			}
//        }
//
//        res = _findnext(lHandle, &tagData);
//    }
//
//    _findclose(lHandle);
//}
//
//bool CFileArchive::GetFullPath(AIRString& strFullPath, const AIRString& strFilename) const
//{
//    struct stat tagStat;
//    int ret = stat(strFilename.c_str(), &tagStat);
//    if (ret == 0)
//    {
//        strFullPath = strFilename;
//    }
//    else
//        strFullPath = m_strName + "/" + strFilename;
//
//    ret = stat(strFullPath.c_str(), &tagStat);
//
//    if (ret != 0)
//    {
//        for (size_t i = 0; i < m_vecSubFolders.size(); ++i)
//        {
//            strFullPath = m_vecSubFolders[i] + strFilename;
//            ret = stat(strFullPath.c_str(), &tagStat);
//            if (ret == 0)
//            {
//                return true;
//            }
//            else
//            {
//                strFullPath = m_vecSubFolders[i] + "/" + strFilename;
//
//                ret = stat(strFullPath.c_str(), &tagStat);
//                if (ret == 0)
//                {
//                    return true;
//                }
//            }
//        }
//    }
//    else
//    {
//        return true;
//    }
//
//    return false;
//}
