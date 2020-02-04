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
	eCO_FileName					= 1, //文件名
	eCO_FileFullName				= 2, //文件全路径名
	eCO_CurFolder					= 4, //当前目录
	eCO_CurFolderAndAllSubFolders	= 8, //当前目录以及所有的子目录

	eCO_CurFolderFiles				= eCO_FileName | eCO_CurFolder,
	eCO_CurFolderFullFiles			= eCO_FileFullName | eCO_CurFolder,
	eCO_AllFolderFiles				= eCO_FileName | eCO_CurFolderAndAllSubFolders,
	eCO_AllFolderFullFiles			= eCO_FileFullName | eCO_CurFolderAndAllSubFolders
};

//文件信息结构体
typedef struct tagFileInfo 
{
	AIRString	strFileName;		//文件名
	AIRString	strFullName;		//文件路径+文件名
	AIRString	strPath;			//文件路径
	u32			uncompressedSize;   //文件大小
	u32			compressedSize;		//压缩后的大小(针对打包后的文件)
}
AIRFILEINFO, *LPAIRFILEINFO;

class FolderInfo;

typedef FolderInfo*						LPFOLDERINFO;
typedef AIRMap<AIRString, AIRFILEINFO*>	AIR_FILE_MAP;
typedef AIRMap<AIRString, FolderInfo*>	AIR_FOLDER_MAP;
typedef AIRList<AIRString>				AIR_STRING_LIST;

//文件夹信息
//FileSystem在读取Archive的时候,会生成该Archive对应的FolderInfo
class EngineExport FolderInfo// : public EngineAllocator(FolderInfo)
{
	EngineAllocator(FolderInfo)
public:
	FolderInfo();

	~FolderInfo();

	//根据文件名取得文件信息
	LPAIRFILEINFO FindFileInfo( const AIRString& fileName );

	//获取路径名为szFolderPath的文件夹信息
	FolderInfo* FindFolderInfo( const AIRString& szFolderPath );

	//获取文件列表
	void GetFileList( AIR_STRING_LIST& lst, eCollectOption opt = eCO_AllFolderFiles );

	//获取子目录列表
	//bCollectSubFoldersChildren : 是否遍历当前文件夹的目录树以获取所有子文件夹
	void GetSubFolderList( AIR_STRING_LIST& lst, bool bCollectSubFoldersChildren = true );

	//调试输出文件夹信息
	void DebugFolderInfo();

	//创建文件夹信息体
	//pRootFolder : 根文件夹信息体
	//szFolderPath : 将要创建的文件夹路径,是相对于根文件夹的路径
	static FolderInfo* CreateFolderInfo( FolderInfo* pRootFolder, const AIRString& szFolderPath );

	//创建文件夹的文件信息体
	//pRootFolder : 根文件夹信息体
	//szFilePath : 文件所在路径,是相对于根文件夹的路径
	//szFileName : 文件名称
	//uncompressedSize : 文件大小
	//compressedSize : 压缩后的文件大小
	static bool CreateFileFolderInfo( 
		FolderInfo* pRootFolder, 
		const AIRString& szFilePath, 
		const AIRString& szFileName, 
		u32 uncompressedSize, 
		u32 compressedSize );

public:
	AIRString				m_szFolderName;			//文件夹名称
	AIRString				m_szFolderFullPath;		//文件夹路径
	AIRString               m_strAbsoluteFullPath;  //全路径，包括盘符
	AIR_FILE_MAP			m_FileMap;				//该文件夹下的文件信息列表
	AIR_FOLDER_MAP			m_SubFolderMap;			//子文件夹信息表
	FolderInfo*				m_pParentFolder;		//父文件夹
};

#endif //__FolderInfo_H__