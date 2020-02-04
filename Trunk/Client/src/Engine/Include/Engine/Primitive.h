/********************************************************************
	created:	12:2:2011   9:13
	filename: 	Primitive.h
	author:		liangairan
	
	purpose:	PrimitiveµÄ·â×°
*********************************************************************/

#pragma once

#include "EngineConfig.h"
#include "AIRTypes.h"
#include <vector>
#include "ITexture.h"

using namespace AIR;

//class CTechnique;
class Renderable;
class IVertexBuffer;
class IIndexBuffer;
class CVertexDeclaration;

class EngineExport Primitive //: public EngineAllocator(Primitive)
{
	EngineAllocator(Primitive)
public:
    enum Type
    {
        Primitive_Point,
        Primitive_LineList,
        Primitive_ListStrip,
        Primitive_TriangleList,
        Primitive_TriangleStrip,
        Primitive_TriangleFan,
    };
public:
    Primitive(Type type/*, CVertexDeclaration* pDecl*/);
    virtual ~Primitive();

    //virtual void Draw(CTechnique* pTechnique);

    IVertexBuffer* m_pVertexBuffer;
    IIndexBuffer*  m_pIndexBuffer;
    Renderable*   m_pSrcRenderable;
    void*          m_pVertexData;  
    void*          m_pIndexData;

    u32 m_nVertexStart;
    u32 m_nIndexStart;

    u32 m_nIndicesNum;
    u32 m_nVerticesStride;

    //CVertexDeclaration*  m_pVertexDecl;

    Type GetType() const
    {
        return m_type;
    }

    void SetPrimitiveCount(int nPrimCount)
    {
        m_nPrimitiveCount = nPrimCount;
    }

    bool IsUserPrimitiveCount() const
    {
        return m_bUserPrimitiveCount;
    }

    u32 GetUserPrimitiveCount() const
    {
        return m_nPrimitiveCount;
    }

    void SetUseUserPrimitive(bool bUse)
    {
        m_bUserPrimitiveCount = bUse;
    }

    void SetType(Type type)
    {
        m_type = type;
    }

    u32 GetVerticesNum() const
    {
        return m_nVerticesNum;
    }

    void SetVerticesNum(u32 nVertices)
    {
        m_nVerticesNum = nVertices;
    }
protected:
    Type m_type;
    u32  m_nVerticesNum;
    u32  m_nPrimitiveCount;   
    bool m_bUserPrimitiveCount;
private:
};

class EngineExport InstancingPrimitive : public Primitive
{
public:
    InstancingPrimitive(Type type/*, CVertexDeclaration* pDecl*/);
    virtual ~InstancingPrimitive();

    IVertexBuffer* m_pInstanceVB;

    //virtual void Draw(CTechnique* pTechnique);
protected:
private:
};

class EngineExport SharePrimitive : public Primitive
{
public:
    SharePrimitive(Type type/*, CVertexDeclaration* pDecl*/);
    virtual ~SharePrimitive();

    //virtual void Draw(CTechnique* pTechnique);

    void AddPrimitive(Primitive* pPrimitive)
    {
        m_lstPrimitive.push_back(pPrimitive);
    }

    void AddTexture(ITexture* pTexture)
    {
        m_vtTextures.push_back(pTexture);
    }
protected:
private:
    typedef AIRVector<Primitive*> PrimitiveList;
    typedef PrimitiveList::iterator PrimitiveListIter;

    PrimitiveList  m_lstPrimitive;

    AIRVector<ITexture*> m_vtTextures;
};
