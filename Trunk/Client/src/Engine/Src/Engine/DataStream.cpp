#include "stdafx.h"
#include "DataStream.h"
#include "StringUtil.h"
#include "BaseArchive.h"
#include "DebugTools.h"
#undef min

CDataStream::CDataStream(void) : m_size(0)
{
}

CDataStream::CDataStream(const AIRString& name) : m_strName(name)
{

}

CDataStream::~CDataStream(void)
{
    
}

template <typename T> CDataStream& CDataStream::operator >>(T& val)
{
    Read(static_cast<void*>(&val), sizeof(T));
    return *this;
}
//-----------------------------------------------------------------------
AIRString CDataStream::GetLine(bool trimAfter)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

    char tmpBuf[STREAM_TEMP_SIZE];
    AIRString retString;
    size_t readCount;
    // Keep looping while not hitting delimiter
    while ((readCount = Read(tmpBuf, STREAM_TEMP_SIZE-1)) != 0)
    {
        // Terminate string
        tmpBuf[readCount] = '\0';

        char* p = strchr(tmpBuf, '\n');
        if (p != 0)
        {
            // Reposition backwards
            Skip((long)(p + 1 - tmpBuf - readCount));
            *p = '\0';
        }

        retString += tmpBuf;

        if (p != 0)
        {
            // Trim off trailing CR if this was a CR/LF entry
            if (retString.length() && retString[retString.length()-1] == '\r')
            {
                retString.erase(retString.length()-1, 1);
            }

            // Found terminator, break out
            break;
        }
    }

    if (trimAfter)
    {
        CStringUtil::Trim(retString);
    }

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
    return retString;
}
//-----------------------------------------------------------------------
size_t CDataStream::ReadLine(char* buf, size_t maxCount, const AIRString& delim)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

    // Deal with both Unix & Windows LFs
    bool trimCR = false;
    if (delim.find_first_of('\n') != AIRString::npos)
    {
        trimCR = true;
    }

    char tmpBuf[STREAM_TEMP_SIZE];
    size_t chunkSize = std::min(maxCount, (size_t)STREAM_TEMP_SIZE-1);
    size_t totalCount = 0;
    size_t readCount; 
    while (chunkSize && (readCount = Read(tmpBuf, chunkSize)))
    {
        // Terminate
        tmpBuf[readCount] = '\0';

        // Find first delimiter
        size_t pos = strcspn(tmpBuf, delim.c_str());

        if (pos < readCount)
        {
            // Found terminator, reposition backwards
            Skip((long)(pos + 1 - readCount));
        }

        // Are we genuinely copying?
        if (buf)
        {
            memcpy(buf+totalCount, tmpBuf, pos);
        }
        totalCount += pos;

        if (pos < readCount)
        {
            // Trim off trailing CR if this was a CR/LF entry
            if (trimCR && totalCount && buf[totalCount-1] == '\r')
            {
                --totalCount;
            }

            // Found terminator, break out
            break;
        }

        // Adjust chunkSize for next time
        chunkSize = std::min(maxCount-totalCount, (size_t)STREAM_TEMP_SIZE-1);
    }

    // Terminate
    buf[totalCount] = '\0';
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
    return totalCount;
}
//-----------------------------------------------------------------------
size_t CDataStream::SkipLine(const AIRString& delim)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

    char tmpBuf[STREAM_TEMP_SIZE];
    size_t total = 0;
    size_t readCount;
    // Keep looping while not hitting delimiter
    while ((readCount = Read(tmpBuf, STREAM_TEMP_SIZE-1)) != 0)
    {
        // Terminate string
        tmpBuf[readCount] = '\0';

        // Find first delimiter
        size_t pos = strcspn(tmpBuf, delim.c_str());

        if (pos < readCount)
        {
            // Found terminator, reposition backwards
            Skip((long)(pos + 1 - readCount));

            total += pos + 1;

            // break out
            break;
        }

        total += readCount;
    }
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
    return total;
}
//-----------------------------------------------------------------------
AIRString CDataStream::GetAsString(void)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

    // Read the entire buffer
    char* pBuf = AIR_NEW_ARRAY_T(char, m_size + 1);//OGRE_ALLOC_T(char, m_size+1, MEMCATEGORY_GENERAL);
    // Ensure read from begin of stream
    Seek(0, AIR_SEEK_SET);
    Read(pBuf, m_size);
    pBuf[m_size] = '\0';
    AIRString str;
    str.insert(0, pBuf, m_size);
    //delete [] pBuf;
	//SafeDeleteArray( pBuf );
    AIR_DELETE_ARRAY_T(pBuf, char, m_size + 1);
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global( std::locale(loc1) );
#endif
    return str;
}


//bool CDataStream::GetFileData( void** pDataPtr, u32& dwDataSize )
//{
//	dwDataSize = m_size;
//	(*pDataPtr) = AURO_NEW_VERBOSE char[m_size];
//	if ( !(*pDataPtr) )
//	{
//		return false;
//	}
//	Seek(0);
//	Read((*pDataPtr), m_size);
//	return true;
//}