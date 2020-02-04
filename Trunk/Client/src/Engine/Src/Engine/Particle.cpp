#include "stdafx.h"
#include "Particle.h"
#include "ISceneManager.h"
#include "ModelInstance.h"

CParticle::CParticle() : 
m_fLifeTime(0), 
m_fTotalTimeToLive(0),
m_fRotationAngle(0), 
m_fRotationSpeed(0), 
m_fWidth(0), 
m_fHeight(0),
m_particleType(PARTICLE_NORMAL),
m_bTrail(false),
m_nSegmentIndex(-1),
m_fAxisRotateSpeed(0.0f),
m_fInitWidth(0),
m_fInitHeight(0)
, m_pRenderable(NULL)
, m_pSceneNode(NULL)
//m_pChainSeg(NULL)
{
 /*   m_color = 0xffffffff;
    m_colorDelta = 0;*/
}

CParticle::~CParticle()
{
	m_fAxisRotateSpeed = 0.0;
}

void CParticle::CreateRenderable(const char* szName, const char* szFilename)
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    if (m_pRenderable && strcmp(m_pRenderable->GetResourceName().c_str(), szFilename) != 0)
    {
        pScene->DestroyRenderable(m_pRenderable);
        m_pRenderable = (Renderable*)pScene->CreateRenderable<CModelInstance>(szFilename);
    }
    else if (m_pRenderable == NULL)
        m_pRenderable = (Renderable*)pScene->CreateRenderable<CModelInstance>(szFilename);
    if (m_pRenderable == NULL)
    {
        return;
    }
    if (m_pSceneNode == NULL)
    {
        m_pSceneNode = new CSceneNode();
    }
    
    m_pSceneNode->AttachRenderable(m_pRenderable);
}

void CParticle::CaculateRenderableTransform()
{
    if (m_pSceneNode)
    {
        m_pSceneNode->SetPosition(m_position);
        float fScale = m_fWidth / m_fInitWidth;
        m_pSceneNode->SetScale(Vector3Df::UNIT_SCALE * fScale);
        m_pSceneNode->Update();
    }

    if (m_pRenderable)
    {
        if (m_color.a < 1.0f)
        {
            m_pRenderable->SetTransparent(true, m_color.a * 255);
        }
        else
        {
            m_pRenderable->SetTransparent(false, 255);
        }
    }
}

void CParticle::SetRenderable(Renderable* pRenderable)
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    if (m_pRenderable)
    {
        if (m_pRenderable != pRenderable)
        {
            pScene->DestroyRenderable(m_pRenderable);
            m_pRenderable = pRenderable;
        }
        else
        {
            return;
        }
    }
    else
    {
        m_pRenderable = pRenderable;
    }

    if (m_pRenderable)
    {
        if (m_pSceneNode)
        {
            m_pSceneNode->AttachRenderable(m_pRenderable);
        }
        else
        {
            m_pSceneNode = new CSceneNode;
            m_pSceneNode->AttachRenderable(m_pRenderable);
        }
    }
}
