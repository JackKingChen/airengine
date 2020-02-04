#pragma once 

#include "Common.h"
#include "MemoryAlloter.h"

enum IndexType 
{
	IT_16BIT,
	IT_32BIT
};

class EngineExport IIndexBuffer// : public EngineAllocator(IIndexBuffer)
{
	EngineAllocator(IIndexBuffer)
	friend class IRenderer;
public:
    IIndexBuffer() : m_dwSize(0), m_dwTotalIndex(0), m_bLock(false) {}
    virtual ~IIndexBuffer() {}

    //创建IndexBuffer
    //@nIndexCount  索引数量
    //@bDynamic     是否动态创建
    //@type         索引类型
    virtual bool	Create(/*UINT nLength*/u32 nIndexCount, bool bDynamic = false, IndexType type = IT_16BIT, bool bUseSystemMemory = false) = 0;

    virtual void*   Lock(u32 OffsetToLock, u32 SizeToLock, bool bDiscard = false) = 0; 
    virtual void	Unlock() = 0;

    virtual void*	GetInterface() = 0;

    virtual void	Release() = 0;

    void SetTotalIndex(u32 dwTotalIndex) 
    {
        m_dwTotalIndex = dwTotalIndex;
    }

    u32 GetTotalIndex() const
    {
        return m_dwTotalIndex;
    }

	virtual void	WriteData( size_t offset, size_t length, const void* pSource, bool discardWholeBuffer ) = 0;

	virtual void	ReadData( size_t offset, size_t length, void* pDest ) = 0;

	u32		GetWholeSize( void )
    {
        return m_dwSize;
    }

	IndexType GetIndexType() const { return m_IndexType; }

protected:
	//virtual void	ReleaseWhenResetDevice( void ) = 0;
	//virtual void	CreateWhenResetDevice( void ) = 0;
protected:

    u32 m_dwSize;			//字节数
    u32 m_dwTotalIndex;		//填充的索引总数
	bool m_bLock;
	IndexType m_IndexType;
private:
};
