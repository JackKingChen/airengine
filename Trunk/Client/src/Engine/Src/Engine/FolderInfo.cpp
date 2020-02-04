#include "stdafx.h"
#include "FolderInfo.h"
#include "StringUtil.h"
#include "DebugTools.h"
#include <assert.h>

#define DEBUG_OUT_FOLDER_INFO_TO_LOG 0

FolderInfo::FolderInfo()
: m_szFolderName("")
, m_szFolderFullPath("")
, m_pParentFolder(NULL)
{
	m_FileMap.clear();
	m_SubFolderMap.clear();
}

FolderInfo::~FolderInfo()
{
	m_pParentFolder = NULL;
	if ( m_FileMap.size() )
	{
		AIR_FILE_MAP::iterator itFile = m_FileMap.begin();
		for ( ; itFile != m_FileMap.end(); ++itFile )
		{
			//AIR_SAFE_DELETE_T(itFile->second, AIRFILEINFO);
            AIR_DELETE_T(itFile->second, AIRFILEINFO);
			//SafeDelete( itFile->second );
		}
		m_FileMap.clear();
	}

	if ( m_SubFolderMap.size() )
	{
		AIR_FOLDER_MAP::iterator itFolder = m_SubFolderMap.begin();
		for ( ; itFolder != m_SubFolderMap.end(); ++ itFolder )
		{
			//AIR_SAFE_DELETE_T( itFolder->second, FolderInfo );
			SafeDelete( itFolder->second );
		}
		m_SubFolderMap.clear();
	}
}

//根据文件名取得文件信息
LPAIRFILEINFO FolderInfo::FindFileInfo( const AIRString& fileName )
{
	LPAIRFILEINFO pFileInfo = NULL;
	LPFOLDERINFO pFolderInfo = NULL;

	AIR_FILE_MAP::iterator itFile = m_FileMap.find( fileName );
	if (itFile != m_FileMap.end())
	{
		return itFile->second;
	}
	AIR_FOLDER_MAP::iterator itFolder = m_SubFolderMap.begin();
	for ( ; itFolder != m_SubFolderMap.end(); ++itFolder )
	{
		pFolderInfo = itFolder->second;
		pFileInfo = pFolderInfo->FindFileInfo( fileName );
		if ( pFileInfo )
		{
			return pFileInfo;
		}
	}
	return NULL;
}

//获取文件列表
void FolderInfo::GetFileList( AIR_STRING_LIST& lst, eCollectOption opt /*= eCO_CurFolderFiles*/ )
{
	LPFOLDERINFO pFolderInfo = NULL;
	LPAIRFILEINFO pFileInfo = NULL;
	AIR_FILE_MAP::iterator itFile = m_FileMap.begin();
	for ( ; itFile != m_FileMap.end(); ++itFile )
	{
		pFileInfo = itFile->second;
		if ( opt & eCO_FileFullName )
		{
			lst.push_back( pFileInfo->strFullName );
		}
		else if ( opt & eCO_FileName )
		{
			lst.push_back( pFileInfo->strFileName );
		}
	}
	if ( opt & eCO_CurFolderAndAllSubFolders )
	{
		AIR_FOLDER_MAP::iterator itFolder = m_SubFolderMap.begin();
		for ( ; itFolder != m_SubFolderMap.end(); ++itFolder )
		{
			pFolderInfo = itFolder->second;
			pFolderInfo->GetFileList( lst, opt );
		}
	}
}

//获取子目录列表
//bCollectSubFoldersChildren : 是否获取子文件夹的子目录
void FolderInfo::GetSubFolderList( AIR_STRING_LIST& lst, bool bCollectSubFoldersChildren /*= true*/ )
{
	LPFOLDERINFO pFolderInfo = NULL;
	AIR_FOLDER_MAP::iterator itFolder = m_SubFolderMap.begin();
	for ( ; itFolder != m_SubFolderMap.end(); ++itFolder )
	{
		pFolderInfo = itFolder->second;
		lst.push_back( itFolder->first );
		if ( bCollectSubFoldersChildren )
		{
			pFolderInfo->GetSubFolderList( lst );
		}			
	}
}

