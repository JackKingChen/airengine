#include "stdafx.h"
#include "FileDataStream.h"
#include "Util.h"
#include "DebugTools.h"

//int g_fileDataCount = 0;

CFileDataStream::CFileDataStream(const AIRString& strName, std::ifstream* stream, bool bFreeOnClose /* = true */) : CDataStream(strName),
m_pStream(stream), m_bFreeOnClose(bFreeOnClose)
{
    m_pStream->seekg(0, std::ios_base::end);
    m_size = m_pStream->tellg();
    m_pStream->seekg(0, std::ios_base::beg);

    //g_fileDataCount++;

    //OutputDebugString(GetString("new CFileDataStream name is:[%s]\n", m_strName.c_str()));
}

CFileDataStream::~CFileDataStream()
{
    //g_fileDataCount--;
    //OutputDebugString(GetString("now the filedatastream count is %d, name is %s\n", g_fileDataCount, m_strName.c_str()));
    Close();
    //DT_TO_DBGSTR(LT_NONE, GetString("Stream [%s] delete!\n", m_strName.c_str()));
}

size_t CFileDataStream::Read(void* buf, size_t count)
{
    m_pStream->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));
    return m_pStream->gcount();
}

size_t CFileDataStream::ReadLine(char* buf, size_t maxCount, 
                                      const AIRString& delim)
{
    if (delim.empty())
    {
        //OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "No delimiter provided",
        //    "FileStreamDataStream::readLine");
    }
    if (delim.size() > 1)
    {
        //LogManager::getSingleton().logMessage(
        //    "WARNING: FileStreamDataStream::readLine - using only first delimeter");
    }
    // Deal with both Unix & Windows LFs
    bool trimCR = false;
    if (delim.at(0) == '\n') 
    {
        trimCR = true;
    }
    // maxCount + 1 since count excludes terminator in getline
    m_pStream->getline(buf, static_cast<std::streamsize>(maxCount+1), delim.at(0));
    size_t ret = m_pStream->gcount();
    // three options
    // 1) we had an eof before we read a whole line
    // 2) we ran out of buffer space
    // 3) we read a whole line - in this case the delim character is taken from the stream but not written in the buffer so the read data is of length ret-1 and thus ends at index ret-2
    // in all cases the buffer will be null terminated for us

    if (m_pStream->eof()) 
    {
        // no problem
    }
    else if (m_pStream->fail())
    {
        // Did we fail because of maxCount hit? No - no terminating character
        // in included in the count in this case
        if (ret == maxCount)
        {
            // clear failbit for next time 
            m_pStream->clear();
        }
        else
        {
            //OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
            //    "Streaming error occurred", 
            //    "FileStreamDataStream::readLine");
        }
    }
    else 
    {
        // we need to adjust ret because we want to use it as a
        // pointer to the terminating null character and it is
        // currently the length of the data read from the stream
        // i.e. 1 more than the length of the data in the buffer and
        // hence 1 more than the _index_ of the NULL character
        --ret;
    }

    // trim off CR if we found CR/LF
    if (trimCR && buf[ret-1] == '\r')
    {
        --ret;
        buf[ret] = '\0';
    }
    return ret;
}
//-----------------------------------------------------------------------
void CFileDataStream::Skip(long count)
{
#if defined(STLPORT)
    // Workaround for STLport issues: After reached eof of file stream,
    // it's seems the stream was putted in intermediate state, and will be
    // fail if try to repositioning relative to current position.
    // Note: tellg() fail in this case too.
    if (m_pStream->eof())
    {
        m_pStream->clear();
        // Use seek relative to either begin or end to bring the stream
        // back to normal state.
        m_pStream->seekg(0, std::ios::end);
    }
#endif 		
    m_pStream->clear(); //Clear fail status in case eof was set
    m_pStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
}
//-----------------------------------------------------------------------
void CFileDataStream::Seek( size_t pos, StreamSeekType type )
{
    m_pStream->clear(); //Clear fail status in case eof was set
    switch (type)
    {
    case CDataStream::AIR_SEEK_SET:
        m_pStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
        break;
    case CDataStream::AIR_SEEK_CUR:
        m_pStream->seekg(static_cast<std::streamoff>(pos), std::ios::cur);
        break;
    case CDataStream::AIR_SEEK_END:
        m_pStream->seekg(static_cast<std::streamoff>(pos), std::ios::end);
        break;
    }
    
}
//-----------------------------------------------------------------------
size_t CFileDataStream::Tell(void) const
{
    m_pStream->clear(); //Clear fail status in case eof was set
    return m_pStream->tellg();
}
//-----------------------------------------------------------------------
bool CFileDataStream::Eof(void) const
{
    return m_pStream->eof();
}
//-----------------------------------------------------------------------
void CFileDataStream::Close(void)
{
    if (m_pStream)
    {
        m_pStream->close();
        if (m_bFreeOnClose)
        {
            //SafeDelete(m_pStream);
            // delete the stream too
            //OGRE_DELETE_T(m_pStream, basic_ifstream, MEMCATEGORY_GENERAL);
            AIR_DELETE_T(m_pStream, std::ifstream);
            m_pStream = 0;
        }
    }
}

