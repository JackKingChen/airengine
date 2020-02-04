#include "stdafx.h"
#include "XMLFileData.h"
#include "DataStream.h"
#include "FileSystem.h"

XMLFileData::XMLFileData()
{
    
}

XMLFileData::~XMLFileData()
{

}

bool XMLFileData::LoadFromFile(const char* szFilename)
{
    CDataStream* pStream = FILESYSTEM->GetFileStream(szFilename);
    if (pStream == NULL)
    {
        return false;
    }

    AIRString strDoc = pStream->GetAsString();

    //CMarkupSTL xml;
    m_xml.SetDoc(strDoc.c_str());

    SafeDelete(pStream);

    return true;
}
