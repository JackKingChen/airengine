/********************************************************************
	created:	2009/12/12
	created:	12:12:2009   16:57
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\MemoryDataStream.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	MemoryDataStream
	file ext:	h
	author:		liangairan
	
	purpose:	文件内存流
*********************************************************************/

#pragma once 

#include "DataStream.h"
#include "AIRTypes.h"
using namespace AIR;

class EngineExport CMemoryDataStream : public CDataStream
{
public:
    CMemoryDataStream(CDataStream& sourceStream, bool freeOnClose = true);
    
    virtual ~CMemoryDataStream();

    u8* GetPtr(void) { return m_pData; }

    /** Get a pointer to the current position in the memory block this stream holds. */
    u8* GetCurrentPtr(void) { return m_pPos; }

    /** @copydoc DataStream::read
    */
    size_t Read(void* buf, size_t count);
    /** @copydoc DataStream::readLine
    */
    size_t ReadLine(char* buf, size_t maxCount, const AIRString& delim = "\n");

    /** @copydoc DataStream::skipLine
    */
    size_t SkipLine(const AIRString& delim = "\n");

    /** @copydoc DataStream::skip
    */
    void Skip(long count);

    /** @copydoc DataStream::seek
    */
    void Seek( size_t pos, StreamSeekType type );

    /** @copydoc DataStream::tell
    */
    size_t Tell(void) const;

    /** @copydoc DataStream::eof
    */
    bool Eof(void) const;

    /** @copydoc DataStream::close
    */
    void Close(void);
protected:
    u8* m_pData;
    u8* m_pPos;
    u8* m_pEnd;

    bool m_bFreeOnClose;
private:
};

