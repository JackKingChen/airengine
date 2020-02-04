#include "stdafx.h"
#include "BillboardRenderer.h"
#include "MaterialManager.h"
#include "Engine.h"
#include "Primitive.h"
//#include "TextureUnit.h"
//#include "Pass.h"
//#include "Technique.h"

BillboardRenderer::BillboardRenderer() : m_nMaxBillboardCountPerDP(500)
, m_nMaxVertexCountPerDP(4 * 500)
, m_nMaxIndexCountPerDP(6 * 500)
, m_nCurTriangleCountToRender(0)
, m_nCurVertexCountToRender(0)
, m_nCurIndexCountToRender(0)
, m_pVB(NULL)
, m_pIB(NULL)
, m_pMaterial(NULL)
, m_pCurTexture(NULL)
, m_pVertices(NULL)
, m_bInitialize(false)
, m_pPrimitive(NULL)
{

}

BillboardRenderer::~BillboardRenderer()
{ 
    SafeDelete(m_pPrimitive);
}

void BillboardRenderer::Clear()
{
    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    }

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pVB)
    {
        pRenderer->ClearVertexBuffer(m_pVB);
    }

    if (m_pIB)
    {
        pRenderer->ClearIndexBuffer(m_pIB);
    }

    if (m_pVertices)
    {
        //SafeDeleteArray(m_pVertices);
        AIR_DELETE_ARRAY_T(m_pVertices, VERTEX_STANDARD, m_nMaxVertexCountPerDP);
    }
}

void BillboardRenderer::GenRenderData()
{
    if (m_pMaterial == NULL)
    {
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Particle");
    }

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    if (m_nMaxBillboardCountPerDP > 0)
    {
        m_nMaxVertexCountPerDP = m_nMaxBillboardCountPerDP * 4;
        m_nMaxIndexCountPerDP = m_nMaxBillboardCountPerDP * 6;

        if (m_pVB)
        {
            pRenderer->ClearVertexBuffer(m_pVB);
        }

        if (m_pIB)
        {
            pRenderer->ClearIndexBuffer(m_pIB);
        }

        //m_pVB = pRenderer->CreateVertexBuffer(E_VB_STANDARD, m_nMaxVertexCountPerDP);
        m_pVB = pRenderer->CreateVertexBuffer(m_nMaxVertexCountPerDP, sizeof(VERTEX_STANDARD));
        m_pIB = pRenderer->CreateIndexBuffer(m_nMaxIndexCountPerDP);

        // 锁定索引缓冲,并填充数据
        u16* pData = static_cast<u16*>(m_pIB->Lock( 0, 0, true ));
        int nCurIndex = 0;
        // 遍历所有的矩形
        for ( int i = 0; i < m_nMaxBillboardCountPerDP; ++i )
        {
            // 将矩形的6个索引值加到缓冲里面
            AddQuadIndex( i, &pData, nCurIndex );
        }
        // 解锁索引缓冲
        m_pIB->Unlock();

        if (m_pVertices)
        {
            SafeDeleteArray(m_pVertices);
        }

        //m_pVertices = new VERTEX_STANDARD[m_nMaxVertexCountPerDP];
        m_pVertices = AIR_NEW_ARRAY_T(VERTEX_STANDARD, m_nMaxVertexCountPerDP);
    }
    m_bInitialize = true;
}

void BillboardRenderer::AddQuadIndex(int nQuadID, u16** ppData, int & nIndexPos)
{
    // 左上角的索引ID
    int nLT = nQuadID * 4;
    // 右上角的索引ID
    int nRT = nLT + 1;
    // 右下角的索引ID
    int nRB = nRT + 1;
    // 左下角的索引ID
    int nLB = nRB + 1;

    // 三角形0-1-3
    (*ppData)[nIndexPos++] = nLT;
    (*ppData)[nIndexPos++] = nRT;
    (*ppData)[nIndexPos++] = nLB;

    // 三角形3-1-2
    (*ppData)[nIndexPos++] = nLB;
    (*ppData)[nIndexPos++] = nRT;
    (*ppData)[nIndexPos++] = nRB;
}

void BillboardRenderer::SetMaxBillboardCount(int nMaxBillboard)
{
    if (m_nMaxBillboardCountPerDP != nMaxBillboard)
    {
        m_nMaxBillboardCountPerDP = nMaxBillboard;
        GenRenderData();
    }
}

void BillboardRenderer::Render(CBillboard* pBillboard)
{
    // 如果当前是从顶点数组的起始位置填充
    if (!m_bInitialize)
    {
        GenRenderData();
    }
    if ( 0 == m_nCurVertexCountToRender )
    {
        // 记录当前纹理
        m_pCurTexture = pBillboard->GetTexture();
        // 填充顶点
        AddBillboardVertex(pBillboard);
    }
    // 如果当前不是从顶点数组的起始位置填充
    else if ( m_nCurVertexCountToRender < m_nMaxVertexCountPerDP )
    {
        // 如果当前的纹理不一致,现将缓冲渲染出来
        if ( m_pCurTexture != pBillboard->GetTexture() )
        {
            RenderImmediately();
            m_pCurTexture = pBillboard->GetTexture();
        }
        // 填充顶点数据
        AddBillboardVertex( pBillboard );
    }

    // 如果当前缓冲已经满了,就将缓冲区绘制出来,并清空
    if ( m_nCurVertexCountToRender == m_nMaxVertexCountPerDP )
    {
        RenderImmediately();
    }
}

void BillboardRenderer::RenderImmediately()
{
    if (m_nCurVertexCountToRender == 0)
    {
        return;
    }

    //if (!m_bInitialize)
    //{
    //    GenRenderData();
    //}
	if (m_pMaterial->IsReady())
	{
		void* pData = m_pVB->Lock( );
		memcpy( pData, m_pVertices, sizeof(VERTEX_STANDARD) * m_nCurVertexCountToRender );
		m_pVB->Unlock();

		// 设置纹理
		IRenderer* pRenderer = ENGINE_INST->GetRenderer();
		pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
        /*
		CPass* pPass = m_pMaterial->GetCurrentTechnique()->GetPass(0);
		//pPass->ApplyTextureStates();
		pPass->ApplyShaders(pRenderer);
		CTextureUnit* pUnit = pPass->GetTextureUnit(0);
		pUnit->ApplyTextureUnit(pPass, m_pCurTexture);
		for (int i = 1; i < 8; ++i)
		{
			pRenderer->SetTexture(i, NULL);
		}

		if (m_pPrimitive == NULL)
		{
			m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList);
			m_pPrimitive->m_pVertexBuffer = m_pVB;
			m_pPrimitive->m_pIndexBuffer = m_pIB;
			m_pPrimitive->SetUseUserPrimitive(true);
		}
        */

		m_pPrimitive->SetPrimitiveCount(m_nCurTriangleCountToRender);

		//m_pPrimitive->Draw(NULL);
		m_nCurVertexCountToRender = 0;
		m_nCurTriangleCountToRender = 0;
		m_nCurIndexCountToRender = 0;
	}
	else
	{
		m_pMaterial->Cache();
	}
}

void BillboardRenderer::AddBillboardVertex(CBillboard* pBillboard)
{
    // 顶点数增加4个
    // 左上角的顶点
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetTopLeft();

    // 右上角的顶点
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetTopRight();

    // 右下角的顶点
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetBottomRight();

    // 左下角的顶点
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetBottomLeft();

    // 索引数增加6个
    m_nCurIndexCountToRender += 6;

    // 三角形数增加2个
    m_nCurTriangleCountToRender += 2;
}

void BillboardRenderer::Initialize()
{
    GenRenderData();
}