FolderInfo* FolderInfo::FindFolderInfo( const AIRString& szFolderPath )
{
	if ( CStringUtil::IsSameFilePath(m_szFolderFullPath, szFolderPath) || CStringUtil::IsSameFilePath(m_strAbsoluteFullPath, szFolderPath))
	{
		return this;
	}
	FolderInfo* pFolder = NULL;
	FolderInfo* pSubFolder = NULL;
	AIR_FOLDER_MAP::iterator itFolder = m_SubFolderMap.begin();
	for ( ; itFolder != m_SubFolderMap.end(); ++itFolder )
	{
		pSubFolder = itFolder->second;
		pFolder = pSubFolder->FindFolderInfo(szFolderPath);
		if (pFolder)
		{
			return pFolder;
		}
	}
	return NULL;
}

void FolderInfo::DebugFolderInfo()
{
#if DEBUG_OUT_FOLDER_INFO_TO_LOG
	if ( NULL == m_pParentFolder )
	{
		DT_TO_LOG( LT_NONE, GetString("********************\nPackage : %s\n********************\n", m_szFolderName.c_str()) );
	}
	else
	{
		DT_TO_LOG( LT_NONE, GetString("Folder : %s\n", m_szFolderFullPath.c_str()) );
	}
	AIR_FILE_MAP::iterator itFile = m_FileMap.begin();
	for ( ; itFile != m_FileMap.end(); ++itFile )
	{
		DT_TO_LOG( LT_NONE, GetString("File :   %s\n", (itFile->second)->strFullName.c_str()) );
	}
	AIR_FOLDER_MAP::iterator itFolder = m_SubFolderMap.begin();
	for ( ; itFolder != m_SubFolderMap.end(); ++itFolder )
	{
		(itFolder->second)->DebugFolderInfo();
	}
#endif
}

FolderInfo* FolderInfo::CreateFolderInfo( FolderInfo* pRootFolder, const AIRString& szFolderPath )
{
	AIRVector<AIRString> vtPath;
	CStringUtil::SplitString( szFolderPath, "/", vtPath );
	AIRString szPath("");
	int i = 0;
	int c = vtPath.size();
	FolderInfo* pCurFolderInfo = pRootFolder;
	FolderInfo* pTmpFolderInfo = NULL;
	for ( ; i < c; ++i )
	{
		szPath = szPath + vtPath[i] + "/";
		pTmpFolderInfo = pCurFolderInfo->FindFolderInfo(szPath);
		if ( pTmpFolderInfo )
		{
			pCurFolderInfo = pTmpFolderInfo;
			continue;
		}
		else
		{
			//pTmpFolderInfo = AIR_NEW_T(FolderInfo);
			pTmpFolderInfo = new FolderInfo;
			pTmpFolderInfo->m_pParentFolder = pCurFolderInfo;
			pTmpFolderInfo->m_szFolderFullPath = szPath;
			pTmpFolderInfo->m_szFolderName = vtPath[i];
			pCurFolderInfo->m_SubFolderMap[ pTmpFolderInfo->m_szFolderFullPath ] = pTmpFolderInfo;

			pCurFolderInfo = pTmpFolderInfo;
		}
	}
	return pCurFolderInfo;
}

bool FolderInfo::CreateFileFolderInfo( FolderInfo* pRootFolder, const AIRString& szFilePath, const AIRString& szFileName, u32 uncompressedSize, u32 compressedSize )
{
	FolderInfo* pFolderInfo = CreateFolderInfo(pRootFolder, szFilePath);
	if (pFolderInfo)
	{
		LPAIRFILEINFO pFileInfo = AIR_NEW_T(AIRFILEINFO);
		//LPAIRFILEINFO pFileInfo = new AIRFILEINFO;
		pFileInfo->compressedSize = compressedSize;
		pFileInfo->uncompressedSize = uncompressedSize;
		pFileInfo->strFileName = szFileName;
		pFileInfo->strPath = szFilePath;
		pFileInfo->strFullName = szFilePath + szFileName;
		
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
        CStringUtil::ToLowerCase(pFileInfo->strFileName);
        CStringUtil::ToLowerCase(pFileInfo->strFullName);
        CStringUtil::ToLowerCase(pFileInfo->strPath);
#endif

        if (pFolderInfo->m_FileMap.find(pFileInfo->strFileName) != pFolderInfo->m_FileMap.end())
        {
            AIR_DELETE_T(pFileInfo, AIRFILEINFO);
            //DT_TO_MSGBOX_FMT(LT_ERROR, "FolderInfo::CreateFileFolderInfo filename[%s] already exist!", pFileInfo->strFileName);
            assert(false);
        }
        else
        {
            pFolderInfo->m_FileMap.insert(std::make_pair(pFileInfo->strFileName, pFileInfo));
        }

        //pFolderInfo->m_FileMap[pFileInfo->strFileName] = pFileInfo;
		return true;
	}
	return false;
}
