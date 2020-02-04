#include "stdafx.h"
#include "MemoryDataStream.h"
#include <assert.h>
#include "Util.h"
//#include "DetectMemLeak.h"

CMemoryDataStream::CMemoryDataStream(CDataStream& sourceStream, bool freeOnClose /* = true */)
{
    m_size = sourceStream.Size();
    m_pData = AIR_NEW_ARRAY_T(u8, m_size);//OGRE_ALLOC_T(uchar, m_size, MEMCATEGORY_GENERAL);
    m_pPos = m_pData;
    sourceStream.Seek(0, CDataStream::AIR_SEEK_SET);
    m_pEnd = m_pData + sourceStream.Read(m_pData, m_size);
    m_bFreeOnClose = freeOnClose;
    m_strName = sourceStream.GetName();
    assert(m_pEnd >= m_pPos);
}

CMemoryDataStream::~CMemoryDataStream()
{
    Close();
}

size_t CMemoryDataStream::Read(void* buf, size_t count)
{
    size_t cnt = count;
    // Read over end of memory?
    if (m_pPos + cnt > m_pEnd)
        cnt = m_pEnd - m_pPos;
    if (cnt == 0)
        return 0;

    assert (cnt<=count);

    memcpy(buf, m_pPos, cnt);
    m_pPos += cnt;
    return cnt;
}
//-----------------------------------------------------------------------
size_t CMemoryDataStream::ReadLine(char* buf, size_t maxCount, 
                                  const AIRString& delim)
{
    // Deal with both Unix & Windows LFs
    bool trimCR = false;
    if (delim.find_first_of('\n') != AIRString::npos)
    {
        trimCR = true;
    }

    size_t pos = 0;

    // Make sure pos can never go past the end of the data 
    while (pos < maxCount && m_pPos < m_pEnd)
    {
        if (delim.find(*m_pPos) != AIRString::npos)
        {
            // Trim off trailing CR if this was a CR/LF entry
            if (trimCR && pos && buf[pos-1] == '\r')
            {
                // terminate 1 character early
                --pos;
            }

            // Found terminator, skip and break out
            ++m_pPos;
            break;
        }

        buf[pos++] = *m_pPos++;
    }

    // terminate
    buf[pos] = '\0';

    return pos;
}
//-----------------------------------------------------------------------
size_t CMemoryDataStream::SkipLine(const AIRString& delim)
{
    size_t pos = 0;

    // Make sure pos can never go past the end of the data 
    while (m_pPos < m_pEnd)
    {
        ++pos;
        if (delim.find(*m_pPos++) != AIRString::npos)
        {
            // Found terminator, break out
            break;
        }
    }

    return pos;

}
//-----------------------------------------------------------------------
void CMemoryDataStream::Skip(long count)
{
    size_t newpos = (size_t)( ( m_pPos - m_pData ) + count );
    assert( m_pData + newpos <= m_pEnd );        

    m_pPos = m_pData + newpos;
}
//-----------------------------------------------------------------------
void CMemoryDataStream::Seek( size_t pos, StreamSeekType type )
{
    assert( m_pData + pos <= m_pEnd );
    switch (type)
    {
    case CDataStream::AIR_SEEK_SET:
        m_pPos = m_pData + pos;
        break;
    case CDataStream::AIR_SEEK_CUR:
        m_pPos = m_pPos + pos;
        break;
    case CDataStream::AIR_SEEK_END:
        m_pPos = m_pData + m_size - pos;
        break;
    }
    
}
//-----------------------------------------------------------------------
size_t CMemoryDataStream::Tell(void) const
{
    //m_pData is start, m_pPos is current location
    return m_pPos - m_pData;
}
//-----------------------------------------------------------------------
bool CMemoryDataStream::Eof(void) const
{
    return m_pPos >= m_pEnd;
}
//-----------------------------------------------------------------------
void CMemoryDataStream::Close(void)    
{
    if (m_pData)
    {
        //SafeDeleteArray(m_pData);
        AIR_DELETE_ARRAY_T(m_pData, u8, m_size);
        m_pData = 0;
    }

}

