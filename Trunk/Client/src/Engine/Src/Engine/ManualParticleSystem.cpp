#include "stdafx.h"
#include "ManualParticleSystem.h"
#include "SceneNode.h"
#include "StringUtil.h"
#include "ParticleSystemResource.h"

ManualParticleSystem::ManualParticleSystem()
{

}

ManualParticleSystem::~ManualParticleSystem()
{

}

void ManualParticleSystem::Update(float fTimeElapse)
{
    // ���û�и������ڵ�,�����̷���
    if ( !m_pNode )
    {
        return;
    }

    //��������ϵͳ��Ⱦʱ������
    m_fRenderTimeElapsed += fTimeElapse;

    //��������ϵͳ��Χ�и���ʱ�䳤��
    m_fBoundingBoxUpdateElapse += fTimeElapse;

    //������Զ����벢�ҵ�ǰֹͣ�������Ӻ�������Ϊ0
    if ( m_bAutoDetach && m_bPauseEmitParticle && m_lstFreeParticles.size() == m_vtParticlePool.size() )
    {
        m_pNode->DetachRenderable( this );
        this->SetAttachedNode( NULL );
        return;
    }

    //u32 dwTimeStart = timeGetTime();
    //�������ӳ�
    Configure();

    // ������β
    SetupRibbonTrail();

    // ��ʼ�������䷢����
    //InitializeEmittedEmitters();

    // �鿴�Ƿ���Ҫ���»�������
    //NotifyRenderData();

    //��ʼ��
    if (!m_bFirstTimeInit)
    {
        m_pNode->NeedUpdate();
        m_pNode->Update();
        float fUpdateRemain = 1.0f;
        while (fUpdateRemain >= 0.1f)
        {
            //�жϵ�ǰ������Ƿ����
            Expire(0.1f);

            //����Ӱ����
            TriggerAffectors(0.1f);

            //�������ӵ�λ��
            ApplyMotion(0.1f);

            //����������
            TriggerEmitters(0.1f);

            //������β
            UpdateTrail( 0.1f );
            fUpdateRemain -= 0.1f;
        }
        //����boundingbox
        m_fBoundingBoxUpdateElapse = m_fBoundingBoxUpdateTime + 0.1f;
        UpdateBoundingBox();
        m_bFirstTimeInit = true;
    }

    Renderable::UpdateNodeAnimation(fTimeElapse);
}

void ManualParticleSystem::PreRender(CCamera* pCamera)
{
    // ���û�и������ڵ�,�����̷���
    //�жϵ�ǰ������Ƿ����
    Expire(ENGINE_INST->GetTimeSinceLastFrame());

    // ����Ӱ����
    TriggerAffectors(ENGINE_INST->GetTimeSinceLastFrame());

    // �������ӵ�λ��
    ApplyMotion(ENGINE_INST->GetTimeSinceLastFrame());

    // ����������
    //TriggerEmitters(ENGINE_INST->GetTimeSinceLastFrame());

    // ����������β
    UpdateTrail( ENGINE_INST->GetTimeSinceLastFrame() );

    // �������Ӱ�Χ��
    UpdateBoundingBox();

    switch ( m_pParticleSystemResource->GetParticleSystemType() )
    {
    case ePST_NONE:
        break;
    case ePST_QUAD:
        //���VB��IB
        UpdateBuffers(pCamera);
        break;
    case ePST_OBJ:
        break;
    }
}

void ManualParticleSystem::EmitParticles(int nParticlesNum)
{
    //��ǰ�ܲ��������������
    size_t emissionAllowed = m_lstFreeParticles.size();  
    //�����ʱ�̲�������������
    size_t totalRequested = 0;   
    size_t i = 0;
    //������������
    size_t emitterCount = m_vtParticleEmitter.size();//m_pParticleSystemResource->GetEmittersCount(); //m_vtParticleEmitter.size();

    size_t nEmitNums = nParticlesNum > emissionAllowed ? emissionAllowed : nParticlesNum;
    ExcuteTriggerEmitter(/*m_pParticleSystemResource->GetEmitter(0)*/m_vtParticleEmitter[0], nEmitNums, ENGINE_INST->GetTimeSinceLastFrame());
    
}

