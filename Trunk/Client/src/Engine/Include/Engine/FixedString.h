#pragma once

#include "EngineConfig.h"
#include "MemoryAllocatorConfig.h"

#include "AIRTypes.h"

using namespace AIR;

typedef u32 FIXED_STRING_HANDLE;

class EngineExport FixedString// : public EngineAllocator(FixedString)
{
	EngineAllocator(FixedString)
public:
    static int SysInit();
    static int SysRelease();

    static FIXED_STRING_HANDLE Insert(const char *pstr, int len=-1);
    static void AddRef(FIXED_STRING_HANDLE h);
    static void Release(FIXED_STRING_HANDLE h);

    static const char *Handle2Ptr(FIXED_STRING_HANDLE h)
    {
        return (const char *)h;
    }

    static const char *Handle2PtrSafe(FIXED_STRING_HANDLE h)
    {
        return (h == NULL) ? "" : (const char *)h;
    }
public:
    FixedString() : m_StrHandle(NULL){}
    FixedString(FIXED_STRING_HANDLE h) : m_StrHandle(h)
    {
        AddRef(h);
    }

    FixedString(const FixedString &rhs) : m_StrHandle(rhs.m_StrHandle)
    {
        AddRef(m_StrHandle);
    }

    FixedString(const char *pstr)
    {
        m_StrHandle = Insert(pstr); 
    }

    FixedString(const char *pstr, size_t len)
    {
        m_StrHandle = Insert(pstr, (int)len); 
    }

    ~FixedString()
    {
        Release(m_StrHandle); 
    }

    const char* AsCharPtr() const
    {
        return Handle2Ptr(m_StrHandle);
    }

    FIXED_STRING_HANDLE GetHandle() const
    {
        return m_StrHandle;
    }

    FixedString &operator=( const FixedString &str );
    FixedString &operator=( const char *pstr );
    size_t GetLength() const;
    bool Empty() const {
        return *AsCharPtr() == '\0';
    }

    bool operator==( const FixedString &s2 ) const;
    bool operator!=( const FixedString &s2 ) const;
    bool operator<(const FixedString& rhs) const;

protected:
    union
    {
        FIXED_STRING_HANDLE m_StrHandle;
        char *m_pStr;
    };
};

inline bool FixedString::operator==( const FixedString &s2 ) const
{
    return m_StrHandle==s2.m_StrHandle;
}

inline bool FixedString::operator!=(  const FixedString &s2 ) const
{
    return m_StrHandle!=s2.m_StrHandle;
}

//------------------------------------------------------------------------------
inline bool
FixedString::operator<(const FixedString& rhs) const
{
    return m_StrHandle < rhs.m_StrHandle;
}
