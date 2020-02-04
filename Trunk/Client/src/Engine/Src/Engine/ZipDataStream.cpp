#include "stdafx.h"
#include "ZipDataStream.h"

CZipDataStream::CZipDataStream(const AIRString& name, ZZIP_FILE* zzipFile, size_t uncompressedSize) : m_ZzipFile(zzipFile), CDataStream(name)
{
    m_size = uncompressedSize;
}

CZipDataStream::CZipDataStream(ZZIP_FILE* zzipFile, size_t uncompressedSize) : m_ZzipFile(zzipFile)
{
    m_size = uncompressedSize;
}

CZipDataStream::~CZipDataStream(void)
{
}

size_t CZipDataStream::Read(void* buf, size_t count)
{
    zzip_ssize_t r = zzip_file_read(m_ZzipFile, (char*)buf, count);
    if (r<0) 
    {
        ZZIP_DIR *dir = zzip_dirhandle(m_ZzipFile);
        //AIRString msg = zzip_strerror_of(dir);
        //OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR,
        //    m_strName+" - error from zziplib: "+msg,
        //    "CZipDataStream::read");
    }
    return (size_t) r;
}
//-----------------------------------------------------------------------
void CZipDataStream::Skip(long count)
{
    zzip_seek(m_ZzipFile, static_cast<zzip_off_t>(count), SEEK_CUR);
}
//-----------------------------------------------------------------------
void CZipDataStream::Seek( size_t pos, CDataStream::StreamSeekType type )
{
    switch (type)
    {
    case CDataStream::AIR_SEEK_SET:
        zzip_seek(m_ZzipFile, static_cast<zzip_off_t>(pos), SEEK_SET);
        break;
    case CDataStream::AIR_SEEK_CUR:
        zzip_seek(m_ZzipFile, static_cast<zzip_off_t>(pos), SEEK_CUR);
        break;
    case CDataStream::AIR_SEEK_END:
        zzip_seek(m_ZzipFile, static_cast<zzip_off_t>(pos), SEEK_END);
        break;
    }
    
}
//-----------------------------------------------------------------------
size_t CZipDataStream::Tell(void) const
{
    return zzip_tell(m_ZzipFile);
}
//-----------------------------------------------------------------------
bool CZipDataStream::Eof(void) const
{
    return (zzip_tell(m_ZzipFile) >= static_cast<zzip_off_t>(m_size));
}
//-----------------------------------------------------------------------
void CZipDataStream::Close(void)
{
    if (m_ZzipFile != 0)
    {
        zzip_file_close(m_ZzipFile);
        m_ZzipFile = 0;
    }
}