CParticle* ManualParticleSystem::EmitAParticle()
{
    float timePoint = 0.0f;
    float timeInc = ENGINE_INST->GetTimeSinceLastFrame();
    CParticleEmitter* pEmitter = /*m_pParticleSystemResource->GetEmitter(0);*/ m_vtParticleEmitter[0];
    const AIRString& strEmitter = pEmitter->GetEmittedEmitterName();

    //����һ������,ʵ�����Ǵӿ�������������ȡһ������
    CParticle* p = NULL;
    if (strEmitter == CStringUtil::BLANK)
    {
        p = CreateParticle();
    }
    else
    {
        //p = CreateEmitterParticle(strEmitter);
    }

    //�����������ʧ��,�����̷���
    if (!p)
        return NULL;

    //�������ӵĿ�ȸ߶��Լ���ת�Ƕ�
    float scale = pEmitter->GetScale();
    p->m_fWidth = p->m_fInitWidth = m_pParticleSystemResource->GetDefaultWidth() * scale;
    p->m_fHeight = p->m_fInitHeight = m_pParticleSystemResource->GetDefaultHeight() * scale;
    p->m_fRotationAngle = 0;

    //�÷����������ӽ��г�ʼ��
    pEmitter->InitParitcle(p);

    if (m_pTrail)
    {
        SetTrackParticle(p);
        //m_pTrail->SetTrackParticle(p);
    }


    ////////�任���ӵ�λ��
    //////matWorld.TransformCoord(p->m_position, p->m_position);
    ////////�任���ӵ��˶�����
    //////matWorld.TransformNormal(p->m_direction);

    //�������е�Ӱ����,�����ӽ��г�ʼ��
    /*
    VectorIterator<ParticleSystemResource::VECTOR_AFFECTOR> iter = m_pParticleSystemResource->GetAffectorIter();
    while (!iter.IsEnd())
    {
        CParticleAffector* pAffector = iter.GetNext();
        pAffector->InitParticle(p);
    }
    */
    for (VECTOR_AFFECTOR_ITER itAff = m_vtParticleAffector.begin(); itAff != m_vtParticleAffector.end(); ++itAff)
    {
        (*itAff)->InitParticle(p);
    }

    //��������λ��
    if (m_pNode)
    {
        const Matrix4f& matWorld = m_pNode->GetFullTransform();
        //�任���ӵ�λ��
        matWorld.TransformCoord(p->m_position, p->m_position);
        //�任���ӵ��˶�����
        matWorld.TransformNormal(p->m_direction);
    }
    p->m_position += (p->m_direction * timePoint);
    p->CaculateRenderableTransform();

    // Increment time fragment
    timePoint += timeInc;


    if (p->m_particleType == CParticle::PARTICLE_EMITTER)
    {
        // If the particle is an emitter, the position on the emitter side must also be initialised
        // Note, that position of the emitter becomes a position in worldspace if mLocalSpace is set 
        // to false (will this become a problem?)
        CParticleEmitter* pParticleEmitter = static_cast<CParticleEmitter*>(p);
        pParticleEmitter->SetPosition(p->m_position);
    }

    UpdateBoundingBox();

    return p;
}

void ManualParticleSystem::OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    if (m_pParticleSystemResource->GetParticleSystemType() == ePST_QUAD)
    {
        Renderable::OnUpdateToRenderQueue(pCamera, pQueue);
    }
    else if (m_pParticleSystemResource->GetParticleSystemType() == ePST_OBJ)
    {
        float fTimeElapse = ENGINE_INST->GetTimeSinceLastFrame();

        //�жϵ�ǰ������Ƿ����
        Expire(fTimeElapse);

        // ����Ӱ����
        TriggerAffectors(fTimeElapse);

        // �������ӵ�λ��
        ApplyMotion(fTimeElapse);

        // ����������β
        UpdateTrail( fTimeElapse);

        // �������Ӱ�Χ��
        UpdateBoundingBox();

        PARTICLE_LIST_ITER itEnd = m_lstActiveParticles.end();
        for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != itEnd; ++it)
        {
            (*it)->CaculateRenderableTransform();
            Renderable* pRenderable = (*it)->GetRenderable();
            if (pRenderable)
            {
                pRenderable->UpdateToRenderQueue(NULL, pQueue);
            }
        }
    }
}
