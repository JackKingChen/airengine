//------------------------------------------------------------------------------
/**
    @class	FolderInfo

	@brief	virtual folder info of an archive

    @date	2012-10-13

	@author HJJ
*/
//------------------------------------------------------------------------------

#pragma once

#ifndef __FolderInfo_H__
#define __FolderInfo_H__

#include "EngineConfig.h"
#include "MemoryAlloter.h"
#include "Common.h"


enum eCollectOption
{
	eCO_FileName					= 1, //�ļ���
	eCO_FileFullName				= 2, //�ļ�ȫ·����
	eCO_CurFolder					= 4, //��ǰĿ¼
	eCO_CurFolderAndAllSubFolders	= 8, //��ǰĿ¼�Լ����е���Ŀ¼

	eCO_CurFolderFiles				= eCO_FileName | eCO_CurFolder,
	eCO_CurFolderFullFiles			= eCO_FileFullName | eCO_CurFolder,
	eCO_AllFolderFiles				= eCO_FileName | eCO_CurFolderAndAllSubFolders,
	eCO_AllFolderFullFiles			= eCO_FileFullName | eCO_CurFolderAndAllSubFolders
};

//�ļ���Ϣ�ṹ��
typedef struct tagFileInfo 
{
	AIRString	strFileName;		//�ļ���
	AIRString	strFullName;		//�ļ�·��+�ļ���
	AIRString	strPath;			//�ļ�·��
	u32			uncompressedSize;   //�ļ���С
	u32			compressedSize;		//ѹ����Ĵ�С(��Դ������ļ�)
}
AIRFILEINFO, *LPAIRFILEINFO;

class FolderInfo;

typedef FolderInfo*						LPFOLDERINFO;
typedef AIRMap<AIRString, AIRFILEINFO*>	AIR_FILE_MAP;
typedef AIRMap<AIRString, FolderInfo*>	AIR_FOLDER_MAP;
typedef AIRList<AIRString>				AIR_STRING_LIST;

//�ļ�����Ϣ
//FileSystem�ڶ�ȡArchive��ʱ��,�����ɸ�Archive��Ӧ��FolderInfo
class EngineExport FolderInfo// : public EngineAllocator(FolderInfo)
{
	EngineAllocator(FolderInfo)
public:
	FolderInfo();

	~FolderInfo();

	//�����ļ���ȡ���ļ���Ϣ
	LPAIRFILEINFO FindFileInfo( const AIRString& fileName );

	//��ȡ·����ΪszFolderPath���ļ�����Ϣ
	FolderInfo* FindFolderInfo( const AIRString& szFolderPath );

	//��ȡ�ļ��б�
	void GetFileList( AIR_STRING_LIST& lst, eCollectOption opt = eCO_AllFolderFiles );

	//��ȡ��Ŀ¼�б�
	//bCollectSubFoldersChildren : �Ƿ������ǰ�ļ��е�Ŀ¼���Ի�ȡ�������ļ���
	void GetSubFolderList( AIR_STRING_LIST& lst, bool bCollectSubFoldersChildren = true );

	//��������ļ�����Ϣ
	void DebugFolderInfo();

	//�����ļ�����Ϣ��
	//pRootFolder : ���ļ�����Ϣ��
	//szFolderPath : ��Ҫ�������ļ���·��,������ڸ��ļ��е�·��
	static FolderInfo* CreateFolderInfo( FolderInfo* pRootFolder, const AIRString& szFolderPath );

	//�����ļ��е��ļ���Ϣ��
	//pRootFolder : ���ļ�����Ϣ��
	//szFilePath : �ļ�����·��,������ڸ��ļ��е�·��
	//szFileName : �ļ�����
	//uncompressedSize : �ļ���С
	//compressedSize : ѹ������ļ���С
	static bool CreateFileFolderInfo( 
		FolderInfo* pRootFolder, 
		const AIRString& szFilePath, 
		const AIRString& szFileName, 
		u32 uncompressedSize, 
		u32 compressedSize );

public:
	AIRString				m_szFolderName;			//�ļ�������
	AIRString				m_szFolderFullPath;		//�ļ���·��
	AIRString               m_strAbsoluteFullPath;  //ȫ·���������̷�
	AIR_FILE_MAP			m_FileMap;				//���ļ����µ��ļ���Ϣ�б�
	AIR_FOLDER_MAP			m_SubFolderMap;			//���ļ�����Ϣ��
	FolderInfo*				m_pParentFolder;		//���ļ���
};

#endif //__FolderInfo_H__