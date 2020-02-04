#include "stdafx.h"
#include "BoundingBoxEntity.h"
#include "Engine.h"
#include "Primitive.h"
#include "RenderQueueGroup.h"
#include "RenderStateObject.h"

static const int scTriangleIDS[36] = 
{ 
	1,5,0,		0,5,4,
	5,7,4,		4,7,6,
	7,3,6,		6,3,2,
	3,1,2,		2,1,0,
	3,7,1,		1,7,5,
	2,6,0,		0,6,4,
};

static const int scLineIDS[] = 
{ 
	0,1, 		4,5, 		6,7, 		2,3, 
	1,5, 		0,4, 		2,6, 		3,7, 
	1,3, 		5,7, 		4,6, 		0,2 
};



BoundingBoxRenderData::BoundingBoxRenderData() : m_pVB(NULL), m_pOcclPtimitive(NULL), m_pOcclVB(NULL)
{
}

BoundingBoxRenderData::~BoundingBoxRenderData()
{
    SafeDelete(m_pPrimitive);
	SafeDelete(m_pOcclPtimitive);
}

void BoundingBoxRenderData::Clear()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pVB)
    {
        pRenderer->ClearVertexBuffer(m_pVB);
        m_pVB = NULL;
    }    
	if (m_pOcclVB)
	{
		pRenderer->ClearVertexBuffer(m_pOcclVB);
		m_pOcclVB = NULL;
	}
	if (m_DepthStencilSO)
	{
		SafeDelete(m_DepthStencilSO);
	}		
}

void BoundingBoxRenderData::Render(const AABBBox3Df& boundingBox)
{
    CreateVertexBuffer();
    Vector3Df vlist[8];
    boundingBox.GetEdges( vlist );
    RenderBoundingBox(vlist);
}

void BoundingBoxRenderData::RenderOcclusionBoundingBox( const AABBBox3Df& boundingBox, u32 color )
{
	CreateVertexBuffer();
	Vector3Df vlist[8];
	boundingBox.GetEdges( vlist );
	RenderOcclusionBoundingBoxImpl( vlist, color );
}

void BoundingBoxRenderData::RenderOcclusionBoundingBoxImpl( const Vector3Df* pVertices, u32 color )
{
	LPVERTEX_NOTEX pVertex = (LPVERTEX_NOTEX)m_pOcclVB->Lock();
	for ( int i = 0; i < 36; ++i )
	{ 
		pVertex[i].position = pVertices[ scTriangleIDS[i] ];
		pVertex[i].color = color;
	}
	m_pOcclVB->Unlock();
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
	pRenderer->SetTexture(0, NULL);
	pRenderer->SetVertexShader(NULL);
	pRenderer->SetPixelShader(NULL);
	if (m_pOcclPtimitive == NULL)
	{
		m_pOcclPtimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_pOcclVB->GetVertexDeclaration()*/);
		m_pOcclPtimitive->m_pVertexBuffer = m_pOcclVB;
	}
	if (!m_DepthStencilSO)
	{
		m_DepthStencilSO = pRenderer->CreateDepthStencilStateObject();
	}
	m_DepthStencilSO->Apply(pRenderer);
	//m_pOcclPtimitive->Draw(NULL);
}

void BoundingBoxRenderData::CreateVertexBuffer()
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer(); 
	LPVERTEX_NOTEX pVertex = NULL;

    if (!m_pVB)
    {         
        //m_pVB = pRenderer->CreateVertexBuffer(E_VB_NOTEX, 12 * 2);
        m_pVB = pRenderer->CreateVertexBuffer(12 * 2, sizeof(VERTEX_NOTEX));
		pVertex = (LPVERTEX_NOTEX)m_pVB->Lock();
		for ( int i = 0; i < 24; )
		{ 
			pVertex[i++].color = 0xffffffff;
		}
		m_pVB->Unlock();
    }
	if (!m_pOcclVB)
	{
		//m_pOcclVB = pRenderer->CreateVertexBuffer(E_VB_NOTEX, 12 * 3);
        m_pOcclVB = pRenderer->CreateVertexBuffer(12 * 3, sizeof(VERTEX_NOTEX));
		pVertex = (LPVERTEX_NOTEX)m_pOcclVB->Lock();
		for ( int i = 0; i < 36; )
		{ 
			pVertex[i++].color = 0xffffffff;
		}
		m_pOcclVB->Unlock();
	}
}

void BoundingBoxRenderData::RenderBoundingBox(const Vector3Df* pVertices)
{
    LPVERTEX_NOTEX pVertex = (LPVERTEX_NOTEX)m_pVB->Lock();
    for ( int i = 0; i < 24; ++i )
    { 
        pVertex[i].position = pVertices[ scLineIDS[i] ];
    }
    m_pVB->Unlock();
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
    pRenderer->SetTexture(0, NULL);
    pRenderer->SetVertexShader(NULL);
    pRenderer->SetPixelShader(NULL);
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_LineList/*, m_pVB->GetVertexDeclaration()*/);
        m_pPrimitive->m_pVertexBuffer = m_pVB;
    }
    //m_pPrimitive->Draw(NULL);
}


BoundingBoxEntity::BoundingBoxEntity()
{
    m_szType = "LineObject";
    m_renderGroupType = eRGT_Line;
	m_strVertexDecl = "E_VB_NOTEX";
}

BoundingBoxEntity::~BoundingBoxEntity()
{

}

void BoundingBoxEntity::SetWorldBoundingBox(const AABBBox3Df& boundingBox)
{
    m_WorldAABB = boundingBox;
}

void BoundingBoxEntity::Render(CCamera* pCamera)
{
    BoundingBoxInstance->Render(m_WorldAABB);
}

