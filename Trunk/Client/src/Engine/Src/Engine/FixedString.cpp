#include "stdafx.h"
#include "FixedString.h"
#include "_Mutex.h"
#include <assert.h>

#pragma pack(push, 1)
struct stShareString
{
    stShareString *pnext;
    size_t refcount;
#ifdef _DEBUG
    unsigned int hash;
#endif
};
#pragma pack(pop)

const int STR_HASH_BUFLEN = 100000;
static stShareString *s_HashTable[STR_HASH_BUFLEN];
static _Mutex_Body_* s_pMutexHashTable = NULL;// 用于同步的互斥对象，保护s_HashTable访问

#define STRUCT2STR(pobj) ((char *)pobj + sizeof(stShareString))
#define STR2STRUCT(pstr) ((stShareString *)pstr - 1)

unsigned int StringHash(const char *pstr, size_t nchar)
{
    unsigned int hash = 0;
    const char *pcur = pstr;
    while((pcur-pstr) < int(nchar))
    {
        hash = (hash <<5) + hash + *pcur;
        pcur++;
    }

    return hash;
}

unsigned int StringHash( const char *pstr )
{
    unsigned int hash = 0;
    while(*pstr)
    {
        hash = (hash <<5) + hash + *pstr;
        pstr++;
    }

    return hash;
}


FIXED_STRING_HANDLE FixedString::Insert(const char *pstr, int len)
{
    if( pstr == NULL ) 
        return NULL;

    if(len < 0) len = (int)strlen(pstr);

    unsigned int i = StringHash(pstr, len) % STR_HASH_BUFLEN;

    // 必须先调用 FixedString::SysInit 才能使用
    assert(s_pMutexHashTable);

    // 修改 s_HashTable 前先加锁
    if( s_pMutexHashTable )
        _Mutex_Help _sc(s_pMutexHashTable);

    stShareString *pcur = s_HashTable[i];

    while(pcur != NULL)
    {
        if( len == strlen( STRUCT2STR(pcur) ) && strncmp(pstr, STRUCT2STR(pcur), len) == 0 )
        {
            pcur->refcount++;

            return (FIXED_STRING_HANDLE)(uintptr_t)STRUCT2STR(pcur);
        }

        pcur = pcur->pnext;
    }

    size_t alloc_len = len + 1 + sizeof(stShareString);
    alloc_len += sizeof(size_t) - (alloc_len%sizeof(size_t));

    pcur = (stShareString *)malloc(alloc_len);
    pcur->pnext = s_HashTable[i];
    s_HashTable[i] = pcur;

    pcur->refcount = 1;

#ifdef _DEBUG
    pcur->hash = StringHash(pstr, len);
#endif

    char *pdststr = STRUCT2STR(pcur);
    memcpy(pdststr, pstr, len);
    pdststr[len] = '\0';

    return (FIXED_STRING_HANDLE)(uintptr_t)pdststr;
}

void FixedString::AddRef(FIXED_STRING_HANDLE h)
{
    if(h == NULL) return;

    stShareString *pcur = STR2STRUCT(Handle2Ptr(h));
#ifdef _DEBUG
    assert(pcur->hash == StringHash(Handle2Ptr(h)));
#endif

    pcur->refcount++;
}

void FixedString::Release(FIXED_STRING_HANDLE h)
{
    const char *pstr = Handle2Ptr(h);
    
    if( pstr == NULL ) return;

    stShareString *pcur = STR2STRUCT(pstr);
#ifdef _DEBUG
    assert( pcur->hash == StringHash(pstr) );
#endif

    pcur->refcount--;
    assert(pcur->refcount >= 0);
    return;

    if( pcur->refcount > 0 ) return;

    unsigned int i = StringHash(pstr) % STR_HASH_BUFLEN;

    // 修改 s_HashTable 前先加锁
    _Mutex_Help _sc(s_pMutexHashTable);

    stShareString *phead = s_HashTable[i];
    if( phead == pcur )
    {
        s_HashTable[i] = pcur->pnext;
    }
    else
    {
        while(1)
        {
            assert( phead != NULL );
            if( phead->pnext == pcur )
            {
                phead->pnext = pcur->pnext;
                break;
            }
            phead = phead->pnext;
        }
        assert(0);
    }

    if (strcmp(pstr, "E_VB_NORMAL") == 0)
    {
        int a = 0;
    }

    free( pcur );
}

int FixedString::SysInit()
{
    for( int i=0; i<STR_HASH_BUFLEN; i++ )
    {
        s_HashTable[i] = NULL;
    }

    if( s_pMutexHashTable == NULL )
    {
        s_pMutexHashTable = new _Mutex_Body_;
    }

    return 0;
}

int FixedString::SysRelease()
{
    for( int i=0; i<STR_HASH_BUFLEN; i++ )
    {
        stShareString *pcur = s_HashTable[i];
        while(pcur != NULL)
        {
            stShareString *pnext = pcur->pnext;
            free(pcur);
            pcur = pnext;
        }

        /*
        assert( s_HashTable[i] == NULL );
        if(s_HashTable[i] != NULL)
        {
        const char *p = STRUCT2STR(s_HashTable[i]);
        p = p;
        }*/
    }

    delete s_pMutexHashTable;

    return 0;
}

FixedString &FixedString::operator=( const FixedString &str )
{
    AddRef(str.m_StrHandle);
    Release(m_StrHandle);

    m_StrHandle = str.m_StrHandle;
    return *this;
}

FixedString &FixedString::operator=( const char *pstr )
{
    Release(m_StrHandle);
    m_StrHandle = Insert(pstr);

    return *this;
}

size_t FixedString::GetLength() const
{
    if( m_StrHandle == NULL ) 
    {
        return 0;
    }
    else 
        return strlen(Handle2Ptr(m_StrHandle));
}


