#include "stdafx.h"
#include "BoxRenderable.h"
#include "Engine.h"
#include "SceneNode.h"
#include "Primitive.h"
#include "RenderQueueGroup.h"

CBoxRenderable::CBoxRenderable() : m_pVertexBuffer(NULL), m_bVBCreated(false)
{
    m_strName = "Box";
    m_szType = "LineObject";
    //m_bShowBoundingBox = true;
    m_renderGroupType = eRGT_Line;
}

CBoxRenderable::~CBoxRenderable()
{

}

void CBoxRenderable::ClearRenderableImp()
{
    if (m_pVertexBuffer)
    {
        m_pVertexBuffer->Release();
        SafeDelete(m_pVertexBuffer);
    }
}

void CBoxRenderable::PreRender(CCamera* pCamera)
{

}

/*
void CBoxRenderable::Render(CCamera* pCamera)
{
    if (!m_bVisable)
    {
        return;
    }
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pNode)
    {
        pRenderer->SetWorldTransform(&m_pNode->GetFullTransform());
    }

    if (m_pMaterial)
    {
        //pRenderer->SetMaterial(m_pMaterial);
        CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
        if (pTechnique)
        {
            size_t nPassNum = pTechnique->GetPassesNum();
            for (size_t i = 0; i < nPassNum; ++i)
            {
                CPass* pPass = pTechnique->GetPass(i);
                pPass->ApplyPass(pRenderer);
                if (m_pVertexBuffer)
                {
                    pRenderer->SetVertexBuffer(0, m_pVertexBuffer);
                    pRenderer->DrawLineList(0, 12);
                }
            }
        }
    }

    Renderable::AfterRender();
}
*/

void CBoxRenderable::GenVertexBuffer()
{
    if (!m_bVBCreated)
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //m_pVertexBuffer = pRenderer->CreateVertexBuffer(E_VB_NOTEX, 12 * 2);
        m_pVertexBuffer = pRenderer->CreateVertexBuffer(12 * 2, sizeof(VERTEX_NOTEX));
        m_bVBCreated = true;
    }

    //  /3--------/7
    // /  |      / |
    ///   |     /  |
    //1---------5  |
    //|   2- - -| -6
    //|  /      |  /
    //|/        | /
    //0---------4/
    Vector3Df position0 = m_position - m_size / 2;
    Vector3Df position1 = position0 + Vector3Df::UNIT_Y * m_size.y;
    Vector3Df position2 = position0 + Vector3Df::UNIT_Z * m_size.z;
    Vector3Df position3 = position2 + Vector3Df::UNIT_Y * m_size.y;
    Vector3Df position4 = position0 + Vector3Df::UNIT_X * m_size.x;
    Vector3Df position5 = position1 + Vector3Df::UNIT_X * m_size.x;
    Vector3Df position6 = position4 + Vector3Df::UNIT_Z * m_size.z;
    Vector3Df position7 = position6 + Vector3Df::UNIT_Y * m_size.y;

    if (m_pVertexBuffer)
    {
        LPVERTEX_NOTEX pVertex = (LPVERTEX_NOTEX)m_pVertexBuffer->Lock();
        GenLine(&pVertex, position0, position2);
        GenLine(&pVertex, position2, position6);
        GenLine(&pVertex, position6, position4);
        GenLine(&pVertex, position4, position0);

        GenLine(&pVertex, position1, position3);
        GenLine(&pVertex, position3, position7);
        GenLine(&pVertex, position7, position5);
        GenLine(&pVertex, position5, position1);

        GenLine(&pVertex, position1, position0);
        GenLine(&pVertex, position3, position2);
        GenLine(&pVertex, position7, position6);
        GenLine(&pVertex, position5, position4);
        
        m_pVertexBuffer->Unlock();
    }
}

void CBoxRenderable::GenLine(LPVERTEX_NOTEX* pVertex, Vector3Df& pos1, Vector3Df& pos2)
{
    (*pVertex)->position = pos1;
    (*pVertex)->color = SColor(0xffffffff);
    ++(*pVertex);
    (*pVertex)->position = pos2;
    (*pVertex)->color = SColor(0xffffffff);
    ++(*pVertex);
}

void CBoxRenderable::SetSize(const Vector3Df& size)
{
    m_size = size;
    GenBoundingBox();
    GenVertexBuffer();
}

void CBoxRenderable::SetPosition(const Vector3Df& position)
{
    m_position = position;
    GenBoundingBox();
    GenVertexBuffer();
}

void CBoxRenderable::GenBoundingBox()
{
    m_boundingBox.MaxEdge = m_position + m_size / 2;
    m_boundingBox.MinEdge = m_position - m_size / 2;
}

void CBoxRenderable::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_LineList/*, m_pVertexBuffer->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
    }

    m_pPrimitive->m_pVertexBuffer = m_pVertexBuffer;
}
