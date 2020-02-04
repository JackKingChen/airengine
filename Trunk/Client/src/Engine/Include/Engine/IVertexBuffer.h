#pragma once 

#include "Common.h"
#include "EngineConfig.h"
#include "VertexDeclaration.h"
#include "VertexDef.h"



class EngineExport IVertexBuffer //: public EngineAllocator(IVertexBuffer)
{
	EngineAllocator(IVertexBuffer)
	friend class IRenderer;

public:
    IVertexBuffer() : m_nVertexCount(0), m_nUnitSize(0), m_nSize(0), m_bLock(false) {};
    virtual ~IVertexBuffer() {};

    //************************************
    // Method:    Create
    // FullName:  IVertexBuffer::Create
    // Access:    virtual public 
    // Returns:   bool : 是否创建成功
    // Qualifier: 创建顶点缓冲
    // Parameter: EVertexBufferType type : 顶点类型
    // Parameter: void * lpBuffer : 顶点内存数据
    // Parameter: u32 nVertexCount : 顶点个数
    // Parameter: bool bDynamic : 是否为动态创建
    //************************************
    //virtual bool				Create(EVertexBufferType type, void* lpBuffer, u32 nVertexCount, bool bDynamic = false) = 0;

    //************************************
    // Method:    Create
    // FullName:  IVertexBuffer::Create
    // Access:    virtual public 
    // Returns:   bool : 是否创建成功
    // Qualifier: 创建顶点缓冲
    // Parameter: EVertexBufferType type : 顶点类型
    // Parameter: u32 nVertexCount : 顶点个数
    // Parameter: bool bDynamic : 是否为动态创建
    //************************************
    //virtual bool				Create(EVertexBufferType type, u32 nVertexCount, bool bDynamic = false) = 0;

	//************************************
	// Method:    Create
	// FullName:  IVertexBuffer::Create
	// Access:    virtual public 
	// Returns:   bool : 是否创建成功
	// Qualifier: 创建顶点缓冲
	// Parameter: CVertexDeclaration * pVD : 顶点格式对象
	// Parameter: void * lpBuffer : 顶点内存数据
	// Parameter: u32 nVertexCount : 顶点个数
	// Parameter: bool bDynamic : 是否为动态创建
	//************************************
	//virtual bool				Create(CVertexDeclaration* pVD, void* lpBuffer, u32 nVertexCount, bool bDynamic = false) = 0;

	//************************************
	// Method:    Create
	// FullName:  IVertexBuffer::Create
	// Access:    virtual public 
	// Returns:   bool : 是否创建成功
	// Qualifier: 创建顶点缓冲
	// Parameter: CVertexDeclaration * pVD  : 顶点格式对象
	// Parameter: u32 nVertexCount : 顶点个数
	// Parameter: bool bDynamic : 是否为动态创建
	//************************************
	//virtual bool				Create(CVertexDeclaration* pVD, u32 nVertexCount, bool bDynamic = false) = 0;

    virtual bool				Create(u32 nVertexCount, u32 nUnitSize, bool bDynamic = false) = 0;

    //************************************
    // Method:    Lock
    // FullName:  IVertexBuffer::Lock
    // Access:    virtual public 
    // Returns:   void*
    // Qualifier: 锁定顶点缓冲
    // Parameter: bool bDiscard
    //************************************
    virtual void*				Lock(bool bDiscard = false) = 0;

    //************************************
    // Method:    Unlock
    // FullName:  IVertexBuffer::Unlock
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: 解锁顶点缓冲
    //************************************
    virtual void				Unlock() = 0;

    //************************************
    // Method:    Release
    // FullName:  IVertexBuffer::Release
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: 释放顶点缓冲对象
    //************************************
    virtual void				Release() = 0;

    //only use in D3D
    //************************************
    // Method:    GetFVF
    // FullName:  IVertexBuffer::GetFVF
    // Access:    virtual public 
    // Returns:   AIR::u32
    // Qualifier: 获得顶点格式.(注:使用顶点声明的方式创建的顶点缓冲,其格式值为0)
    //************************************
    virtual u32					GetFVF() = 0;

    //************************************
    // Method:    GetInterface
    // FullName:  IVertexBuffer::GetInterface
    // Access:    virtual public 
    // Returns:   void*
    // Qualifier: 获取D3D9的顶点缓冲对象
    //************************************
    virtual void*				GetInterface() = 0;

    //************************************
    // Method:    GetUnitSize
    // FullName:  IVertexBuffer::GetUnitSize
    // Access:    virtual public 
    // Returns:   AIR::u32
    // Qualifier: 获得顶点单元的大小(单位是字节)
    //************************************
    u32 GetUnitSize()
    {
        return m_nUnitSize;
    }

    //************************************
    // Method:    GetVertexCount
    // FullName:  IVertexBuffer::GetVertexCount
    // Access:    virtual public 
    // Returns:   AIR::u32
    // Qualifier: 获得顶点数量
    //************************************
    u32 GetVertexCount()
    {
        return m_nVertexCount;
    }

    u32 GetWholeSize( void ) 
    {
        return m_nSize;
    }

	//************************************
	// Method:    GetVertexDeclaration
	// FullName:  IVertexBuffer::GetVertexDeclaration
	// Access:    virtual public 
	// Returns:   CVertexDeclaration*
	// Qualifier: 获得顶点声明对象
	// Parameter: void
	//************************************
    /*
	virtual CVertexDeclaration* GetVertexDeclaration( void )
	{
		return m_pVertexDecl;
	}

    void						SetVertexDeclaration(CVertexDeclaration* pVertexDecl)
    {
        if (m_pVertexDecl && !m_pVertexDecl->IsAutoDelete() && m_pVertexDecl != pVertexDecl)
        {
            m_pVertexDecl->Clear();
            SafeDelete(m_pVertexDecl);
        }
        m_pVertexDecl = pVertexDecl;
    }
    */

	virtual void				WriteData( size_t offset, size_t length, const void* pSource, bool discardWholeBuffer ) = 0;

	virtual void				ReadData( size_t offset, size_t length, void* pDest ) = 0;

	bool						IsLock() { return m_bLock; }

protected:
	//************************************
	// Method:    ReleaseWhenResetDevice
	// FullName:  IVertexBuffer::ReleaseWhenResetDevice
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier: 当显示设备重置的时候进行资源释放
	// Parameter: void
	//************************************
	//virtual void				ReleaseWhenResetDevice( void ) = 0;

	//************************************
	// Method:    CreateWhenResetDevice
	// FullName:  IVertexBuffer::CreateWhenResetDevice
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier: 当显示设备重置的时候进行资源重建
	// Parameter: void
	//************************************
	//virtual void				CreateWhenResetDevice( void ) = 0;




protected:
	//顶点声明
	//CVertexDeclaration*			m_pVertexDecl;
    u32						m_nVertexCount;		// 顶点数量
    u32						m_nUnitSize;		// 一个顶点所占的字节
    u32						m_nSize;			// 缓冲大小（单位：字节）
	bool					m_bLock;
};
