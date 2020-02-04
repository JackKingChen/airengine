/********************************************************************
	created:	2010/11/11
	created:	11:11:2010   9:11
	filename: 	ParticleFileData
	author:		liangairan
	
	purpose:	XML文件数据
*********************************************************************/

#pragma once 

#include "FileData.h"
#include "MarkupSTL.h"

class EngineExport XMLFileData : public FileData
{
public:
    XMLFileData();
    virtual ~XMLFileData();

    virtual bool LoadFromFile(const char* szFilename);

    CMarkupSTL& GetXML()
    {
        return m_xml;
    }

    virtual void Clear() {}
protected:
    CMarkupSTL m_xml;
private:
};
