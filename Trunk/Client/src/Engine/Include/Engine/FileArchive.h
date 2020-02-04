/********************************************************************
	created:	2009/12/12
	created:	12:12:2009   10:53
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\FileArchive.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	FileArchive
	file ext:	h
	author:		liangairan
	
	purpose:	�ļ�
*********************************************************************/
#pragma once 
#include "BaseArchive.h"

class EngineExport CFileArchive : public CBaseArchive
{
public:
    CFileArchive(const AIRString& strName);
    virtual ~CFileArchive();

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

protected:
	void GenarateFolderInfo( FolderInfo* pCurFolder );

	bool IsAbsolutePath(const char* szFilename) const;
};

//class CFileArchive : public CBaseArchive
//{
//public:
//    typedef AIRVector<AIRString> VectorString;
//    typedef AIRVector<AIRFILEINFO> VectorFileInfo;
//    typedef VectorFileInfo::iterator FileInfo_Iter;
//
//
//    CFileArchive(const AIRString& strName);
//    virtual ~CFileArchive();
//
//    void Unload();
//
//    /// @copydoc Archive::open
//    virtual CDataStream* Open(const AIRString& filename) const;
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
//    virtual void Clear();
//
//    virtual void GetFileList(AIRList<AIRString>& lstFiles);
//
//	virtual void GetSubFolders( AIRVector<AIRString>& folders );
//
//protected:
//    //�ж��Ƿ��Ǿ���·��
//    bool IsAbsolutePath(const char* szFilename) const;
//
//    VectorFileInfo  m_vtFiles;   //first���ļ��� second���ļ��б�
//
//private:
//    void GetSubFolders(const AIRString& strFolder);
//    AIRVector<AIRString> m_vecSubFolders;
//
//    bool GetFullPath(AIRString& strFullPath, const AIRString& strFilename) const;
//};
