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

        // ������������,���������
        u16* pData = static_cast<u16*>(m_pIB->Lock( 0, 0, true ));
        int nCurIndex = 0;
        // �������еľ���
        for ( int i = 0; i < m_nMaxBillboardCountPerDP; ++i )
        {
            // �����ε�6������ֵ�ӵ���������
            AddQuadIndex( i, &pData, nCurIndex );
        }
        // ������������
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
    // ���Ͻǵ�����ID
    int nLT = nQuadID * 4;
    // ���Ͻǵ�����ID
    int nRT = nLT + 1;
    // ���½ǵ�����ID
    int nRB = nRT + 1;
    // ���½ǵ�����ID
    int nLB = nRB + 1;

    // ������0-1-3
    (*ppData)[nIndexPos++] = nLT;
    (*ppData)[nIndexPos++] = nRT;
    (*ppData)[nIndexPos++] = nLB;

    // ������3-1-2
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
    // �����ǰ�ǴӶ����������ʼλ�����
    if (!m_bInitialize)
    {
        GenRenderData();
    }
    if ( 0 == m_nCurVertexCountToRender )
    {
        // ��¼��ǰ����
        m_pCurTexture = pBillboard->GetTexture();
        // ��䶥��
        AddBillboardVertex(pBillboard);
    }
    // �����ǰ���ǴӶ����������ʼλ�����
    else if ( m_nCurVertexCountToRender < m_nMaxVertexCountPerDP )
    {
        // �����ǰ������һ��,�ֽ�������Ⱦ����
        if ( m_pCurTexture != pBillboard->GetTexture() )
        {
            RenderImmediately();
            m_pCurTexture = pBillboard->GetTexture();
        }
        // ��䶥������
        AddBillboardVertex( pBillboard );
    }

    // �����ǰ�����Ѿ�����,�ͽ����������Ƴ���,�����
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

		// ��������
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
    // ����������4��
    // ���ϽǵĶ���
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetTopLeft();

    // ���ϽǵĶ���
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetTopRight();

    // ���½ǵĶ���
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetBottomRight();

    // ���½ǵĶ���
    m_pVertices[m_nCurVertexCountToRender++] = pBillboard->GetBottomLeft();

    // ����������6��
    m_nCurIndexCountToRender += 6;

    // ������������2��
    m_nCurTriangleCountToRender += 2;
}

void BillboardRenderer::Initialize()
{
    GenRenderData();
}
