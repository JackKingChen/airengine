/********************************************************************
	created:	2010/02/17
	created:	17:2:2010   23:16
	filename: 	d:\My Codes\91wan\client\Current\Engine\Engine\Include\VertexDeclaration.h
	file path:	d:\My Codes\91wan\client\Current\Engine\Engine\Include
	file base:	VertexDeclaration
	file ext:	h
	author:		liangairan
	
	purpose:	顶点声明
*********************************************************************/

#pragma once 

#include "EngineConfig.h"
#include <list>
#include "VertexDef.h"
#include "MemoryAlloter.h"
#include "IteratorWrappers.h"

class IGPUProgram;

enum VertexElementType
{
    eVET_FLOAT1 = 0,
    eVET_FLOAT2 = 1,
    eVET_FLOAT3 = 2,
    eVET_FLOAT4 = 3,
    /// alias to more specific colour type - use the current rendersystem's colour packing
    eVET_COLOUR = 4,
    eVET_SHORT1 = 5,
    eVET_SHORT2 = 6,
    eVET_SHORT3 = 7,
    eVET_SHORT4 = 8,
    eVET_UBYTE4 = 9,
    /// D3D style compact colour
    eVET_COLOUR_ARGB = 10,
    /// GL style compact colour
    eVET_COLOUR_ABGR = 11
};

enum VertexElementUsage
{
    /// Position, 3 reals per vertex
    eVEU_POSITION = 1,
    /// Blending weights
    eVEU_BLEND_WEIGHTS = 2,
    /// Blending indices
    eVEU_BLEND_INDICES = 3,
    /// Normal, 3 reals per vertex
    eVEU_NORMAL = 4,
    /// Diffuse colours
    eVEU_DIFFUSE = 5,
    /// Specular colours
    eVEU_SPECULAR = 6,
    /// Texture coordinates
    eVEU_TEXTURE_COORDINATES = 7,
    /// Binormal (Y axis if normal is Z)
    eVEU_BINORMAL = 8,
    /// Tangent (X axis if normal is Z)
    eVEU_TANGENT = 9,
	/// rhw position, 4 reals per vertex
	eVEU_POSITIONT = 10,

};

typedef class CVertexElement 
{
public:
    //CVertexElement();
    CVertexElement(unsigned short nStream, 
        unsigned short nOffset,
        VertexElementType type,
        VertexElementUsage usage,
		bool bInstancing,
        unsigned short nIndex);
public:
    unsigned short		m_nStream;
    unsigned short		m_nOffset;
    VertexElementType	m_eType;
    VertexElementUsage	m_eUsage;
    unsigned short		m_nIndex;
	bool m_bInstancing;
}VERTEX_ELEMENT;



class EngineExport CVertexDeclaration //: public EngineAllocator(CVertexDeclaration)
{
	EngineAllocator(CVertexDeclaration)
public:
    typedef AIRList<VERTEX_ELEMENT> VertexElementList;
    typedef VertexElementList::iterator VertexElement_Iter;
public:
    CVertexDeclaration();

    virtual ~CVertexDeclaration();

    virtual void	AddVertexElement(
		unsigned short		nStream, 
        unsigned short		nOffset,
        VertexElementType	type,
        VertexElementUsage	usage,
        unsigned short		nIndex,
		bool bInstancing);

	virtual void	Build( EVertexBufferType type );

    virtual void	Build( void );

    virtual void    Clear( void ) {}

	size_t			GetStride( void ) { return m_nStride; }

    bool            IsAutoDelete() const
    {
        return m_bAutoDelete;
    }

    inline void SetAutoDelete(bool bAutoDelete)
    {
        m_bAutoDelete = bAutoDelete;
    }

    void SetStride(size_t nStride)
    {
        m_nStride = nStride;
    }

	static size_t GetTypeSize( VertexElementType etype );

    void Create(AIRVector<CVertexElement>& elements, IGPUProgram* pVertexShader);

    void Create(AIRVector<CVertexElement>& elements);

    ListIterator<VertexElementList> GetVertexElement()
    {
        return ListIterator<VertexElementList>(m_lstVertexElement);
    }
protected:

    virtual void BuildByVertexShader(IGPUProgram* pVertexShader);
    
    VertexElementList	m_lstVertexElement;
	size_t				m_nStride;
    bool                m_bAutoDelete;    //true，系统自动删除，false 用户自己删除 
};
