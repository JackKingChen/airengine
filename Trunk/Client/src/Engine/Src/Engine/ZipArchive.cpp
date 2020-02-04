#include "stdafx.h"

#include "ZipArchive.h"
#include "StringUtil.h"
#include "ZipDataStream.h"
//#include "DetectMemLeak.h"

AIRString getZzipErrorDescription(zzip_error_t zzipError) 
{
    AIRString errorMsg;
    switch (zzipError)
    {
    case ZZIP_NO_ERROR:
        break;
    case ZZIP_OUTOFMEM:
        errorMsg = "Out of memory.";
        break;            
    case ZZIP_DIR_OPEN:
    case ZZIP_DIR_STAT: 
    case ZZIP_DIR_SEEK:
    case ZZIP_DIR_READ:
        errorMsg = "Unable to read zip file.";
        break;            
    case ZZIP_UNSUPP_COMPR:
        errorMsg = "Unsupported compression format.";
        break;            
    case ZZIP_CORRUPTED:
        errorMsg = "Corrupted archive.";
        break;            
    default:
        errorMsg = "Unknown error.";
        break;            
    };

    return errorMsg;
}


CZipArchive::CZipArchive(const AIRString& strName) : CBaseArchive(strName), m_ZzipDir(NULL)
{
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
	CStringUtil::ToLowerCase(m_strName);
#endif
	Load();

	m_pFolderInfo->DebugFolderInfo();
}

CZipArchive::~CZipArchive(void)
{
	Clear();
}

void CZipArchive::CheckZzipError(int zzipError, const AIRString& operation) const
{
	if (zzipError != ZZIP_NO_ERROR)
	{
		AIRString errorMsg = getZzipErrorDescription(static_cast<zzip_error_t>(zzipError));
	}
}

bool CZipArchive::Load()
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

	if (!m_ZzipDir)
	{
		zzip_error_t zzipError;
		//��zip�ļ�
		m_ZzipDir = zzip_dir_open(m_strName.c_str(), &zzipError);
		CheckZzipError(zzipError, "opening archive");

		//�������ļ�����Ϣ��
		//m_pFolderInfo = AIR_NEW_T(FolderInfo);
		m_pFolderInfo = new FolderInfo;
		m_pFolderInfo->m_szFolderFullPath = m_strName;
		m_pFolderInfo->m_szFolderName = m_strName;

		// Cache names
		ZZIP_DIRENT zzipEntry;
		//����zip�ļ������ÿ���ļ�/�ļ���
		while (zzip_dir_read(m_ZzipDir, &zzipEntry))
		{
			//�ļ�/�ļ���ȫ��
			AIRString strFileFullName = zzipEntry.d_name;
			AIRString strPath, strFileName;
			CStringUtil::SplitFilename(strFileFullName, strPath, strFileName);

			//�ļ����ǿյ�
			if (strFileName.empty())
			{
				//��·����
				if ( !strPath.empty() )
				{
					//�������ļ�����Ϣ��
					FolderInfo::CreateFolderInfo( m_pFolderInfo, strPath + "/" );
				}
			}
			else
			{
				//���������Archive�ĸ�Ŀ¼��,��Ŀ¼·�����ƺ���Ҫ��"/"
				if ( !strPath.empty() )
				{
					strPath += "/";
				}
				//������Ŀ¼�µ��ļ���Ϣ��
				FolderInfo::CreateFileFolderInfo( 
					m_pFolderInfo, 
					strPath, 
					strFileName, 
					u32(zzipEntry.st_size),
					u32(zzipEntry.d_csize));
			}
		}
	}
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
	return true;
}

void CZipArchive::Clear()
{
	if (m_ZzipDir)
	{
		//�ر�zip�ļ�
		zzip_dir_close(m_ZzipDir);
		m_ZzipDir = 0;
	}
	//AIR_SAFE_DELETE_T(m_pFolderInfo, FolderInfo);
	SafeDelete( m_pFolderInfo );
}

AIRString CZipArchive::GetFileFullPath(const char* szFilename)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

	LPAIRFILEINFO pFileInfo = m_pFolderInfo->FindFileInfo( szFilename );
	if (pFileInfo)
	{
		return pFileInfo->strFullName;
	}
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
	return "";
}

