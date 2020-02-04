#include "stdafx.h"
#include "BillboardSet.h"
#include "Engine.h"

BillboardSet::BillboardSet() :
m_pVB(NULL)
, m_pIB(NULL)
{

}

BillboardSet::~BillboardSet()
{
    Clear();
}

void BillboardSet::Clear()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pVB)
    {
        pRenderer->ClearVertexBuffer(m_pVB);
    }

    if (m_pIB)
    {
        pRenderer->ClearIndexBuffer(m_pIB);
    }
}

bool BillboardSet::Create(u32 nBillboardsNum)
{
    
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pVB)
    {
        pRenderer->ClearVertexBuffer(m_pVB);
    }

    if (m_pIB)
    {
        pRenderer->ClearIndexBuffer(m_pIB);
    }
    //m_pVB = pRenderer->CreateVertexBuffer(E_VB_STANDARD, nBillboardsNum * 4, true);
    m_pVB = pRenderer->CreateVertexBuffer(nBillboardsNum * 4, sizeof(VERTEX_STANDARD), true);
    //CVertexDeclaration* pVC = pRenderer->CreateVertexDec
    if (m_pVB == NULL)
    {
        return false;
    }

    m_pIB = pRenderer->CreateIndexBuffer(nBillboardsNum * 2 * 3, true);

    if (m_pIB == NULL)
    {
        Clear();
        return false;
    }
    u16* pIndices = (u16*)m_pIB->Lock(0, 0, true);

    for(
        size_t idx, idxOff, bboard = 0;
        bboard < nBillboardsNum;
    ++bboard )
    {
        // Do indexes
        idx    = bboard * 6;
        idxOff = bboard * 4;

        pIndices[idx] = static_cast<unsigned short>(idxOff); // + 0;, for clarity
        pIndices[idx + 1] = static_cast<unsigned short>(idxOff + 1);
        pIndices[idx + 2] = static_cast<unsigned short>(idxOff + 2);
        pIndices[idx + 3] = static_cast<unsigned short>(idxOff + 1);
        pIndices[idx + 4] = static_cast<unsigned short>(idxOff + 3);
        pIndices[idx + 5] = static_cast<unsigned short>(idxOff + 2);

    }

    m_pIB->Unlock();

    m_nBillboardsNum = nBillboardsNum;
    return true;
}

s32 BillboardSet::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }
    return GetRef();
}

void BillboardSet::SetBillboardsNum(u32 nBillboardsNum)
{
    if (m_nBillboardsNum != nBillboardsNum)
    {
        Create(nBillboardsNum);
    }
}

BillboardChainData::BillboardChainData()
{

}

BillboardChainData::~BillboardChainData()
{

}

bool BillboardChainData::Create(int nBillboardsNum)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	if (m_pVB)
	{
		pRenderer->ClearVertexBuffer(m_pVB);
	}

	if (m_pIB)
	{
		pRenderer->ClearIndexBuffer(m_pIB);
	}

    //m_pVB = pRenderer->CreateVertexBuffer(E_VB_STANDARD, nBillboardsNum * 4, true);
    m_pVB = pRenderer->CreateVertexBuffer(nBillboardsNum * 4, sizeof(VERTEX_STANDARD), true);

    if (m_pVB == NULL)
    {
        return false;
    }

    m_pIB = pRenderer->CreateIndexBuffer(nBillboardsNum * 2 * 3, true);
    if (m_pIB == NULL)
    {
        Clear();
        return false;
    }

    m_nBillboardsNum = nBillboardsNum;
    m_nChainsNum = nBillboardsNum;
    return true;
}
