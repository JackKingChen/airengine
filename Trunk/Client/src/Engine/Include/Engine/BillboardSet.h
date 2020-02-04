/********************************************************************
	created:	2010/05/22
	created:	22:5:2010   15:18
	filename: 	BillboardSet.h
	author:		liangairan
	
	purpose:	billboard渲染数据的集合
*********************************************************************/

#pragma once
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "IReference.h"

class EngineExport BillboardSet : public IReference//, public EngineAllocator(BillboardSet)
{
	EngineAllocator(BillboardSet)
public:
    BillboardSet();
    virtual ~BillboardSet();

    IVertexBuffer* GetVertexBuffer() 
    {
        return m_pVB;
    }

    IIndexBuffer*  GetIndexBuffer()
    {
        return m_pIB;
    }

    //创建数据
    virtual bool Create(u32 nBillboardsNum);

    u32 GetVertexCount() const
    {
        if (m_pVB)
        {
            return m_pVB->GetVertexCount();
        }
        return 0;
    }

    void Clear();

    int GetBillboardsNum() const
    {
        return m_nBillboardsNum;
    }

    void SetBillboardsNum(u32 nBillboardsNum);

    virtual s32 Release();
protected:
    IVertexBuffer*  m_pVB;
    IIndexBuffer*   m_pIB;
    u32             m_nBillboardsNum;   //billboard数量
private:
};

class EngineExport BillboardChainData : public BillboardSet
{
public:
    BillboardChainData();
    virtual ~BillboardChainData();

    virtual bool Create(int nBillboardsNum);

    u32 GetChainsNum() const
    {
        return m_nChainsNum;
    }
protected:
    u32             m_nChainsNum;   //billboardchain数量
private:
};