CDataStream* CZipArchive::Open(const AIRString& filename)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

	AIRString fileFullPath = GetFileFullPath(filename.c_str());
	// Format not used here (always binary)
	ZZIP_FILE* zzipFile = 
		zzip_file_open(m_ZzipDir, fileFullPath.c_str(), ZZIP_ONLYZIP | ZZIP_CASELESS);
	if (!zzipFile)
	{
		int zerr = zzip_error(m_ZzipDir);
		AIRString zzDesc = getZzipErrorDescription((zzip_error_t)zerr);
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
		std::locale::global( std::locale(loc1) );
#endif
		return NULL ;
	}

	// Get uncompressed size too
	ZZIP_STAT zstat;
	zzip_dir_stat(m_ZzipDir, fileFullPath.c_str(), &zstat, ZZIP_CASEINSENSITIVE);

	// Construct & return stream
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
	return new CZipDataStream(filename, zzipFile, static_cast<size_t>(zstat.st_size)); //DataStreamPtr(OGRE_NEW ZipDataStream(filename, zzipFile, static_cast<size_t>(zstat.st_size)));

}

bool CZipArchive::Exist(const AIRString& filename, AIRString& fullPath) const
{
	LPAIRFILEINFO pFileInfo = m_pFolderInfo->FindFileInfo( filename );
	if ( pFileInfo )
	{
		fullPath = pFileInfo->strFullName;
		return true;
	}
	return false;
}

bool CZipArchive::Exist(const AIRString& strPath, const AIRString& filename, AIRString& fullPath) const
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

void CZipArchive::GetFileList(AIRList<AIRString>& lstFiles, eCollectOption opt)
{
	m_pFolderInfo->GetFileList( lstFiles, opt );
}

//void CZipArchive::GetSubFolders( AIRList<AIRString>& folders )
//{
//	m_pFolderInfo->GetSubFolderList( folders, true );
//}



