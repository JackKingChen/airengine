/********************************************************************
	created:	2009/12/11
	created:	11:12:2009   18:28
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\Archive.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	Archive
	file ext:	h
	author:		liangairan
	
	purpose:	代表一组文件的集合，可以是一个文件夹或一个zip文件
*********************************************************************/


#pragma once 

#include "DataStream.h"
#include "_Mutex.h"
#include "FolderInfo.h"

class EngineExport CBaseArchive// : public EngineAllocator(CBaseArchive)
{
	EngineAllocator(CBaseArchive)
public:
	CBaseArchive(const AIRString& strName)
		: m_strName(strName) 
		, m_pFolderInfo(NULL)
	{		
	}

	virtual ~CBaseArchive() 
	{
	}

	//获得档案名称
	inline const AIRString& GetName() const
	{
		return m_strName;
	}

	//获得该档案的根文件夹信息体
	inline const FolderInfo* GetFolderInfo() const
	{
		return m_pFolderInfo;
	}

	//打开文件
	//@filename 文件路径名
	virtual CDataStream* Open(const AIRString& filename) = 0;

	//判断文件filename是否存在，如果存在，fullPath将返回文件的全路径
	virtual bool Exist(const AIRString& filename, AIRString& fullPath) const = 0;

	virtual bool Exist(const AIRString& strPath, const AIRString& filename, AIRString& fullPath) const = 0;

	//获得某一文件的绝对路径
	virtual AIRString GetFileFullPath(const char* szFilename) = 0;

	//清空数据
	virtual void Clear() = 0;

	//获得所有文件名
	virtual void GetFileList( AIRList<AIRString>& lstFiles, eCollectOption opt = eCO_AllFolderFiles ) = 0;

	//virtual void GetSubFolders( AIRVector<AIRString>& folders ) {};
protected:
	AIRString m_strName;	
	_Mutex_Body_ m_sect;    //线程锁，用于线程安全

	FolderInfo* m_pFolderInfo;
};

//
//class EngineExport CBaseArchive : public EngineAllocator
//{
//public:
//    CBaseArchive(const AIRString& strName) : m_strName(strName) {};
//    virtual ~CBaseArchive() {};
//
//	inline const AIRString& GetName() const
//	{
//		return m_strName;
//	}
//
//    //打开文件
//    //@filename 文件路径名
//    virtual CDataStream* Open(const AIRString& filename) const = 0;
//
//    //virtual bool Load() = 0;
//
//    virtual void Unload() = 0;
//
//    //判断文件是否存在
//    virtual bool Exist(const AIRString& filename, AIRString& fullPath) const = 0;
//
//    //获得某一文件的绝对路径
//    virtual AIRString GetFileFullPath(const char* szFilename) = 0;
//
//    //清空数据
//    virtual void Clear() = 0;
//
//    //获得所有文件名
//    virtual void GetFileList(AIRList<AIRString>& lstFiles) = 0;
//
//
//	virtual void GetSubFolders( AIRVector<AIRString>& folders ) = 0;
//
//protected:
//    AIRString m_strName;	
//    _Mutex_Body_ m_sect;    //线程锁，用于线程安全
//};
