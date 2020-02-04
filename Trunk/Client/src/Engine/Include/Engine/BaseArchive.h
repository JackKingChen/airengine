/********************************************************************
	created:	2009/12/11
	created:	11:12:2009   18:28
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\Archive.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	Archive
	file ext:	h
	author:		liangairan
	
	purpose:	����һ���ļ��ļ��ϣ�������һ���ļ��л�һ��zip�ļ�
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

	//��õ�������
	inline const AIRString& GetName() const
	{
		return m_strName;
	}

	//��øõ����ĸ��ļ�����Ϣ��
	inline const FolderInfo* GetFolderInfo() const
	{
		return m_pFolderInfo;
	}

	//���ļ�
	//@filename �ļ�·����
	virtual CDataStream* Open(const AIRString& filename) = 0;

	//�ж��ļ�filename�Ƿ���ڣ�������ڣ�fullPath�������ļ���ȫ·��
	virtual bool Exist(const AIRString& filename, AIRString& fullPath) const = 0;

	virtual bool Exist(const AIRString& strPath, const AIRString& filename, AIRString& fullPath) const = 0;

	//���ĳһ�ļ��ľ���·��
	virtual AIRString GetFileFullPath(const char* szFilename) = 0;

	//�������
	virtual void Clear() = 0;

	//��������ļ���
	virtual void GetFileList( AIRList<AIRString>& lstFiles, eCollectOption opt = eCO_AllFolderFiles ) = 0;

	//virtual void GetSubFolders( AIRVector<AIRString>& folders ) {};
protected:
	AIRString m_strName;	
	_Mutex_Body_ m_sect;    //�߳����������̰߳�ȫ

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
//    //���ļ�
//    //@filename �ļ�·����
//    virtual CDataStream* Open(const AIRString& filename) const = 0;
//
//    //virtual bool Load() = 0;
//
//    virtual void Unload() = 0;
//
//    //�ж��ļ��Ƿ����
//    virtual bool Exist(const AIRString& filename, AIRString& fullPath) const = 0;
//
//    //���ĳһ�ļ��ľ���·��
//    virtual AIRString GetFileFullPath(const char* szFilename) = 0;
//
//    //�������
//    virtual void Clear() = 0;
//
//    //��������ļ���
//    virtual void GetFileList(AIRList<AIRString>& lstFiles) = 0;
//
//
//	virtual void GetSubFolders( AIRVector<AIRString>& folders ) = 0;
//
//protected:
//    AIRString m_strName;	
//    _Mutex_Body_ m_sect;    //�߳����������̰߳�ȫ
//};
