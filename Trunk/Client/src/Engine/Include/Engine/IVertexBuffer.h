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
    // Returns:   bool : �Ƿ񴴽��ɹ�
    // Qualifier: �������㻺��
    // Parameter: EVertexBufferType type : ��������
    // Parameter: void * lpBuffer : �����ڴ�����
    // Parameter: u32 nVertexCount : �������
    // Parameter: bool bDynamic : �Ƿ�Ϊ��̬����
    //************************************
    //virtual bool				Create(EVertexBufferType type, void* lpBuffer, u32 nVertexCount, bool bDynamic = false) = 0;

    //************************************
    // Method:    Create
    // FullName:  IVertexBuffer::Create
    // Access:    virtual public 
    // Returns:   bool : �Ƿ񴴽��ɹ�
    // Qualifier: �������㻺��
    // Parameter: EVertexBufferType type : ��������
    // Parameter: u32 nVertexCount : �������
    // Parameter: bool bDynamic : �Ƿ�Ϊ��̬����
    //************************************
    //virtual bool				Create(EVertexBufferType type, u32 nVertexCount, bool bDynamic = false) = 0;

	//************************************
	// Method:    Create
	// FullName:  IVertexBuffer::Create
	// Access:    virtual public 
	// Returns:   bool : �Ƿ񴴽��ɹ�
	// Qualifier: �������㻺��
	// Parameter: CVertexDeclaration * pVD : �����ʽ����
	// Parameter: void * lpBuffer : �����ڴ�����
	// Parameter: u32 nVertexCount : �������
	// Parameter: bool bDynamic : �Ƿ�Ϊ��̬����
	//************************************
	//virtual bool				Create(CVertexDeclaration* pVD, void* lpBuffer, u32 nVertexCount, bool bDynamic = false) = 0;

	//************************************
	// Method:    Create
	// FullName:  IVertexBuffer::Create
	// Access:    virtual public 
	// Returns:   bool : �Ƿ񴴽��ɹ�
	// Qualifier: �������㻺��
	// Parameter: CVertexDeclaration * pVD  : �����ʽ����
	// Parameter: u32 nVertexCount : �������
	// Parameter: bool bDynamic : �Ƿ�Ϊ��̬����
	//************************************
	//virtual bool				Create(CVertexDeclaration* pVD, u32 nVertexCount, bool bDynamic = false) = 0;

    virtual bool				Create(u32 nVertexCount, u32 nUnitSize, bool bDynamic = false) = 0;

    //************************************
    // Method:    Lock
    // FullName:  IVertexBuffer::Lock
    // Access:    virtual public 
    // Returns:   void*
    // Qualifier: �������㻺��
    // Parameter: bool bDiscard
    //************************************
    virtual void*				Lock(bool bDiscard = false) = 0;

    //************************************
    // Method:    Unlock
    // FullName:  IVertexBuffer::Unlock
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: �������㻺��
    //************************************
    virtual void				Unlock() = 0;

    //************************************
    // Method:    Release
    // FullName:  IVertexBuffer::Release
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: �ͷŶ��㻺�����
    //************************************
    virtual void				Release() = 0;

    //only use in D3D
    //************************************
    // Method:    GetFVF
    // FullName:  IVertexBuffer::GetFVF
    // Access:    virtual public 
    // Returns:   AIR::u32
    // Qualifier: ��ö����ʽ.(ע:ʹ�ö��������ķ�ʽ�����Ķ��㻺��,���ʽֵΪ0)
    //************************************
    virtual u32					GetFVF() = 0;

    //************************************
    // Method:    GetInterface
    // FullName:  IVertexBuffer::GetInterface
    // Access:    virtual public 
    // Returns:   void*
    // Qualifier: ��ȡD3D9�Ķ��㻺�����
    //************************************
    virtual void*				GetInterface() = 0;

    //************************************
    // Method:    GetUnitSize
    // FullName:  IVertexBuffer::GetUnitSize
    // Access:    virtual public 
    // Returns:   AIR::u32
    // Qualifier: ��ö��㵥Ԫ�Ĵ�С(��λ���ֽ�)
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
    // Qualifier: ��ö�������
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
	// Qualifier: ��ö�����������
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
	// Qualifier: ����ʾ�豸���õ�ʱ�������Դ�ͷ�
	// Parameter: void
	//************************************
	//virtual void				ReleaseWhenResetDevice( void ) = 0;

	//************************************
	// Method:    CreateWhenResetDevice
	// FullName:  IVertexBuffer::CreateWhenResetDevice
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier: ����ʾ�豸���õ�ʱ�������Դ�ؽ�
	// Parameter: void
	//************************************
	//virtual void				CreateWhenResetDevice( void ) = 0;




protected:
	//��������
	//CVertexDeclaration*			m_pVertexDecl;
    u32						m_nVertexCount;		// ��������
    u32						m_nUnitSize;		// һ��������ռ���ֽ�
    u32						m_nSize;			// �����С����λ���ֽڣ�
	bool					m_bLock;
};
