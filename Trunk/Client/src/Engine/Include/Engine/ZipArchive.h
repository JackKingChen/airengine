/********************************************************************
	created:	2009/12/11
	created:	11:12:2009   11:33
	filename: 	d:\My Codes\XMLTest\XMLTest\ZipArchive.h
	file path:	d:\My Codes\XMLTest\XMLTest
	file base:	ZipArchive
	file ext:	h
	author:		liangairan
	
	purpose:	����һ��zip�ļ���
*********************************************************************/
#pragma once
#include <string>
#include <vector>
#include "BaseArchive.h"

typedef struct zzip_dir		ZZIP_DIR;
typedef struct zzip_file	ZZIP_FILE;

//typedef struct tagZipFileInfo 
//{
//    AIRString filename;
//    /// Path name; separated by '/' and ending with '/'
//    AIRString path;
//    /// Base filename
//    AIRString basename;
//    /// Compressed size
//    size_t compressedSize;
//    /// Uncompressed size
//    size_t uncompressedSize;
//}ZipFileInfo;


class CZipArchive : public CBaseArchive
{
public:
	CZipArchive(const AIRString& strName);
	virtual ~CZipArchive(void);

	//���ļ�
	//@filename �ļ�·����
	virtual CDataStream* Open(const AIRString& filename);

	//�ж��ļ��Ƿ����
	virtual bool Exist(const AIRString& filename, AIRString& fullPath) const;

	virtual bool Exist(const AIRString& strPath, const AIRString& filename, AIRString& fullPath) const;

	//���ĳһ�ļ��ľ���·��
	virtual AIRString GetFileFullPath(const char* szFilename);

	//�������
	virtual void Clear();

	//��������ļ���
	virtual void GetFileList( AIRList<AIRString>& lstFiles, eCollectOption opt = eCO_AllFolderFiles );

	//�����Ŀ¼�б�
	//virtual void GetSubFolders( AIRList<AIRString>& folders );

private:
	bool Load();

	void CheckZzipError(int zzipError, const AIRString& operation) const;

private:
	ZZIP_DIR* m_ZzipDir;
};


//
//class CZipArchive : public CBaseArchive
//{
//public:
//    CZipArchive(const AIRString& strName);
//    virtual ~CZipArchive(void);
//
//    bool Load();
//
//    void Clear();
//
//    /// @copydoc Archive::open
//    virtual CDataStream* Open(const AIRString& filename);
//
//    /// @copydoc Archive::list
//    //StringVectorPtr List(bool recursive = true, bool dirs = false);
//
//    /// @copydoc Archive::listFileInfo
//    //FileInfoListPtr ListFileInfo(bool recursive = true, bool dirs = false);
//
//    /// @copydoc Archive::find
//    //StringVectorPtr Find(const AIRString& pattern, bool recursive = true,
//    //    bool dirs = false);
//
//    /// @copydoc Archive::findFileInfo
//    //FileInfoListPtr FindFileInfo(const AIRString& pattern, bool recursive = true,
//    //    bool dirs = false);
//
//    /// @copydoc Archive::exists
//    bool Exist(const AIRString& filename, AIRString& fullPath) const;
//
//    /// @copydoc Archive::getModifiedTime
//    time_t GetModifiedTime(const AIRString& filename);
//
//    virtual AIRString GetFileFullPath(const char* szFilename);
//
//    virtual void GetFileList(AIRList<AIRString>& lstFiles);
//
//	virtual void GetSubFolders( AIRList<AIRString>& folders ) {}
//
//private:
//    void CheckZzipError(int zzipError, const AIRString& operation) const;
//
//    const AIRString& _GetFileFullPath(const AIRString& strFilename) const;
//private:
//
//    typedef AIRVector<ZipFileInfo> FileInfoList;
//    typedef FileInfoList::iterator   FileInfo_Iter;
//    typedef FileInfoList::const_iterator   FileInfo_Const_Iter;
//    FileInfoList  m_vtFileInfo;
//
//    ZZIP_DIR* m_ZzipDir;
//};
