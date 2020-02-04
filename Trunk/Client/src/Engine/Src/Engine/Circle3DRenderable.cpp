#include "stdafx.h"
#include "Circle3DRenderable.h"
#include "Engine.h"
#include "VertexDef.h"
#include "MaterialManager.h"
#include "Primitive.h"

CCircle3DRenderable::CCircle3DRenderable(float a, float b, const Vector3Df& position, const Vector3Df& direction) :
//m_pVertexBuffer(NULL),
m_a(a),
m_b(b),
m_position(position),
m_direction(direction),
m_bVBCreated(false)
{
    CreateVertexBuffer();
}

CCircle3DRenderable::~CCircle3DRenderable()
{
    ClearRenderableImp();
}

void CCircle3DRenderable::ClearRenderableImp()
{
    //if (m_pVertexBuffer)
    //{
    //    m_pVertexBuffer->Release();
    //    SafeDelete(m_pVertexBuffer);
    //}
    m_renderData.Clear();
}

void CCircle3DRenderable::CreateVertexBuffer()
{
    //以10度为标准
    if (!m_bVBCreated)
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //m_renderData.m_pVB = pRenderer->CreateVertexBuffer(E_VB_NOTEX, 37);
        m_renderData.m_pVB = pRenderer->CreateVertexBuffer(37, sizeof(VERTEX_NOTEX));

        m_renderData.m_nPrimCount = 36;
        m_renderData.m_renderType = RT_LINE_STRIP;
        if (!m_renderData.m_pVB)
        {
            return;
        }
        m_bVBCreated = true;
    }

    if (!m_renderData.m_pVB)
    {
        return;
    }

    
    Vector3Df firstPos;
    Vector3Df vec;
    if (m_direction == Vector3Df::UNIT_Y)
    {
        vec = Vector3Df::UNIT_Z;
        firstPos = m_position + Vector3Df::UNIT_Z * m_b;
    }
    LPVERTEX_NOTEX pVertices = (LPVERTEX_NOTEX)m_renderData.m_pVB->Lock();
    pVertices->position = firstPos;
    pVertices->color = SColor(0xffffffff);
    ++pVertices;
    Quaternion rotate;
    rotate.FromAngleAxis(AIR::PI / 18, Vector3Df::UNIT_Y);
    for (int i = 0; i < 36; i++)
    {
        vec = rotate * vec;
        vec.Normalize();
        float fRadius = AIR::PI / 18 * (i + 1);
        float fSin = sin(fRadius);
        float fCos = cos(fRadius);
        float fLength = sqrt((m_a * m_a * m_b * m_b) / (fSin * fSin * m_b * m_b + fCos * fCos * m_a * m_a));
        pVertices->position = m_position +  vec * fLength;
        pVertices->color = SColor(0xffffffff);
        ++pVertices;
    }

    m_renderData.m_pVB->Unlock();
}

/*
void CCircle3DRenderable::Render(CCamera* pCamera)
{
    if (!m_bVisable)
    {
        return;
    }
    Renderable::Render(pCamera);

	Renderable::AfterRender();
}
*/

void CCircle3DRenderable::PreRender(CCamera* pCamera)
{
    //if (m_pMaterial == NULL)
    //{
    //    m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/LineObject");
    //}
}

void CCircle3DRenderable::SetA(float a)
{
    m_a = a;
    CreateVertexBuffer();
}

void CCircle3DRenderable::SetB(float b)
{
    m_b = b;
    CreateVertexBuffer();
}

void CCircle3DRenderable::SetSize(float a, float b)
{
    m_a = a;
    m_b = b;
    CreateVertexBuffer();
}

void CCircle3DRenderable::SetDirection(const Vector3Df& vDirection)
{
    m_direction = vDirection;
    CreateVertexBuffer();
}

void CCircle3DRenderable::SetPosition(const Vector3Df& vPosition)
{
    m_position = vPosition;
    CreateVertexBuffer();
}

void CCircle3DRenderable::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_ListStrip/*, m_renderData.m_pVB->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
       
    }

    m_pPrimitive->m_pVertexBuffer = m_renderData.m_pVB;
    m_pPrimitive->m_pIndexBuffer = m_renderData.m_pIB;
}