//
//CZipArchive::CZipArchive(const AIRString& strName) : CBaseArchive(strName), m_ZzipDir(NULL)
//{
//    CStringUtil::ToLowerCase(m_strName);
//    Load();
//}
//
//CZipArchive::~CZipArchive(void)
//{
//    Unload();
//}
//
//void CZipArchive::CheckZzipError(int zzipError, const AIRString& operation) const
//{
//    if (zzipError != ZZIP_NO_ERROR)
//    {
//        AIRString errorMsg = getZzipErrorDescription(static_cast<zzip_error_t>(zzipError));
//    }
//}
//
//bool CZipArchive::Load()
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    if (!m_ZzipDir)
//    {
//        zzip_error_t zzipError;
//        m_ZzipDir = zzip_dir_open(m_strName.c_str(), &zzipError);
//        CheckZzipError(zzipError, "opening archive");
//
//        // Cache names
//        ZZIP_DIRENT zzipEntry;
//        while (zzip_dir_read(m_ZzipDir, &zzipEntry))
//        {
//            ZipFileInfo info;
//            //info.archive = this;
//            // Get basename / path
//            CStringUtil::SplitFilename(zzipEntry.d_name, info.path, info.basename);
//            info.filename = zzipEntry.d_name;
//            // Get sizes
//            info.compressedSize = static_cast<size_t>(zzipEntry.d_csize);
//            info.uncompressedSize = static_cast<size_t>(zzipEntry.st_size);
//            // folder entries
//            if (info.basename.empty())
//            {
//                info.filename = info.filename.substr (0, info.filename.length () - 1);
//                CStringUtil::SplitFilename(info.filename, info.path, info.basename);
//                // Set compressed size to -1 for folders; anyway nobody will check
//                // the compressed size of a folder, and if he does, its useless anyway
//                info.compressedSize = size_t (-1);
//            }
//
//            m_vtFileInfo.push_back(info);
//
//        }
//
//    }
//    return true;
//}
//
//void CZipArchive::Unload()
//{
//    if (m_ZzipDir)
//    {
//        zzip_dir_close(m_ZzipDir);
//        m_ZzipDir = 0;
//        m_vtFileInfo.clear();
//    }
//
//}
//
//const AIRString& CZipArchive::_GetFileFullPath(const AIRString& strFilename) const
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    for (FileInfo_Const_Iter it = m_vtFileInfo.begin(); it != m_vtFileInfo.end(); it++)
//    {
//        //ZipFileInfo& info = *it;
//        if ((*it).basename == strFilename)
//        {
//            return (*it).filename;
//        }
//    }
//
//    return "";
//}
//
//AIRString CZipArchive::GetFileFullPath(const char* szFilename)
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    for (FileInfo_Const_Iter it = m_vtFileInfo.begin(); it != m_vtFileInfo.end(); it++)
//    {
//        //ZipFileInfo& info = *it;
//        if ((*it).basename == AIRString(szFilename))
//        {
//            return (*it).filename;
//        }
//    }
//
//    return "";
//}
//
////-----------------------------------------------------------------------
//CDataStream* CZipArchive::Open(const AIRString& filename) const
//{
//
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//    AIRString fileFullPath = _GetFileFullPath(filename);
//    // Format not used here (always binary)
//    ZZIP_FILE* zzipFile = 
//        zzip_file_open(m_ZzipDir, fileFullPath.c_str(), ZZIP_ONLYZIP | ZZIP_CASELESS);
//    if (!zzipFile)
//    {
//        int zerr = zzip_error(m_ZzipDir);
//        AIRString zzDesc = getZzipErrorDescription((zzip_error_t)zerr);
//        //LogManager::getSingleton().logMessage(
//        //    mName + " - Unable to open file " + filename + ", error was '" + zzDesc + "'");
//
//        // return null pointer
//        return NULL ;//DataStreamPtr();
//    }
//
//    // Get uncompressed size too
//    ZZIP_STAT zstat;
//    zzip_dir_stat(m_ZzipDir, fileFullPath.c_str(), &zstat, ZZIP_CASEINSENSITIVE);
//
//    // Construct & return stream
//    return new CZipDataStream(filename, zzipFile, static_cast<size_t>(zstat.st_size)); //DataStreamPtr(OGRE_NEW ZipDataStream(filename, zzipFile, static_cast<size_t>(zstat.st_size)));
//
//}
//
//bool CZipArchive::Exist(const AIRString& filename, AIRString& fullPath) const
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    ZZIP_STAT zstat;
//    int res = zzip_dir_stat(m_ZzipDir, filename.c_str(), &zstat, ZZIP_CASEINSENSITIVE);
//
//    return (res == ZZIP_NO_ERROR);
//}
//
//void CZipArchive::GetFileList(AIRList<AIRString>& lstFiles)
//{
//	std::locale loc1 = std::locale::global(std::locale(".936"));
//
//    zzip_error_t zzipError;
//    m_ZzipDir = zzip_dir_open(m_strName.c_str(), &zzipError);
//    CheckZzipError(zzipError, "opening archive");
//
//    // Cache names
//    ZZIP_DIRENT zzipEntry;
//    while (zzip_dir_read(m_ZzipDir, &zzipEntry))
//    {
//        ZipFileInfo info;
//        //info.archive = this;
//        // Get basename / path
//        CStringUtil::SplitFilename(zzipEntry.d_name, info.path, info.basename);
//        info.filename = zzipEntry.d_name;
//        // Get sizes
//        info.compressedSize = static_cast<size_t>(zzipEntry.d_csize);
//        info.uncompressedSize = static_cast<size_t>(zzipEntry.st_size);
//        // folder entries
//        if (info.basename.empty())
//        {
//            info.filename = info.filename.substr (0, info.filename.length () - 1);
//            CStringUtil::SplitFilename(info.filename, info.path, info.basename);
//            // Set compressed size to -1 for folders; anyway nobody will check
//            // the compressed size of a folder, and if he does, its useless anyway
//            info.compressedSize = size_t (-1);
//        }
//
//        //m_vtFileInfo.push_back(info);
//        lstFiles.push_back(info.filename);
//
//    }
//}
