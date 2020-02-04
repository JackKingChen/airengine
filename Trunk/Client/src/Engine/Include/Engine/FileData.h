/********************************************************************
	created:	2010/11/10
	created:	10:11:2010   18:21
	filename: 	FileData
	author:		liangairan
	
	purpose:	����ļ����ݵ�����
*********************************************************************/

#pragma once

#include "EngineResource.h"
#include "DataStream.h"

class EngineExport FileData : public Resource
{
	DECLARE_RTTI;
public:
    FileData();
    virtual ~FileData();

    virtual s32 Release();

    virtual void Clear();

    //virtual bool LoadFromFile(const char* szFilename);

    CDataStream* GetFileStream()
    {
        return m_pStream;
    }

    virtual bool Cache()
    {
        m_bReady = true;
        return true;
    }

    //virtual bool Load();
protected:
    //CDataStream* m_pFileStream;
private:
};
