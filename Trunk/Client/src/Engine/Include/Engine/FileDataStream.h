/********************************************************************
	created:	2009/12/12
	created:	12:12:2009   12:10
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\FileDataStream.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	FileDataStream
	file ext:	h
	author:		liangairan
	
	purpose:	普通文件流
*********************************************************************/

#pragma once

#include "DataStream.h"

class CFileDataStream : public CDataStream
{
public:
    CFileDataStream(const AIRString& strName, std::ifstream* stream, bool bFreeOnClose = true);
    virtual ~CFileDataStream();

    virtual size_t ReadLine(char* buf, size_t maxCount, const AIRString& delim = "\n");
    /// @copydoc DataStream::read
    size_t Read(void* buf, size_t count);
    /// @copydoc DataStream::skip
    void Skip(long count);
    /// @copydoc DataStream::seek
    void Seek( size_t pos, StreamSeekType type );
    /// @copydoc DataStream::seek
    size_t Tell(void) const;
    /// @copydoc DataStream::eof
    bool Eof(void) const;
    /// @copydoc DataStream::close
    void Close(void);
protected:

    std::ifstream* m_pStream;
    bool           m_bFreeOnClose;		
private:
};
