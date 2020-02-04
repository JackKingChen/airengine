/********************************************************************
	created:	2009/12/11
	created:	11:12:2009   14:26
	filename: 	d:\My Codes\XMLTest\XMLTest\DataStream.h
	file path:	d:\My Codes\XMLTest\XMLTest
	file base:	DataStream
	file ext:	h
	author:		liangairan
	
	purpose:	代表文件数据流
*********************************************************************/
#pragma once

#include <string>
#include <istream>
#include "EngineConfig.h"
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"

class EngineExport CDataStream// : public EngineAllocator(CDataStream)
{
	EngineAllocator(CDataStream)
public:
    enum StreamSeekType
    {
        AIR_SEEK_SET,
        AIR_SEEK_CUR,
        AIR_SEEK_END,
    };
    #define STREAM_TEMP_SIZE 128
    CDataStream(void);
    CDataStream(const AIRString& name);
    virtual ~CDataStream(void);

    const AIRString& GetName(void) { return m_strName; }

    // Streaming operators
    template<typename T> CDataStream& operator>>(T& val);

    /** Read the requisite number of bytes from the stream, 
    stopping at the end of the file.
    @param buf Reference to a buffer pointer
    @param count Number of bytes to read
    @returns The number of bytes read
    */
    virtual size_t Read(void* buf, size_t count) = 0;
    /** Get a single line from the stream.
    @remarks
    The delimiter character is not included in the data
    returned, and it is skipped over so the next read will occur
    after it. The buffer contents will include a
    terminating character.
    @note
    If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
    otherwise, it'll produce unexpected results.
    @param buf Reference to a buffer pointer
    @param maxCount The maximum length of data to be read, excluding the terminating character
    @param delim The delimiter to stop at
    @returns The number of bytes read, excluding the terminating character
    */
    virtual size_t ReadLine(char* buf, size_t maxCount, const AIRString& delim = "\n");

    /** Returns a String containing the next line of data, optionally 
    trimmed for whitespace. 
    @remarks
    This is a convenience method for text streams only, allowing you to 
    retrieve a String object containing the next line of data. The data
    is read up to the next newline character and the result trimmed if
    required.
    @note
    If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
    otherwise, it'll produce unexpected results.
    @param 
    trimAfter If true, the line is trimmed for whitespace (as in 
    String.trim(true,true))
    */
    virtual AIRString GetLine( bool trimAfter = true );

    /** Returns a String containing the entire stream. 
    @remarks
    This is a convenience method for text streams only, allowing you to 
    retrieve a String object containing all the data in the stream.
    */
    virtual AIRString GetAsString(void);

    /** Skip a single line from the stream.
    @note
    If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
    otherwise, it'll produce unexpected results.
    @param delim The delimiter(s) to stop at
    @returns The number of bytes skipped
    */
    virtual size_t SkipLine(const AIRString& delim = "\n");

    /** Skip a defined number of bytes. This can also be a negative value, in which case
    the file pointer rewinds a defined number of bytes. */
    virtual void Skip(long count) = 0;

    /** Repositions the read point to a specified byte.
    */
    virtual void Seek( size_t pos, StreamSeekType type ) = 0;

    /** Returns the current byte offset from beginning */
    virtual size_t Tell(void) const = 0;

    /** Returns true if the stream has reached the end.
    */
    virtual bool Eof(void) const = 0;

    /** Returns the total size of the data to be read from the stream, 
    or 0 if this is indeterminate for this stream. 
    */
    size_t Size(void) const { return m_size; }

    /** Close the stream; this makes further operations invalid. */
    virtual void Close(void) = 0;

	////************************************
	//// Method:    GetFileData
	//// Returns:   bool
	//// Qualifier: 获得文件的数据,用户使用完获得的数据后要自己销毁pDataPtr
	//// Parameter: void * pDataPtr 数据指针
	//// Parameter: u32 & dwDataSize 数据大小
	////************************************
	//bool GetFileData( void** pDataPtr, u32& dwDataSize );

protected:
    AIRString m_strName;
    //CBaseArchive* m_pArchive;  //路径
    size_t m_size;
};
