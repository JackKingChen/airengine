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
    // 如果没有父场景节点,就立刻返回
    if ( !m_pNode )
    {
        return;
    }

    //计算粒子系统渲染时间流逝
    m_fRenderTimeElapsed += fTimeElapse;

    //计算粒子系统包围盒更新时间长度
    m_fBoundingBoxUpdateElapse += fTimeElapse;

    //如果是自动脱离并且当前停止发射粒子和粒子数为0
    if ( m_bAutoDetach && m_bPauseEmitParticle && m_lstFreeParticles.size() == m_vtParticlePool.size() )
    {
        m_pNode->DetachRenderable( this );
        this->SetAttachedNode( NULL );
        return;
    }

    //u32 dwTimeStart = timeGetTime();
    //配置粒子池
    Configure();

    // 设置拖尾
    SetupRibbonTrail();

    // 初始化被发射发射器
    //InitializeEmittedEmitters();

    // 查看是否需要更新缓冲数据
    //NotifyRenderData();

    //初始化
    if (!m_bFirstTimeInit)
    {
        m_pNode->NeedUpdate();
        m_pNode->Update();
        float fUpdateRemain = 1.0f;
        while (fUpdateRemain >= 0.1f)
        {
            //判断当前活动粒子是否过期
            Expire(0.1f);

            //触发影响器
            TriggerAffectors(0.1f);

            //计算粒子的位置
            ApplyMotion(0.1f);

            //触发发射器
            TriggerEmitters(0.1f);

            //更新拖尾
            UpdateTrail( 0.1f );
            fUpdateRemain -= 0.1f;
        }
        //计算boundingbox
        m_fBoundingBoxUpdateElapse = m_fBoundingBoxUpdateTime + 0.1f;
        UpdateBoundingBox();
        m_bFirstTimeInit = true;
    }

    Renderable::UpdateNodeAnimation(fTimeElapse);
}

void ManualParticleSystem::PreRender(CCamera* pCamera)
{
    // 如果没有父场景节点,就立刻返回
    //判断当前活动粒子是否过期
    Expire(ENGINE_INST->GetTimeSinceLastFrame());

    // 触发影响器
    TriggerAffectors(ENGINE_INST->GetTimeSinceLastFrame());

    // 计算粒子的位置
    ApplyMotion(ENGINE_INST->GetTimeSinceLastFrame());

    // 触发发射器
    //TriggerEmitters(ENGINE_INST->GetTimeSinceLastFrame());

    // 更新粒子拖尾
    UpdateTrail( ENGINE_INST->GetTimeSinceLastFrame() );

    // 计算粒子包围盒
    UpdateBoundingBox();

    switch ( m_pParticleSystemResource->GetParticleSystemType() )
    {
    case ePST_NONE:
        break;
    case ePST_QUAD:
        //填充VB和IB
        UpdateBuffers(pCamera);
        break;
    case ePST_OBJ:
        break;
    }
}

void ManualParticleSystem::EmitParticles(int nParticlesNum)
{
    //当前能产生的最大粒子数
    size_t emissionAllowed = m_lstFreeParticles.size();  
    //代表该时刻产生的粒子总数
    size_t totalRequested = 0;   
    size_t i = 0;
    //发射器总数量
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

    //创建一个粒子,实际上是从空闲粒子序列中取一个出来
    CParticle* p = NULL;
    if (strEmitter == CStringUtil::BLANK)
    {
        p = CreateParticle();
    }
    else
    {
        //p = CreateEmitterParticle(strEmitter);
    }

    //如果创建粒子失败,就立刻返回
    if (!p)
        return NULL;

    //设置粒子的宽度高度以及旋转角度
    float scale = pEmitter->GetScale();
    p->m_fWidth = p->m_fInitWidth = m_pParticleSystemResource->GetDefaultWidth() * scale;
    p->m_fHeight = p->m_fInitHeight = m_pParticleSystemResource->GetDefaultHeight() * scale;
    p->m_fRotationAngle = 0;

    //用发射器对粒子进行初始化
    pEmitter->InitParitcle(p);

    if (m_pTrail)
    {
        SetTrackParticle(p);
        //m_pTrail->SetTrackParticle(p);
    }


    ////////变换粒子的位置
    //////matWorld.TransformCoord(p->m_position, p->m_position);
    ////////变换粒子的运动方向
    //////matWorld.TransformNormal(p->m_direction);

    //遍历所有的影响器,对粒子进行初始化
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

    //更新例子位置
    if (m_pNode)
    {
        const Matrix4f& matWorld = m_pNode->GetFullTransform();
        //变换粒子的位置
        matWorld.TransformCoord(p->m_position, p->m_position);
        //变换粒子的运动方向
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

        //判断当前活动粒子是否过期
        Expire(fTimeElapse);

        // 触发影响器
        TriggerAffectors(fTimeElapse);

        // 计算粒子的位置
        ApplyMotion(fTimeElapse);

        // 更新粒子拖尾
        UpdateTrail( fTimeElapse);

        // 计算粒子包围盒
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
