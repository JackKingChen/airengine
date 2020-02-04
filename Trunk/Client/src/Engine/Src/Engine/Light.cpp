#include "stdafx.h"
#include "Light.h"
#include "SceneNode.h"
#include "LightingManager.h"
#include "BoundingBoxEntity.h"

const Vector3Df& CLight::GetDerivedPosition()
{
    Update();
    return m_DerivedPosition;
}

const Vector3Df& CLight::GetDerivedDirection()
{
    Update();
    return m_DerivedDirection;
}

void CLight::Update()
{
    if (!m_bTransform)
    {
        
        if (m_pNode)
        {
            const Quaternion& parentOrientation = m_pNode->GetDerivedRotation();
            const Vector3Df& parentPosition = m_pNode->GetDerivedPosition();
            m_DerivedDirection = parentOrientation * m_vDirection;
            m_DerivedPosition = (parentOrientation * m_vPosition) + parentPosition;
        }
        else
        {
            m_DerivedPosition = m_vPosition;
            m_DerivedDirection = m_vDirection;
        }
        m_bTransform = true;
    }
}

void CLight::Update(float fTimeElapse)
{
    Renderable::UpdateNodeAnimation(fTimeElapse);
}

void CLight::SetAttachedNode(CSceneNode* pNode)
{
    Renderable::SetAttachedNode(pNode);
    m_bTransform = false;
}

void CLight::SetPosition(const Vector3Df& position)
{
    m_vPosition = position;
    m_bTransform = false;
}

void CLight::SetPosition(float x, float y, float z)
{
    m_vPosition.x = x;
    m_vPosition.y = y;
    m_vPosition.z = z;
    m_bTransform = false;
    CaculateBoundingbox();
}

void CLight::SetDirection(const Vector3Df& direction)
{
    m_vDirection = direction;
    m_vDirection.Normalize();
    m_bTransform = false;
}

void CLight::NotifyMove()
{
    m_bTransform = false;
}

void CLight::SetRange(float fRange)
{
    m_range = fRange;
    CaculateBoundingbox();
}

void CLight::CaculateBoundingbox()
{
    m_boundingBox.MaxEdge.x = m_vPosition.x + m_range;
    m_boundingBox.MaxEdge.y = m_vPosition.y + m_range;
    m_boundingBox.MaxEdge.z = m_vPosition.z + m_range;

    m_boundingBox.MinEdge.x = m_vPosition.x - m_range;
    m_boundingBox.MinEdge.y = m_vPosition.y - m_range;
    m_boundingBox.MinEdge.z = m_vPosition.z - m_range;
}

void CLight::SetAmbient(const CColorValue& color)
{
    m_clrAmbient = color;
}

void CLight::SetAttentionConst(f32 fAttention)
{
    m_fAttentionConst = fAttention;
}

void CLight::SetAttentionLinear(f32 fAttention)
{
    m_fAttentionLinear = fAttention;
}

void CLight::SetAttentionQuad(f32 fAttention)
{
    m_fAttentionQuad = fAttention;
}

void CLight::OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    m_pNode->GetFullTransform().TransformCoord(m_vPosition, m_DerivedPosition);
    if (m_type == LT_DIRECTIONAL)
    {
        m_pNode->GetFullTransform().TransformNormal(m_vDirection, m_DerivedDirection);
    }

    if (m_pCurNodeAnimState)
    {
        CommitNodeAnimation();
        m_bTransform = false;
    }
    LIGHTING_MANAGER->AddCurrentLight(this);
}

void CLight::PostRender(CCamera* pCamera)
{
    if (m_boundingBoxEntity == NULL)
    {
        m_boundingBoxEntity = new BoundingBoxEntity;
        m_boundingBoxEntity->SetWorldBoundingBox(GetWorldBoundingBox(true));
    }

    m_boundingBoxEntity->Render(pCamera);
}

