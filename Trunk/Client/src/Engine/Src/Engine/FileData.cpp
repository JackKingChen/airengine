#include "stdafx.h"
#include "FileData.h"
#include "FileSystem.h"

IMPLEMENT_RTTI(FileData, Resource)

FileData::FileData()
{
    
}

FileData::~FileData()
{
    Clear();
}

int FileData::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }
    return GetRef();
}

void FileData::Clear()
{
    if (m_pStream)
    {
        SafeDelete(m_pStream);
    }
}

/*
bool FileData::Load()
{
    if (Resource::Load())
    {
        m_bReady = true;
    }
    return m_bLoaded;
}
*/

/*
bool FileData::LoadFromFile(const char* szFilename)
{
    m_pStream = FILESYSTEM->GetFileStream(szFilename);
    if (m_pFileStream == NULL)
    {
        return false;
    }
    m_strName = szFilename;
    return true;
}
*/