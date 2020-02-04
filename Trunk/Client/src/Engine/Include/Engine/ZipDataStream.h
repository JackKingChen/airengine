/********************************************************************
	created:	2009/12/11
	created:	11:12:2009   14:56
	filename: 	ZipDataStream.h
	author:		liangairan
	
	purpose:	ZIP的文件流类
*********************************************************************/
#pragma once
#include "datastream.h"
#include "zlib/zzip.h"

class CZipDataStream :
    public CDataStream
{
public:
    CZipDataStream(ZZIP_FILE* zzipFile, size_t uncompressedSize);
    CZipDataStream(const AIRString& name, ZZIP_FILE* zzipFile, size_t uncompressedSize);
    virtual ~CZipDataStream(void);

    /// @copydoc DataStream::read
    size_t Read(void* buf, size_t count);
    /// @copydoc DataStream::skip
    void Skip(long count);
    /// @copydoc DataStream::seek
    void Seek( size_t pos, CDataStream::StreamSeekType type );
    /// @copydoc DataStream::seek
    size_t Tell(void) const;
    /// @copydoc DataStream::eof
    bool Eof(void) const;
    /// @copydoc DataStream::close
    void Close(void);

protected:
    ZZIP_FILE* m_ZzipFile;
};
