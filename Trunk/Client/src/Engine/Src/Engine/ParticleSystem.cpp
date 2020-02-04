#include "stdafx.h"
#include "ParticleSystem.h"
#include "Util.h"
#include "VertexDef.h"
#include "FileSystem.h"
#include "StringUtil.h"
#include "PointEmitter.h"
#include "BoxEmitter.h"
#include "ParticleAffectorFactory.h"
#include "ParticleEmitterFactory.h"
#include "MaterialManager.h"
#include "SceneNode.h"
#include "Util.h"
#include "BillboardSetManager.h"
#include "ISceneManager.h"
#include "ModelInstance.h"
#include "XMLFileData.h"
#include "Primitive.h"
//#include "TextureUnit.h"
#include "ParticleSystemResource.h"
#include "ParticleSystemManager.h"
//#include "Technique.h"
#include "EngineStringConverter.h"

#undef max

BillboardSet* CParticleSystem::m_pBillboardSet = NULL;

CParticleSystem::CParticleSystem() : 
//m_pBillboardSet(NULL),
m_fVisableTime(0), 
//m_nPoolSize(0), 
//m_nEmittedEmitterPoolSize(0),
//m_fDefaultWidth(0),
//m_fDefaultHeight(0), 
m_bConfigured(false), 
m_bBoundsAutoUpdate(false), 
//m_billboardType(BBT_POINT), 
//m_bCullBackFace(true),
//m_vCommonDir(Vector3Df::UNIT_Y),
//m_vCommonUpDir(Vector3Df::UNIT_Z),
m_bInitializeEmittedEmitters(false),
m_bNeedCreateRenderData(false),
m_bCreateTrail(false),
//m_nTrailSegmentCount(0),
//m_nMaxChainElement(20),
m_pTrail(NULL),
//m_fInitTrailWidth(10.0f),
//m_fTrailWidthChange(-1.0f),
m_fBoundingBoxUpdateElapse(0.0f),
m_fBoundingBoxUpdateTime(1.0f),
//m_bTrailUseVertexColor(true),
//m_bTrailUseTexCoord(true),
m_fRenderTimeElapsed(0),
m_bFirstTimeInit(false)
//m_szTrailTex(""),
//m_TCD(BillBoardChain::TexCoordDirection::TCD_U)
, m_bParticleSystemUpdated(false)
, m_fEmitEndTime(0)
, m_bPauseEmitParticle(false)
, m_bAutoDetach(false)
//, m_szBlendMode("BLENDOP_ADD")
//, m_szSrcBlend("BLEND_SRCALPHA")
//, m_szDestBlend("BLEND_INVSRCALPHA")
//, m_pRenderableObjectNode(NULL)
//, m_pRenderableObject(NULL)
//, m_ParticleSystemType(ePST_QUAD)
, m_pParticleSystemResource(NULL)
, m_bLocalSpace(false)
{
    m_szType = "particlesystem";
    m_rcTextureCoord.left = 0.0f;
    m_rcTextureCoord.top = 0.0f;
    m_rcTextureCoord.right = 1.0f;
    m_rcTextureCoord.bottom = 1.0f;
    //m_renderableType = RT_ParticleSystem;
    /*
    m_bPauseEmitParticle = false;
	m_bAutoDetach = false;
    //m_vCommonDir.Set( 0.0f, 0.0f, 1.0f );
    //m_vCommonUpDir.Set( 0.0f, 1.0f, 0.0f );
	m_szBlendMode = "BLENDOP_ADD";
	m_szSrcBlend = "BLEND_SRCALPHA";
	m_szDestBlend = "BLEND_INVSRCALPHA";
	m_pRenderableObjectNode = NULL;
	m_pRenderableObject = NULL;

	m_ParticleSystemType = ePST_QUAD;
    */


	if (m_pBillboardSet == NULL)
	{	
		int nBillboardsNum = 1000;
		m_pBillboardSet = BillboardSetManager::GetInstance()->CreateBillboardSet(nBillboardsNum);
	}

	m_strVertexDecl = "E_VB_STANDARD";
}

CParticleSystem::~CParticleSystem()
{
    Clear();
}

/*
void CParticleSystem::RemoveAllEmittedEmitters()
{
    //先将被发射的发射器的粒子中lstActiveParticles中取出来
    CParticle* pParticle = NULL;
    CParticleEmitter* pParticleEmitter = NULL;
    for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end();)
    {
        pParticle = *it;
        if (pParticle->m_particleType == CParticle::PARTICLE_EMITTER)
        {
            // For now, it can only be an emitted emitter
            pParticleEmitter = static_cast<CParticleEmitter*>(*it);
            std::list<CParticleEmitter*>* fee = FindFreeEmittedEmitter(pParticleEmitter->GetName());
            fee->push_back(pParticleEmitter);

            // Also erase from mActiveEmittedEmitters
            RemoveFromActiveEmittedEmitters (pParticleEmitter);

            // And erase from mActiveParticles
            it = m_lstActiveParticles.erase( it );

            if (pParticle->m_bTrail)
            {
                if (m_pTrail)
                {
                    NotifyTrackParticleDestroy(pParticle);
                    //m_pTrail->NotifyParticleDestory(pParticle);
                }
            } 
            
        }
        else
        {
            ++it;
        }
    }

    EMITTER_POOL_ITER emittedEmitterPoolIterator;
    VECTOR_EMITTER_ITER emittedEmitterListIterator;
    VECTOR_EMITTER* e = 0;
    for (emittedEmitterPoolIterator = m_emittedEmitterPool.begin(); emittedEmitterPoolIterator != m_emittedEmitterPool.end(); ++emittedEmitterPoolIterator)
    {
        e = &emittedEmitterPoolIterator->second;
        for (emittedEmitterListIterator = e->begin(); emittedEmitterListIterator != e->end(); ++emittedEmitterListIterator)
        {
            SafeDelete( *emittedEmitterListIterator );
            //ParticleSystemManager::getSingleton()._destroyEmitter(*emittedEmitterListIterator);
        }
        e->clear();
    }

    // Don磘 leave any references behind
    m_emittedEmitterPool.clear();
    m_lstFreeEmittedEmitters.clear();
    m_lstActiveEmitters.clear();
}
*/

void CParticleSystem::Reset( void )
{
	//重置粒子系统的拖尾对象
    m_colorDiffuse = 0xffffffff;
	ResetTrail();

	//回收所有激活的粒子并放到可用粒子序列
	CParticle* pParticle = NULL;
	CParticleEmitter* pParticleEmitter = NULL;
	for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end();)
	{
		pParticle = *it;
		if (pParticle->m_particleType == CParticle::PARTICLE_NORMAL)
		{
			m_lstFreeParticles.splice(m_lstFreeParticles.end(), m_lstActiveParticles, it++);
		}
		else
		{
            /*
			pParticleEmitter = static_cast<CParticleEmitter*>(*it);
			std::list<CParticleEmitter*>* fee = FindFreeEmittedEmitter(pParticleEmitter->GetName());
			fee->push_back(pParticleEmitter);
			RemoveFromActiveEmittedEmitters (pParticleEmitter);
			it = m_lstActiveParticles.erase( it );
            */
		}
	}

	//重置所有的粒子发射器
	CParticleEmitter* pEmitter = NULL;
	for (VECTOR_EMITTER_ITER it = m_vtParticleEmitter.begin(); it != m_vtParticleEmitter.end(); ++it)
	{
		pEmitter = *it;
		pEmitter->Reset();
	}

	//重置节点动画
	this->ResetNodeAnimation();

	m_bFirstTimeInit = false;
}

void CParticleSystem::ClearRenderableImp()
{
    //销毁发射器
    for (VECTOR_EMITTER_ITER it = m_vtParticleEmitter.begin(); it != m_vtParticleEmitter.end(); it++)
    {
        CParticleEmitter* pEmitter = *it;
        SafeDelete(pEmitter);
    }
    m_vtParticleEmitter.clear();

    //销毁影响器
    for (VECTOR_AFFECTOR_ITER it = m_vtParticleAffector.begin(); it != m_vtParticleAffector.end(); ++it)
    {
        CParticleAffector* pAffector = *it;
        SafeDelete(pAffector);
    }
    m_vtParticleAffector.clear();
    

    //销毁粒子池
    for (VECTOR_PARTICLE_ITER it = m_vtParticlePool.begin(); it != m_vtParticlePool.end(); it++)
    {
        CParticle* pParticle = *it;
        SafeDelete(pParticle);
    }
    m_vtParticlePool.clear();

	//AddActiveEmittedEmittesToFreeList();

    m_lstActiveParticles.clear();
    m_lstFreeParticles.clear();

    //RemoveAllEmittedEmitters();

    if (m_pTrail)
    {
        m_pTrail->Clear();
        SafeDelete(m_pTrail);
    }

    //销毁粒子材质
    //if (m_pMaterial)
    //{
    //    MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    //}

    //销毁粒子系统的顶点,索引缓冲
    /*
    if (m_pBillboardSet)
    {
        BILLBOARDSET_MGR->ClearBillboardSet(m_pBillboardSet);
		m_pBillboardSet = NULL;
    }
    */
    if (m_pParticleSystemResource)
    {
        RESOURCE_MANAGER->ClearResource(m_pParticleSystemResource);
        m_pParticleSystemResource = NULL;
    }
    
}

bool CParticleSystem::LoadFromFile(const char* szFilename)
{
    if (m_pParticleSystemResource == NULL)
    {
        //m_pParticleSystemResource = static_cast<ParticleSystemResource*>(RESOURCE_MANAGER->CreateResource(Resource::Res_ParticleSystem, szFilename));
        m_pParticleSystemResource = RESOURCE_MANAGER->CreateResource<ParticleSystemResource>(szFilename);

        //m_pParticleSystemResource->LoadFromFile(szFilename, ENGINE_INST->GetConfig()->m_bBackgroundThread);
        if (m_pParticleSystemResource && m_pParticleSystemResource->IsLoaded())
        {
            if (!m_pParticleSystemResource->IsReady())
            {
                if (m_pParticleSystemResource->IsLoaded())
                    m_pParticleSystemResource->Cache();
            }

            if (m_pParticleSystemResource->IsReady())
            {
                m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Particle");//m_pParticleSystemResource->GetMaterial();
                //m_bShareParentMaterial = true;
            }
        }
        m_strResourceName = CStringUtil::GetFilename(szFilename);
    }

    return m_pParticleSystemResource != NULL;
}

/*
void CParticleSystem::Render(CCamera* pCamera)
{

    //渲染前的处理
    PreRender(pCamera);
    
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	//粒子是一个四边面
	if ( m_ParticleSystemType == ePST_QUAD )
	{
		//if ( m_pRenderableObjectNode )
		//{
		//	m_pRenderableObjectNode->DetachAllRenderable();
		//}

		//渲染
		int nVertexCount = m_lstActiveParticles.size() * 4;
		int nFaceCount = m_lstActiveParticles.size() * 2;

		if (nVertexCount > 0)
		{
			pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
			if (m_pMaterial)
			{
				CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
				if (pTechnique)
				{
					pRenderer->SetVertexBuffer(0, m_pBillboardSet->GetVertexBuffer());
					pRenderer->SetIndexBuffer(m_pBillboardSet->GetIndexBuffer());
					size_t nPassNum = pTechnique->GetPassesNum();
					for (size_t i = 0; i < nPassNum; ++i)
					{
						CPass* pPass = pTechnique->GetPass(i);
						pPass->ApplyPass(pRenderer);

						pRenderer->DrawTriangleListByIndex(0, 0, nVertexCount, 0, nFaceCount);
					}
				}
			}
		}

	}
    

	//渲染粒子的拖尾
    if (m_pTrail)
    {
        m_pTrail->Render(pCamera);
    }

	Renderable::AfterRender();

    //m_fRenderTimeElapsed = 0;
}
*/

void CParticleSystem::Update(float fTimeElapse)
{
    // 如果没有父场景节点,就立刻返回
    if (!m_pParticleSystemResource || !m_pParticleSystemResource->IsReady())
    {
        if (m_pParticleSystemResource && m_pParticleSystemResource->IsLoaded())
        {
            m_pParticleSystemResource->Cache();
        }
        //return;
    }
    if (!m_bPaused && m_pParticleSystemResource->IsReady())
    {
        if (!m_bPrepared)
        {
            Prepare();
        }
        else
        {
            //计算粒子系统渲染时间流逝
            m_fRenderTimeElapsed += fTimeElapse;

            //计算粒子系统包围盒更新时间长度
            m_fBoundingBoxUpdateElapse += fTimeElapse;

            //如果是自动脱离并且当前停止发射粒子和粒子数为0
            /* 由liangairan注释掉，因为这样做会让初始发射的时候自动退出
            if ( m_bAutoDetach && m_bPauseEmitParticle && m_lstFreeParticles.size() == m_vtParticlePool.size() )
            {
            m_pNode->DetachRenderable( this );
            this->SetAttachedNode( NULL );
            return;
            }
            */

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
                
                m_bFirstTimeInit = true;
            }

            UpdateBoundingBox();
        }
        
        /*
        Expire(fTimeElapse);

        //触发影响器
        TriggerAffectors(fTimeElapse);

        //计算粒子的位置
        ApplyMotion(fTimeElapse);

        //触发发射器
        TriggerEmitters(fTimeElapse);

        UpdateTrail( fTimeElapse );
        //计算boundingbox
        UpdateBoundingBox();
        */

        //u32 dwTimeDelta = timeGetTime() - dwTimeStart;
        //char szDebug[256] = { 0 };
        //sprintf(szDebug, "CParticleSystem::Update use time : %d\n", dwTimeDelta);
        //OutputDebugString(szDebug);

        //-----------------------------------
        //! add by hjj 2010-5-24
        Renderable::UpdateNodeAnimation(fTimeElapse);
        //-----------------------------------

        
    }
	
	m_bParticleSystemUpdated = false;

    if (IsCompleted() && m_bAutoDetach && m_pNode)
    {
        m_pNode->DetachRenderable( this );
    }
}

void CParticleSystem::UpdateTrail( float fFrameTime )
{
	if ( !m_pTrail )
	{
		return;
	}
	CParticleIterator particleIter(m_lstTrailParticle.begin(), m_lstTrailParticle.end());
	m_pTrail->UpdateByParticle( fFrameTime, particleIter );
}

void CParticleSystem::PreRender(CCamera* pCamera)
{
	if ( m_bParticleSystemUpdated == true )
	{
		return;
	}

    if (!m_bPaused)
    {
        if (m_pBillboardSet == NULL)
        {
            //m_pBillboardSet = new BillboardSet;			
            int nBillboardsNum = m_pParticleSystemResource->GetQuota() + m_pParticleSystemResource->GetEmittedEmitterSize();
            nBillboardsNum = nBillboardsNum > 1000 ? nBillboardsNum : 1000;
			m_pBillboardSet = BillboardSetManager::GetInstance()->CreateBillboardSet(nBillboardsNum);
            //m_pBillboardSet->Create(nBillboardsNum);  //默认是1000个particle
        }
        else
        {
            int nBillboardsNum = m_pParticleSystemResource->GetQuota() + m_pParticleSystemResource->GetEmittedEmitterSize();
            if (nBillboardsNum > m_pBillboardSet->GetBillboardsNum())
            {
                m_pBillboardSet->SetBillboardsNum(nBillboardsNum);
            }            
        }
        float fTimeElapse = ENGINE_INST->GetTimeSinceLastFrame();

        //判断当前活动粒子是否过期
        Expire(fTimeElapse);

        // 触发影响器
        TriggerAffectors(fTimeElapse);

        // 计算粒子的位置
        ApplyMotion(fTimeElapse);

        // 触发发射器
        if (m_fEmitEndTime > 0)
        {
            if (m_fRenderTimeElapsed <= m_fEmitEndTime)
            {
                TriggerEmitters(fTimeElapse);
            }

        }
        else
        {
            TriggerEmitters(fTimeElapse);
        }

        // 更新粒子拖尾
        UpdateTrail(fTimeElapse);

        // 计算粒子包围盒
        //UpdateBoundingBox();

        switch ( m_pParticleSystemResource->GetParticleSystemType() )
        {
        case ePST_NONE:
            break;
        case ePST_QUAD:
            //填充VB和IB
            UpdateBuffers(pCamera);
            break;
        case ePST_OBJ:
            //{
            //	m_pRenderableObjectNode->AttachRenderable( m_pRenderableObject );
            //	if ( m_pRenderableObject->GetTypeString() == "model" )
            //	{
            //		CModelInstance* pModel = (CModelInstance*)m_pRenderableObject;
            //		pModel->SetInstancingEnable( true );
            //	}
            //	CParticle* pParticle = NULL;
            //	for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end(); ++it)
            //	{
            //		pParticle = *it;
            //		m_pRenderableObjectNode->SetPosition( pParticle->m_position );
            //		m_pRenderableObjectNode->NeedUpdate();
            //		m_pRenderableObjectNode->Update();
            //	}
            //}
            break;
        }
    }

	m_bParticleSystemUpdated = true;
}


//// 更新到结点
//void CParticleSystem::UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
//{
//	Renderable::UpdateToRenderQueue(pCamera, pQueue);
//
//
//	pQueue->AddRenderable( this );
//}

/*
void CParticleSystem::SetDefaultHeight(float fHeight)
{
    m_fDefaultHeight = fHeight;
}

void CParticleSystem::SetDefaultWidth(float fWidth)
{
    m_fDefaultWidth = fWidth;
}

void CParticleSystem::SetQuota(int nQuota)
{
    m_nPoolSize = nQuota;
}
*/
void CParticleSystem::IncreasePool(size_t size)
{
	//旧粒子池的大小
    size_t oldSize = m_vtParticlePool.size();

    //重置粒子池的大小
    m_vtParticlePool.reserve(size);
    m_vtParticlePool.resize(size);

    //创建粒子对象
    char szName[256] = { 0 };
    for( size_t i = oldSize; i < size; i++ )
    {
        m_vtParticlePool[i] = new CParticle;
        m_vtParticlePool[i]->m_fWidth = m_pParticleSystemResource->m_fDefaultWidth;
        m_vtParticlePool[i]->m_fHeight = m_pParticleSystemResource->m_fDefaultHeight;

        if (m_pParticleSystemResource->m_ParticleSystemType == ePST_OBJ)
        {
            sprintf(szName, "%s_%d", m_strName.c_str(), i);
            m_vtParticlePool[i]->CreateRenderable(szName, m_pParticleSystemResource->m_szParticleRenderableResource.c_str());
        }
    }
}

void CParticleSystem::DecreasePool(size_t size)
{
    m_lstActiveParticles.clear();
    m_lstFreeParticles.clear();
    m_lstTrailParticle.clear();

    size_t oldsize = m_vtParticlePool.size();
    for (size_t i = size; i < oldsize; ++i)
    {
        SafeDelete(m_vtParticlePool[i]);
    }

    m_vtParticlePool.reserve(size);
    m_vtParticlePool.resize(size);
}

void CParticleSystem::Configure()
{
    //获得粒子池的大小
    size_t nCurSize = m_vtParticlePool.size();
    //如果当前粒子池的大小小于最大值
    if (nCurSize < m_pParticleSystemResource->GetQuota())
    {
        //调整池的大小
        IncreasePool(m_pParticleSystemResource->GetQuota());

        for (u32 i = nCurSize; i < m_pParticleSystemResource->GetQuota(); ++i)
        {
            m_lstFreeParticles.push_back(m_vtParticlePool[i]);
        }

        m_bNeedCreateRenderData = true;
        //NotifyRenderData();
    }
    else if (nCurSize > m_pParticleSystemResource->GetQuota())
    {
        DecreasePool(m_pParticleSystemResource->GetQuota());

        for (u32 i = 0; i < m_pParticleSystemResource->GetQuota(); ++i)
        {
            m_lstFreeParticles.push_back(m_vtParticlePool[i]);
        }

        m_bNeedCreateRenderData = true;
    }

}
/*
void CParticleSystem::NotifyRenderData()
{
	//如果需要重建渲染数据
    if (m_bNeedCreateRenderData)
    {
		//销毁原来的渲染数据
        DestoryBuffers();
		//创建渲染缓冲
        CreateBuffers();
        m_bNeedCreateRenderData = false;
    }
}

bool CParticleSystem::CreateBuffers()
{
    DestoryBuffers();

    if (m_pBillboardSet == NULL)
    {
        m_pBillboardSet = BILLBOARDSET_MGR->CreateBillboardSet(m_vtParticlePool.size() * 4 + m_nEmittedEmitterPoolSize * 4);
    }

    return true;
}

void CParticleSystem::DestoryBuffers()
{
    
    if (m_pBillboardSet)
    {
        BILLBOARDSET_MGR->ClearBillboardSet(m_pBillboardSet);
		m_pBillboardSet = NULL;
    }
    
}
*/
void CParticleSystem::Expire(float fTimeElapsed)
{
    //遍历所有激活的粒子
    CParticle* pParticle = NULL;
	CParticleEmitter* pParticleEmitter = NULL;
    for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end();)
    {
        //取出粒子对象
        pParticle = *it;

		//如果粒子剩余的生命时间小于每帧渲染的时间长度
		if (pParticle->m_fLifeTime < fTimeElapsed)   
        {
			//如果是普通粒子,就讲他从激活的粒子列表移到可用粒子列表
			if (pParticle->m_particleType == CParticle::PARTICLE_NORMAL)
			{
				m_lstFreeParticles.splice(m_lstFreeParticles.end(), m_lstActiveParticles, it++);
			}
			//如果是带发射器的粒子
			else
			{
                /*
				pParticleEmitter = static_cast<CParticleEmitter*>(*it);
				std::list<CParticleEmitter*>* fee = FindFreeEmittedEmitter(pParticleEmitter->GetName());
				fee->push_back(pParticleEmitter);

				// Also erase from mActiveEmittedEmitters
				RemoveFromActiveEmittedEmitters (pParticleEmitter);

				// And erase from mActiveParticles
				it = m_lstActiveParticles.erase( it );
                */
			}

            if (pParticle->m_bTrail)
            {
                if (m_pTrail)
                {
                    NotifyTrackParticleDestroy(pParticle);
                    //m_pTrail->NotifyParticleDestory(pParticle);
                }
            }
        }
        else
        {
            //粒子的生命值减去帧时间
            pParticle->m_fLifeTime -= fTimeElapsed;
            ++it;
        }
    }

}

void CParticleSystem::TriggerEmitters(float fTimeElapsed)
{
    static AIRVector<unsigned> requested;

    //如果当前暂停发射,就立刻返回
    if ( m_bPauseEmitParticle )
    {
        return;
    }

    //调整请求的发射器数
    if( requested.size() != /*m_pParticleSystemResource->*/m_vtParticleEmitter.size() )
        requested.resize( /*m_pParticleSystemResource->*/m_vtParticleEmitter.size() );

    //当前能产生的最大粒子数
    size_t emissionAllowed = m_lstFreeParticles.size();  
    //代表该时刻产生的粒子总数
    size_t totalRequested = 0;   
    size_t i = 0;
    //发射器总数量
    size_t emitterCount = /*m_pParticleSystemResource->*/m_vtParticleEmitter.size();

     CParticleEmitter* pEmitter = NULL;
    //遍历所有的发射器
     for (VECTOR_EMITTER_ITER it = /*m_pParticleSystemResource->*/m_vtParticleEmitter.begin(); it != /*m_pParticleSystemResource->*/m_vtParticleEmitter.end(); ++it)
    {
        pEmitter = *it;
        //获得当前第I个发射器发射出来的粒子数
        if (!pEmitter->IsEmitted())
        {
            requested[i] = pEmitter->GenParticleCount(fTimeElapsed);
            //计算发射的粒子总数
            totalRequested += requested[i];
            ++i;
        }
    }

	//遍历所有激活的发射器粒子  被发射的发射器全部屏蔽掉
    /*
    for (LIST_EMITTER_ITER it = m_lstActiveEmitters.begin(); it != m_lstActiveEmitters.end(); ++it)
    {
        pEmitter = *it;
        //计算发射的粒子总数
        if (pEmitter->IsDelayEmitComplete())
        {
            totalRequested += pEmitter->GenParticleCount(fTimeElapsed);
        }
    }
    */

    float fRatio = 1.0f;
    //如果当前产生的粒子总数大于当前能产生的最大粒子数
    if (totalRequested > emissionAllowed)
    {
        //计算每个发射器有效粒子的比率
        fRatio =  (float)emissionAllowed / (float)totalRequested;
        //遍历每个发射器
        for (i = 0; i < emitterCount; ++i)
        {
            //计算每个发射器的实际产生粒子数
            requested[i] = static_cast<unsigned>(requested[i] * fRatio);
        }
    }

    AIRVector<unsigned>::iterator qit = requested.begin();

    //遍历所有的发射器
    for (VECTOR_EMITTER_ITER it = /*m_pParticleSystemResource->*/m_vtParticleEmitter.begin(); it != /*m_pParticleSystemResource->*/m_vtParticleEmitter.end() && qit != requested.end(); ++it, ++qit )
    {
        //执行发射器触发
        pEmitter = *it;
        if (!pEmitter->IsEmitted())
        {
            ExcuteTriggerEmitter(pEmitter, (*qit), fTimeElapsed);
        }
    }

    /*   屏蔽了被发射的发射器
    for (LIST_EMITTER_ITER it = m_lstActiveEmitters.begin(); it != m_lstActiveEmitters.end(); ++it)
    {
        pEmitter = *it;
        if (pEmitter->IsDelayEmitComplete())
        {
            unsigned particleCount = static_cast<unsigned>(pEmitter->GenParticleCount(fTimeElapsed) * fRatio);
            ExcuteTriggerEmitter(pEmitter, particleCount, fTimeElapsed);
        }
    }
    */

    //if (m_lstActiveParticles.size() * 4 > m_pVB->GetVertexCount())
    //{
    //    int a = 0;
    //}
}

CParticle* CParticleSystem::CreateParticle()
{
    //从空闲的粒子序列中取开始的粒子,并加到激活的粒子序列中
    CParticle* p = 0;
    if (!m_lstFreeParticles.empty())
    {
        p = m_lstFreeParticles.front();
        m_lstActiveParticles.splice(m_lstActiveParticles.end(), m_lstFreeParticles, m_lstFreeParticles.begin());
    }
    return p;
}

/*
CParticle* CParticleSystem::CreateEmitterParticle(const AIRString& strEmittedEmitter)
{
	// Get the appropriate list and retrieve an emitter	
	CParticle* p = 0;
	std::list<CParticleEmitter*>* fee = FindFreeEmittedEmitter(strEmittedEmitter);
	if (fee && !fee->empty())
	{
		p = fee->front();
		p->m_particleType = CParticle::PARTICLE_EMITTER;
		fee->pop_front();
		m_lstActiveParticles.push_back(p);

		// Also add to mActiveEmittedEmitters. This is needed to traverse through all active emitters
		// that are emitted. Don't use mActiveParticles for that (although they are added to
		// mActiveParticles also), because it would take too long to traverse.
		m_lstActiveEmitters.push_back(static_cast<CParticleEmitter*>(p));

		//p->_notifyOwner(this);
	}

	return p;
}
*/

void CParticleSystem::ExcuteTriggerEmitter(CParticleEmitter* pEmitter, u32 nParticleCount, float fTimeElapse)
{

    float timePoint = 0.0f;
    float timeInc = fTimeElapse / nParticleCount;

    ////////获得父节点相对世界空间的转换
    //////const Matrix4f& matWorld = m_pNode->GetFullTransform();
	const AIRString& strEmitter = pEmitter->GetEmittedEmitterName();

    //遍历所有的粒子
    for (unsigned int j = 0; j < nParticleCount; ++j)
    {
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
            return;

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
        
        //更新例子位置
        if (m_pNode)
        {
            const Matrix4f& matWorld = m_pNode->GetFullTransform();
            //变换粒子的位置
            matWorld.TransformCoord(p->m_position, p->m_position);
            //变换粒子的运动方向
            if (!m_bLocalSpace)
            {
                matWorld.TransformNormal(p->m_direction);
            }
            
        }
        p->m_position += (p->m_direction * timePoint);

        // apply particle initialization by the affectors 影响器代码，到时要补加上去
        //itAffEnd = mAffectors.end();
        //for (itAff = mAffectors.begin(); itAff != itAffEnd; ++itAff)
        //    (*itAff)->_initParticle(p);
        //遍历所有的影响器,对粒子进行初始化
        for (VECTOR_AFFECTOR_ITER itAff = /*m_pParticleSystemResource->*/m_vtParticleAffector.begin(); itAff != /*m_pParticleSystemResource->*/m_vtParticleAffector.end(); ++itAff)
        {
            (*itAff)->InitParticle(p);
        }

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

        // Notify renderer
        //mRenderer->_notifyParticleEmitted(p);
    }
}

void CParticleSystem::UpdateBoundingBox()
{
    if (m_pNode && m_fBoundingBoxUpdateElapse > m_fBoundingBoxUpdateTime)
    {
        if (m_lstActiveParticles.empty())
        {
            //m_boundingBox.MinEdge = Vector3Df::ZERO;
            //m_boundingBox.MaxEdge = Vector3Df::ZERO;

            m_WorldAABB = m_pNode->GetWorldBoundingBox();
        }
        else
        {
            Vector3Df vMax(-999999, -999999, -999999);
            Vector3Df vMin(999999, 999999, 999999);

            Vector3Df halfScale = Vector3Df::UNIT_SCALE * 0.5;
            Vector3Df defaultPadding = 
                halfScale * std::max(m_pParticleSystemResource->m_fDefaultHeight, m_pParticleSystemResource->m_fDefaultWidth);

            for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end(); ++it)
            {
                //if ((*it)->mOwnDimensions)
                //{
                //    Vector3Df padding = 
                //        halfScale * std::max((*it)->m_fWidth, (*it)->m_fHeight);
                //    vMin.MakeFloor((*it)->m_position - padding);
                //    vMax.MakeCeil((*it)->m_position + padding);
                //}
                //else
                {
                    vMin.MakeFloor((*it)->m_position - defaultPadding);
                    vMax.MakeCeil((*it)->m_position + defaultPadding);
                }
            }
            m_boundingBox.MinEdge = vMin;
            m_boundingBox.MaxEdge = vMax;
            m_WorldAABB.Reset(m_boundingBox);
        }

        
    
        m_pNode->NeedUpdate();
        m_pNode->Update();      //这里很奇怪，会引起视觉上的错误

        m_bBoundsAutoUpdate = false;

        m_fBoundingBoxUpdateElapse = 0.0f;

        m_vtCenter = m_WorldAABB.GetCenter();
    }
}

//更新顶点缓冲和索引缓冲
void CParticleSystem::UpdateBuffers(CCamera* pCamera)
{
    UpdateVertices(pCamera);
}

void CParticleSystem::ApplyMotion(float fTimeElapsed)
{
    //遍历所有激活的粒子
    CParticle* pParticle = NULL;
	CParticleEmitter* pParticleEmitter = NULL;
    for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end(); ++it)
    {
        //计算粒子的位置
        pParticle = *it;
        pParticle->m_position += pParticle->m_direction * fTimeElapsed;

        //OutputDebugStr(GetString("particle position:(%f, %f, %f)\n", pParticle->m_position.x, pParticle->m_position.y, pParticle->m_position.z));

		if (pParticle->m_particleType == CParticle::PARTICLE_EMITTER)
		{
			pParticleEmitter = static_cast<CParticleEmitter*>(pParticle);
			pParticleEmitter->SetPosition(pParticle->m_position);
		}
    }
}

void CParticleSystem::UpdateParticleVertices( CParticle* pParticle, VERTEX_STANDARD** ppVertices )
{
    VERTEX_STANDARD* pVertices = *ppVertices;

    SColor diffuse = m_colorDiffuse * SColor(pParticle->m_color.GetAsARGB());

    if (pParticle->m_fRotationAngle == 0)   //没有旋转
    {
        //左上角
        pVertices->position = m_vtOffset[0] + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.left;
        pVertices->tv = m_rcTextureCoord.top;

        //右上角
        ++pVertices;
        pVertices->position = m_vtOffset[1] + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.right;
        pVertices->tv = m_rcTextureCoord.top;

        //左下角
        ++pVertices;
        pVertices->position = m_vtOffset[2] + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.left;
        pVertices->tv = m_rcTextureCoord.bottom;

        //右下角
        ++pVertices;
        pVertices->position = m_vtOffset[3] + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.right;
        pVertices->tv = m_rcTextureCoord.bottom;

        ++pVertices;
    }
    else //有旋转
    {
        //计算出旋转轴
        Vector3Df vAxis = (m_vtOffset[3] - m_vtOffset[0]).CrossProduct(m_vtOffset[2] - m_vtOffset[1]).Normalize();

        Matrix4f matRotation;
        matRotation.BuildRotationMatrixByAxisRadians(vAxis, pParticle->m_fRotationAngle);

        Vector3Df pt;
        //matRotation.TransformCoord(m_vtOffset[0], pt);
		matRotation.TransformCoordFast(m_vtOffset[0], pt);

        //左上角
        pVertices->position = pt + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.left;
        pVertices->tv = m_rcTextureCoord.top;

        //右上角
        //matRotation.TransformCoord(m_vtOffset[1], pt);
		matRotation.TransformCoordFast(m_vtOffset[1], pt);
        ++pVertices;
        pVertices->position = pt + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.right;
        pVertices->tv = m_rcTextureCoord.top;

        //左下角
        //matRotation.TransformCoord(m_vtOffset[2], pt);
		matRotation.TransformCoordFast(m_vtOffset[2], pt);
        ++pVertices;
        pVertices->position = pt + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.left;
        pVertices->tv = m_rcTextureCoord.bottom;

        //右下角
        //matRotation.TransformCoord(m_vtOffset[3], pt);
		matRotation.TransformCoordFast(m_vtOffset[3], pt);
        ++pVertices;
        pVertices->position = pt + pParticle->m_position;
        pVertices->color = diffuse;
        pVertices->tu = m_rcTextureCoord.right;
        pVertices->tv = m_rcTextureCoord.bottom;

        ++pVertices;
    }

    *ppVertices = pVertices;
}

void CParticleSystem::UpdateVertices(CCamera* pCamera)
{
    if (m_pBillboardSet)
    {
        IVertexBuffer* pVB = m_pBillboardSet->GetVertexBuffer();
        // 获得顶点缓冲
        VERTEX_STANDARD* pVertices = (VERTEX_STANDARD*)pVB->Lock(true);
        CParticle* pParticle = NULL;

        switch ( m_pParticleSystemResource->m_billboardType )
        {
        case BBT_POINT:
        case BBT_HORZ_COMMON_DIR_FACE_CAM:
        case BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR:
        case BBT_VERT_COMMON_DIR_FACE_CAM:
        case BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR:
            {
                // 预生成粒子的右方向和上方向
                GenBillboardDirection(pCamera, NULL);
                for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end(); ++it)
                {
                    pParticle = *it;
                    //计算顶点偏移
                    GenVertexOffset(pParticle);
                    UpdateParticleVertices( pParticle, &pVertices );
                }
            }
            break;

        case BBT_HORZ_SELF_DIR_FACE_CAM:
        case BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR:
        case BBT_VERT_SELF_DIR_FACE_CAM:
        case BBT_VERT_SELF_DIR_FACE_COMMON_UP_DIR:
            {
                for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != m_lstActiveParticles.end(); ++it)
                {
                    pParticle = *it;
                    // 生成粒子的右方向和上方向
                    GenBillboardDirection(pCamera, pParticle);
                    //计算顶点偏移
                    GenVertexOffset(pParticle);
                    UpdateParticleVertices( pParticle, &pVertices );
                }
            }
            break;
        }

        pVB->Unlock();
    }
}

void CParticleSystem::GenBillboardDirection(CCamera* pCamera, CParticle* pParticle)
{
    switch (m_pParticleSystemResource->m_billboardType)
    {
        // 始终面向摄像机
        //    | pCamera->GetUp()
        // ---|--- 
        // |  |  |
        // |  |__|_______ pCamera->GetRight()
        // |     |
        // -------
    case BBT_POINT:
        {
            pCamera->GetRight(m_vCameraRight);
            pCamera->GetUp(m_vCameraUp);
        }
        break;

        // 平行于公共方向(m_vCommonDir)并朝向摄像机
    case BBT_HORZ_COMMON_DIR_FACE_CAM:
        {
            m_vCameraRight = m_pParticleSystemResource->GetCommonDir().CrossProduct( pCamera->GetLookDir() );
            m_vCameraUp = m_pParticleSystemResource->GetCommonDir();
        }
        break;

        // 平行于公共方向(m_vCommonDir)并朝向公共上方向(m_vCommonUpDir)
    case BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR:
        {
            m_vCameraRight = m_pParticleSystemResource->GetCommonDir().CrossProduct( m_pParticleSystemResource->m_vCommonUpDir );
            m_vCameraUp = m_pParticleSystemResource->GetCommonDir();
        }
        break;

        // 垂直于公共方向(m_vCommonDir)并朝向摄像机
    case BBT_VERT_COMMON_DIR_FACE_CAM:
        {
            m_vCameraRight = m_pParticleSystemResource->m_vCommonDir.CrossProduct( pCamera->GetLookDir() );
            m_vCameraUp = m_vCameraRight.CrossProduct( m_pParticleSystemResource->m_vCommonDir );
        }
        break;

        // 垂直于公共方向(m_vCommonDir)并朝向公共上方向(m_vCommonUpDir)
    case BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR:
        {
            m_vCameraRight = m_pParticleSystemResource->m_vCommonDir.CrossProduct( m_pParticleSystemResource->m_vCommonUpDir );
            m_vCameraUp = m_vCameraRight.CrossProduct( m_pParticleSystemResource->m_vCommonDir );
        }
        break;

        // 平行于自己的运动方向(pParticle->m_direction)并朝向摄像机
    case BBT_HORZ_SELF_DIR_FACE_CAM:
        {
            m_vCameraRight = pParticle->m_direction.CrossProduct( pCamera->GetLookDir() );
            m_vCameraUp = pParticle->m_direction;
        }
        break;

        // 平行于自己的运动方向(pParticle->m_direction)并朝向公共上方向(m_vCommonUpDir)
    case BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR:
        {
            m_vCameraRight = pParticle->m_direction.CrossProduct( m_pParticleSystemResource->m_vCommonUpDir );
            m_vCameraUp = pParticle->m_direction;
        }
        break;

        // 垂直于自己的运动方向(pParticle->m_direction)并朝向摄像机
    case BBT_VERT_SELF_DIR_FACE_CAM:
        {
            m_vCameraRight = pParticle->m_direction.CrossProduct( pCamera->GetLookDir() );
            m_vCameraUp = m_vCameraRight.CrossProduct( pParticle->m_direction );
        }
        break;

        // 垂直于自己的运动方向(pParticle->m_direction)并朝向公共上方向(m_vCommonUpDir)
    case BBT_VERT_SELF_DIR_FACE_COMMON_UP_DIR:
        {
            m_vCameraRight = pParticle->m_direction.CrossProduct( m_pParticleSystemResource->m_vCommonUpDir );
            m_vCameraUp = m_vCameraRight.CrossProduct( pParticle->m_direction );
        }
        break;
    }
    m_vCameraRight.Normalize();
    m_vCameraUp.Normalize();
}

/*
void CParticleSystem::SetBillBoardType(BillboardType type)
{
    m_billboardType = type;
}
*/

void CParticleSystem::GenVertexOffset(CParticle* pParticle)
{
    Vector3Df vLeftOff   = -m_vCameraRight * pParticle->m_fWidth  * 0.5;
    Vector3Df vTopOff    =  m_vCameraUp    * pParticle->m_fHeight * 0.5;
    Vector3Df vRightOff  =  m_vCameraRight * pParticle->m_fWidth  * 0.5;
    Vector3Df vBottomOff = -m_vCameraUp    * pParticle->m_fHeight * 0.5;
    m_vtOffset[0] = vLeftOff  + vTopOff;
    m_vtOffset[1] = vRightOff + vTopOff;
    m_vtOffset[2] = vLeftOff  + vBottomOff;
    m_vtOffset[3] = vRightOff + vBottomOff;
}

/*
void CParticleSystem::LoadEmitter(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("type");

    CParticleEmitter* pEmitter = PARTICLEMITTER_FACTORY->CreateEmitter(strValue, xml);

    if (pEmitter != NULL)
    {
        m_vtParticleEmitter.push_back(pEmitter);
    }
}

void CParticleSystem::LoadAffector(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("type");

    CParticleAffector* pAffector = PARTICLEAFFECTOR_FACTORY->CreateAffector(strValue, xml);

    if (pAffector != NULL)
    {
		pAffector->SetParticleSystem( this );
        m_vtParticleAffector.push_back(pAffector);
    }
}
*/


void CParticleSystem::TriggerAffectors(float fTimeElapsed)
{
    //遍历所有的影响器
    for (VECTOR_AFFECTOR_ITER it = /*m_pParticleSystemResource->*/m_vtParticleAffector.begin(); it != /*m_pParticleSystemResource->*/m_vtParticleAffector.end(); ++it)
    {
        CParticleAffector* pAffector = *it;

		pAffector->RefreshAffectorParam();

        //遍历所有的激活的粒子,并进行影响操作
        pAffector->AffectParticle(this, fTimeElapsed);
        /*
        for (PARTICLE_LIST_ITER pit = m_lstActiveParticles.begin(); pit != m_lstActiveParticles.end(); ++pit)
        {
            pAffector->AffectParticle(*pit, fTimeElapsed);
        }
        */
    }
}


void CParticleSystem::AddEmitter(CParticleEmitter* pEmitter)
{
    m_vtParticleEmitter.push_back(pEmitter);
}

void CParticleSystem::AddAffector(CParticleAffector* pAffector)
{
	pAffector->SetParticleSystem( this );
    m_vtParticleAffector.push_back(pAffector);
}

CParticleEmitter* CParticleSystem::GetFirstEmitter()
{
    VECTOR_EMITTER_ITER it = m_vtParticleEmitter.begin();
    if (it != m_vtParticleEmitter.end())
    {
        return *it;
    }

    return NULL;
}

//获得某个影响器
CParticleAffector* CParticleSystem::GetAffector(u32 nIndex)
{
    if (nIndex < m_vtParticleAffector.size())
    {
        return m_vtParticleAffector[nIndex];
    }
    return NULL;
}


void CParticleSystem::SetPauseEmitParticle(bool b )
{
    m_bPauseEmitParticle = b;
}

void CParticleSystem::SetAutoDetachFromParentNodeWhenEmpty( bool b )
{
	m_bAutoDetach = b;
}


bool CParticleSystem::SaveToFile(const char* szFilename)
{
    CMarkupSTL xml;

    xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
    xml.AddElem("ParticleSystem");
    AIRString strImageFile;
    if (m_pMaterial)
    {
        /*
        CTextureUnit* pTexUnit = m_pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);
        ITexture* pTexture = pTexUnit->GetTexture();
        if (pTexture)
        {
            strImageFile = pTexture->GetName();
        }
        */
    }
    xml.AddAttrib("image", strImageFile.c_str());
    char szTmp[256] = { 0 };
    sprintf(szTmp, "%.2f", m_pParticleSystemResource->GetDefaultWidth());
    xml.AddAttrib("particle_width", szTmp);
    sprintf(szTmp, "%.2f", m_pParticleSystemResource->GetDefaultHeight());
    xml.AddAttrib("particle_height", szTmp);
    xml.AddAttrib("quota", m_pParticleSystemResource->GetQuota());
    xml.AddAttrib("billboard_type", m_pParticleSystemResource->GetBillBoardTypeStr().c_str());
    xml.AddAttrib("boundingbox_autoupdate", m_bBoundsAutoUpdate ? "TRUE" : "FALSE");
    sprintf(szTmp, "%.2f, %.2f, %.2f", m_boundingBox.MaxEdge.x, m_boundingBox.MaxEdge.y, m_boundingBox.MaxEdge.z);
    xml.AddAttrib("boundingbox_max", szTmp);

    sprintf(szTmp, "%.2f, %.2f, %.2f", m_boundingBox.MinEdge.x, m_boundingBox.MinEdge.y, m_boundingBox.MinEdge.z);
    xml.AddAttrib("boundingbox_min", szTmp);

    const Vector3Df& vCommonDir = m_pParticleSystemResource->GetCommonDir();
    sprintf(szTmp, "(%.2f, %.2f, %.2f)", vCommonDir.x, vCommonDir.y, vCommonDir.z );
    xml.AddAttrib("common_dir", szTmp);

    const Vector3Df& vCommonUpDir = m_pParticleSystemResource->GetCommonUpDir();
    sprintf(szTmp, "(%.2f, %.2f, %.2f)", vCommonUpDir.x, vCommonUpDir.y, vCommonUpDir.z );
    xml.AddAttrib("common_up_dir", szTmp);

    if ( m_pParticleSystemResource->GetCullBackFace() )
    {
        xml.AddAttrib( "cull_back_face", "true" );
    }
    else
    {
        xml.AddAttrib( "cull_back_face", "false" );
    }

    if (m_bLocalSpace)
    {
        xml.AddAttrib("local_space", "true");
    }
    else
    {
        xml.AddAttrib("local_space", "false");
    }

	sprintf(szTmp, "%d", m_pParticleSystemResource->GetEmittedEmitterSize());
	xml.AddAttrib("emitted_emitter_quota", szTmp);


    if ( m_bCreateTrail )
    {
        xml.AddAttrib( "create_trail", "TRUE" );
    }
    else
    {
        xml.AddAttrib( "create_trail", "FALSE" );
    }

    sprintf(szTmp, "%d", m_pParticleSystemResource->GetTrailSegmentCount() );
    xml.AddAttrib( "trail_segment_count", szTmp );

    sprintf(szTmp, "%d", m_pParticleSystemResource->GetMaxChainElementNum() /*m_nMaxChainElement*/ );
    xml.AddAttrib( "max_chain_elem", szTmp );

    sprintf(szTmp, "%.2f", m_pParticleSystemResource->GetMaxChainWidth() /*m_fInitTrailWidth*/ );
    xml.AddAttrib( "init_trail_width", szTmp );

    sprintf(szTmp, "%.2f", m_pParticleSystemResource->GetChainWidthChange()/*m_fTrailWidthChange*/ );
    xml.AddAttrib( "trail_width_change", szTmp );

    sprintf(szTmp, "%.4f", m_fEmitEndTime);
    xml.AddAttrib("emit_end_time", m_fEmitEndTime);

	if ( m_pParticleSystemResource->GetTrailUseVertexColor()/*m_bTrailUseVertexColor*/ )
	{
		xml.AddAttrib( "trail_use_vertex_color", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "trail_use_vertex_color", "FALSE" );
	}

	if ( m_pParticleSystemResource->GetTrailUseTexCoord()/*m_bTrailUseTexCoord*/ )
	{
		xml.AddAttrib( "trail_use_tex_coord", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "trail_use_tex_coord", "FALSE" );
	}

	if ( /*m_TCD*/m_pParticleSystemResource->GetTrailCoordDir() == BillBoardChain::TCD_U )
	{
		xml.AddAttrib( "trail_tex_dir", "u" );
	}
	else
	{
		xml.AddAttrib( "trail_tex_dir", "v" );
	}

	if ( /*m_szTrailTex.length()*/m_pParticleSystemResource->GetTrailTex().length() > 0 )
	{
		xml.AddAttrib( "trail_image", /*m_szTrailTex*/m_pParticleSystemResource->GetTrailTex().c_str() );
	}

	xml.AddAttrib( "src_blend", /*m_szSrcBlend*/m_pParticleSystemResource->GetSrcBlend().c_str() );
	xml.AddAttrib( "dest_blend", /*m_szDestBlend*/m_pParticleSystemResource->GetDestBlend().c_str() );
	xml.AddAttrib( "blend_mode", /*m_szBlendMode*/m_pParticleSystemResource->GetBlendMode().c_str() );

	xml.AddAttrib( "RenderType", int(/*m_ParticleSystemType*/m_pParticleSystemResource->GetParticleSystemType()) );
    xml.AddAttrib( "RenderObject", /*m_szParticleRenderableResource*/m_pParticleSystemResource->GetParticleSystemRenderableObjectResource().c_str() );


    //进入ParticleSystem结点
    xml.IntoElem();
    xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");

    //保存发射器信息
    for (VECTOR_EMITTER_ITER it = m_vtParticleEmitter.begin(); it != m_vtParticleEmitter.end(); ++it)
    {
        xml.AddElem("Emitter");
        CParticleEmitter* pEmitter = *it;
        pEmitter->SaveToXMLNode(xml);
    }

    //保存影响器
    for (VECTOR_AFFECTOR_ITER it = m_vtParticleAffector.begin(); it != m_vtParticleAffector.end(); ++it)
    {
        xml.AddElem("Affector");
        CParticleAffector* pAffector = *it;
        pAffector->SaveToXMLNode(xml);
    }

    xml.OutOfElem();
    //-----
    return xml.Save(szFilename);
}

void CParticleSystem::RemoveEmitter(u32 nIndex)
{
    if (nIndex < m_vtParticleEmitter.size())
    {
        VECTOR_EMITTER_ITER it = m_vtParticleEmitter.begin() + nIndex;
        CParticleEmitter* pEmitter = *it;
        m_vtParticleEmitter.erase(it);
        if (pEmitter)
        {
            SafeDelete(pEmitter);
        }
    }
}

void CParticleSystem::RemoveEmitter(CParticleEmitter* pEmitter)
{
    VECTOR_EMITTER_ITER it = std::find(m_vtParticleEmitter.begin(), m_vtParticleEmitter.end(), pEmitter);
    if (it != m_vtParticleEmitter.end())
    {
        m_vtParticleEmitter.erase(it);
        SafeDelete(pEmitter);
    }
}

void CParticleSystem::RemoveAffector(u32 nIndex)
{
    if (nIndex < m_vtParticleAffector.size())
    {
        VECTOR_AFFECTOR_ITER it = m_vtParticleAffector.begin() + nIndex;
        CParticleAffector* pAffector = *it;
        m_vtParticleAffector.erase(it);
        if (pAffector)
        {
            SafeDelete(pAffector);
        }
    }
}

void CParticleSystem::RemoveAffector(CParticleAffector* pAffector)
{
    VECTOR_AFFECTOR_ITER it = std::find(m_vtParticleAffector.begin(), m_vtParticleAffector.end(), pAffector);
    if (it != m_vtParticleAffector.end())
    {
        m_vtParticleAffector.erase(it);
        SafeDelete(pAffector);
    }
}

CParticleEmitter* CParticleSystem::GetEmitter(u32 nIndex)
{
    return m_vtParticleEmitter[nIndex];
}
/*
void CParticleSystem::SetCommonDir( const Vector3Df& vCommonDir )
{
    m_vCommonDir = vCommonDir;
}

const Vector3Df& CParticleSystem::GetCommonDir( void ) const
{
    return m_vCommonDir;
}

void CParticleSystem::SetCommonUpDir( const Vector3Df& vCommonUpDir )
{
    m_vCommonUpDir = vCommonUpDir;
}

const Vector3Df& CParticleSystem::GetCommonUpDir( void ) const
{
    return m_vCommonUpDir;
}

void CParticleSystem::SetCullBackFace( bool b )
{
    m_bCullBackFace = b;
    if ( m_bCullBackFace )
    {
        m_pMaterial->SetCullMode( CM_CCW );
    }
    else
    {
        m_pMaterial->SetCullMode( CM_None );
    }
}

bool CParticleSystem::GetCullBackFace( void )
{
    return m_bCullBackFace;
}
*/

CParticleIterator CParticleSystem::GetIterator()
{
    return CParticleIterator(m_lstActiveParticles.begin(), m_lstActiveParticles.end());
}

//size_t CParticleSystem::GetEmittedEmitterSize()
//{
//    return m_nEmittedEmitterPoolSize;
//}

/*
void CParticleSystem::SetEmittedEmitterSize(size_t nQuota)
{
    m_nEmittedEmitterPoolSize = nQuota;
    m_bInitializeEmittedEmitters = false;
}

void CParticleSystem::IncreaseEmittedEmitterPool(size_t size)
{
    // Don't proceed if the pool doesn't contain any keys of emitted emitters
    if (m_emittedEmitterPool.empty())
        return;

    EMITTER_POOL_ITER emittedEmitterPoolIterator;
    VECTOR_EMITTER_ITER emitterIterator;
    CParticleEmitter* emitter = 0;
    CParticleEmitter* clonedEmitter = 0;
    AIRString name = CStringUtil::BLANK;
    VECTOR_EMITTER* e = 0;
    size_t maxNumberOfEmitters = size / m_emittedEmitterPool.size(); // equally distribute the number for each emitted emitter list
    size_t oldSize = 0;

    // Run through mEmittedEmitterPool and search for every key (=name) its corresponding emitter in mEmitters
    for (emittedEmitterPoolIterator = m_emittedEmitterPool.begin(); emittedEmitterPoolIterator != m_emittedEmitterPool.end(); ++emittedEmitterPoolIterator)
    {
        name = emittedEmitterPoolIterator->first;
        e = &emittedEmitterPoolIterator->second;

        // Search the correct emitter in the mEmitters vector
        emitter = 0;
        for (emitterIterator = m_vtParticleEmitter.begin(); emitterIterator != m_vtParticleEmitter.end(); ++emitterIterator)
        {
            emitter = *emitterIterator;
            if (emitter && 
                name != CStringUtil::BLANK && 
                name == emitter->GetName())
            {		
                // Found the right emitter, clone each emitter a number of times
                oldSize = e->size();
                for (size_t t = oldSize; t < maxNumberOfEmitters; ++t)
                {
                    clonedEmitter = PARTICLEMITTER_FACTORY->CreateEmitter(emitter->GetTypeStr());//ParticleSystemManager::getSingleton()._createEmitter(emitter->getType(), this);
                    clonedEmitter->Copy(emitter);
                    clonedEmitter->SetEmitted(emitter->IsEmitted()); // is always 'true' by the way, but just in case

                    // Initially deactivate the emitted emitter if duration/repeat_delay are set
                    if (clonedEmitter->GetDuration() > 0.0f && 
                        (clonedEmitter->GetRepeatDelay() > 0.0f || clonedEmitter->GetRepeatDelayMin() > 0.0f || clonedEmitter->GetRepeatDelayMax() > 0.0f))
					{
                        clonedEmitter->SetEnable(false);
					}

                    // Add cloned emitters to the pool
                    e->push_back(clonedEmitter);
                }
            }
        }
    }

    m_bNeedCreateRenderData = true;
}

void CParticleSystem::AddFreeEmittedEmitters()
{
    // Don't proceed if the EmittedEmitterPool is empty
    if (m_emittedEmitterPool.empty())
        return;

    // Copy all pooled emitters to the free list
    EMITTER_POOL_ITER emittedEmitterPoolIterator;
    VECTOR_EMITTER_ITER emittedEmitterIterator;
    //VECTOR_EMITTER* 
    VECTOR_EMITTER* emittedEmitters = 0;
    std::list<CParticleEmitter*>* fee = 0;
    AIRString name = CStringUtil::BLANK;

    // Run through the emittedEmitterPool map
    for (emittedEmitterPoolIterator = m_emittedEmitterPool.begin(); emittedEmitterPoolIterator != m_emittedEmitterPool.end(); ++emittedEmitterPoolIterator)
    {
        name = emittedEmitterPoolIterator->first;
        emittedEmitters = &emittedEmitterPoolIterator->second;
        fee = FindFreeEmittedEmitter(name);

        // If it磗 not in the map, create an empty one
        if (!fee)
        {
            LIST_EMITTER empty;
            m_lstFreeEmittedEmitters.insert(make_pair(name, empty));
            fee = FindFreeEmittedEmitter(name);
        }

        // Check anyway if it磗 ok now
        if (!fee)
            return; // forget it!

        // Add all emitted emitters from the pool to the free list
        for(emittedEmitterIterator = emittedEmitters->begin(); emittedEmitterIterator != emittedEmitters->end(); ++emittedEmitterIterator)
        {
            fee->push_back(*emittedEmitterIterator);
        }
    }
}

void CParticleSystem::AddActiveEmittedEmittesToFreeList()
{
	for (LIST_EMITTER_ITER itActive = m_lstActiveEmitters.begin(); itActive != m_lstActiveEmitters.end(); ++itActive)
	{
		std::list<CParticleEmitter*>* fee = FindFreeEmittedEmitter ((*itActive)->GetName());
		if (fee)
			fee->push_back(*itActive);
	}
}

CParticleSystem::LIST_EMITTER* CParticleSystem::FindFreeEmittedEmitter(const AIRString& strName)
{
    FREE_EMITTER_ITER it;
    it = m_lstFreeEmittedEmitters.find (strName);
    if (it != m_lstFreeEmittedEmitters.end())
    {
        // Found it
        return &it->second;
    }

    return 0;
}

void CParticleSystem::InitializeEmittedEmitters()
{
    // Initialise the pool if needed
    size_t currSize = 0;
    if (m_emittedEmitterPool.empty())
    {
        if (m_bInitializeEmittedEmitters)
        {
            // It was already initialised, but apparently no emitted emitters were used
            return;
        }
        else
        {
            InitializeEmittedEmitterPool();
        }
    }
    else
    {
        EMITTER_POOL_ITER i;
        for (i = m_emittedEmitterPool.begin(); i != m_emittedEmitterPool.end(); ++i)
        {
            currSize += i->second.size();
        }
    }

    size_t size = m_nEmittedEmitterPoolSize;
    if( currSize < size && !m_emittedEmitterPool.empty())
    {
        // Increase the pool. Equally distribute over all vectors in the map
        IncreaseEmittedEmitterPool(size);

        // Add new items to the free list
        AddFreeEmittedEmitters();
    }
}

void CParticleSystem::InitializeEmittedEmitterPool()
{
    if (m_bInitializeEmittedEmitters)
        return;

    // Run through mEmitters and add keys to the pool
    //ParticleEmitterList::iterator emitterIterator;
    //ParticleEmitterList::iterator emitterIteratorInner;
    CParticleEmitter* emitter = 0;
    CParticleEmitter* emitterInner = 0;
    for (VECTOR_EMITTER_ITER emitterIterator = m_vtParticleEmitter.begin(); emitterIterator != m_vtParticleEmitter.end(); ++emitterIterator)
    {
        // Determine the names of all emitters that are emitted
        emitter = *emitterIterator ;
        if (emitter && emitter->GetEmittedEmitterName() != CStringUtil::BLANK)
        {
            // This one will be emitted, register its name and leave the vector empty!
            VECTOR_EMITTER empty;
            m_emittedEmitterPool.insert(std::make_pair(emitter->GetEmittedEmitterName(), empty));
        }

        // Determine whether the emitter itself will be emitted and set the 'mEmitted' attribute
        for (VECTOR_EMITTER_ITER emitterIteratorInner = m_vtParticleEmitter.begin(); emitterIteratorInner != m_vtParticleEmitter.end(); ++emitterIteratorInner)
        {
            emitterInner = *emitterIteratorInner;
            if (emitter && 
                emitterInner && 
                emitter->GetName() != CStringUtil::BLANK && 
                emitter->GetName() == emitterInner->GetEmittedEmitterName())
            {
                emitter->SetEmitted(true);
                break;
            }
            else
            {
                // Set explicitly to 'false' although the default value is already 'false'
                //emitter->SetEmitted(false);
            }
        }
    }

    m_bInitializeEmittedEmitters = true;
}


void CParticleSystem::RemoveFromActiveEmittedEmitters(CParticleEmitter* pEmittedEmitter)
{
    LIST_EMITTER_ITER itActiveEmit;
    for (itActiveEmit = m_lstActiveEmitters.begin(); itActiveEmit != m_lstActiveEmitters.end(); ++itActiveEmit)
    {
        if (pEmittedEmitter == (*itActiveEmit))
        {
            m_lstActiveEmitters.erase(itActiveEmit);
            break;
        }
    }
}
*/


void CParticleSystem::SetTrailSegmentCount(int nSegment)
{
    m_pParticleSystemResource->SetTrailSegmentCount(nSegment);
    m_bCreateTrail = false;
}

void CParticleSystem::SetMaxChainElement(int nMaxCount)
{
    m_pParticleSystemResource->SetMaxChainElement(nMaxCount);
    m_bCreateTrail = false;
}

void CParticleSystem::SetMaxChainWidth(float fWidth)
{
    m_pParticleSystemResource->SetMaxChainWidth(fWidth);
    if ( m_pTrail )
    {
        m_pTrail->SetMaxChainWidth( fWidth );
    }
}

void CParticleSystem::SetChainWidthChange(float fWidth)
{
    m_pParticleSystemResource->SetChainWidthChange(fWidth);
    if ( m_pTrail )
    {
        m_pTrail->SetChainWidthChange( fWidth );
    }
}

void CParticleSystem::SetupRibbonTrail()
{
    if (m_bCreateTrail)
    {
        return;
    }
	// 如果拖尾的段数大于0并且最大的段元素数大于0
    if (m_pParticleSystemResource->m_nTrailSegmentCount > 0 && m_pParticleSystemResource->m_nMaxChainElement > 0)
    {
		// 如果拖尾对象不存在
        if (m_pTrail == NULL)
        {
			// 创建拖尾
            m_pTrail = new RibbonTrail(m_pParticleSystemResource->m_nMaxChainElement, m_pParticleSystemResource->m_nTrailSegmentCount);
            //IMaterial* pMaterial = MATERIAL_MANAGER->CreateMaterial();
            //pMaterial->DeepCopy(m_pMaterial);
			// 读入拖尾的材质
			IMaterial* pMaterial = MATERIAL_MANAGER->CreateFromTemplate("RibbonTrail");
            m_pTrail->SetMaterial(pMaterial);
			//////////////////////////////////////////////////////////////////////////
			// 设置拖尾贴图的方向
			m_pTrail->SetTextureCoordDirection( m_pParticleSystemResource->m_TCD );
			// 设置拖尾节点是否使用顶点颜色
			m_pTrail->SetUseVertexColours( m_pParticleSystemResource->m_bTrailUseVertexColor );
			// 设置拖尾节点是否使用纹理坐标
			m_pTrail->SetUseTextureCoords( m_pParticleSystemResource->m_bTrailUseTexCoord );
			// 如果材质对象存在,设置纹理
			if ( pMaterial )
			{
				//pMaterial->SetTextureUnitTex( 0, m_pParticleSystemResource->m_szTrailTex.c_str() );
			}
			// 设置拖尾的变化掩码
			m_pTrail->SetChangeMask( 0, RibbonTrail::eCM_ALL_CHANGE );
			//////////////////////////////////////////////////////////////////////////
            //m_pTrail->setNumberOfChains(m_nTrailSegmentCount);
            //m_pTrail->setMaxChainElements(m_nTrailSegmentCount * m_nChainElementPerSegment);
        }
        else
        {
            m_pTrail->SetNumberOfChains(m_pParticleSystemResource->m_nTrailSegmentCount);
            m_pTrail->SetMaxChainElements(m_pParticleSystemResource->m_nMaxChainElement);
			m_pTrail->SetTextureCoordDirection( m_pParticleSystemResource->m_TCD );
        }

        m_lstFreeChain.clear();
        m_lstActiveChain.clear();

        for ( PARTICLE_LIST_ITER it = m_lstTrailParticle.begin(); it != m_lstTrailParticle.end(); ++it )
        {
            (*it)->m_bTrail = false;
            (*it)->m_nSegmentIndex = -1;
        }
        m_lstTrailParticle.clear();

        for (int i = 0; i < m_pParticleSystemResource->m_nTrailSegmentCount; ++i)
        {
            m_lstFreeChain.push_back(i);
        }

        m_pTrail->ResizeTrailChain( m_pParticleSystemResource->m_nTrailSegmentCount, m_pParticleSystemResource->m_fInitTrailWidth, m_pParticleSystemResource->m_fTrailWidthChange );
    }
	// 当前不用拖尾对象,删除之
    else
    {
        if (m_pTrail)
        {
            m_pTrail->Clear();
            SafeDelete(m_pTrail);
        }

        m_lstFreeChain.clear();
        m_lstActiveChain.clear();
    }

    m_bCreateTrail = true;
}


void CParticleSystem::ResetTrail( void )
{
	if ( m_pTrail )
	{
		m_lstFreeChain.clear();
		m_lstActiveChain.clear();

		for ( PARTICLE_LIST_ITER it = m_lstTrailParticle.begin(); it != m_lstTrailParticle.end(); ++it )
		{
			(*it)->m_bTrail = false;
			(*it)->m_nSegmentIndex = -1;
		}
		m_lstTrailParticle.clear();

		for (int i = 0; i < m_pParticleSystemResource->m_nTrailSegmentCount; ++i)
		{
			m_lstFreeChain.push_back(i);
		}

		m_pTrail->ResizeTrailChain( m_pParticleSystemResource->m_nTrailSegmentCount, 
            m_pParticleSystemResource->m_fInitTrailWidth, m_pParticleSystemResource->m_fTrailWidthChange );
	}
}


void CParticleSystem::SetTrackParticle(CParticle* pParticle)
{
    int nIndex = GetActiveChainIndex();
    if (nIndex > -1)
    {
        pParticle->m_nSegmentIndex = nIndex;
        pParticle->m_bTrail = true;
        m_lstTrailParticle.push_back(pParticle);
    }
}

int CParticleSystem::GetActiveChainIndex()
{
    int nChain = -1;
    if (!m_lstFreeChain.empty())
    {
        nChain = m_lstFreeChain.front();
        m_lstActiveChain.splice(m_lstActiveChain.end(), m_lstFreeChain, m_lstFreeChain.begin());
    }

    return nChain;
}

void CParticleSystem::NotifyTrackParticleDestroy(CParticle* pParticle)
{
    
    PARTICLE_LIST_ITER it = std::find(m_lstTrailParticle.begin(), m_lstTrailParticle.end(), pParticle);
    if (it != m_lstTrailParticle.end())
    {
        m_lstTrailParticle.erase(it);
        if (m_pTrail)
        {
            m_pTrail->DestroyChainSegment(pParticle->m_nSegmentIndex);
        }
        ChainIndex_iter it = std::find(m_lstActiveChain.begin(), m_lstActiveChain.end(), pParticle->m_nSegmentIndex);
        m_lstFreeChain.splice(m_lstFreeChain.end(), m_lstActiveChain, it);
    }
    pParticle->m_nSegmentIndex = -1;
    pParticle->m_bTrail = false;
}

/*
void CParticleSystem::ReorganizeEmittedEmitters()
{
    RemoveAllEmittedEmitters();
    m_bInitializeEmittedEmitters = false;
}
*/
//
//
//// 设置链接的节点
//void CParticleSystem::SetAttachedNode(CSceneNode* pNode)
//{
//	Renderable::SetAttachedNode( pNode );
//
//	CParticleEmitter* pEmitter = NULL;
//
//	//遍历所有的发射器
//	for (VECTOR_EMITTER_ITER it = m_vtParticleEmitter.begin(); it != m_vtParticleEmitter.end() ; ++it )
//	{
//		//执行发射器触发
//		pEmitter = *it;
//		
//		pEmitter->SetPosition( pEmitter->GetPosition() + m_pNode->GetDerivedPosition() );
//		pEmitter->SetDirection( m_pNode->GetDerivedRotation() * pEmitter->GetDirection() );
//	}
//
//}

/*    
void CParticleSystem::SetParticleSystemRenderableObjectResource( const AIRString& szRenderableObject )
{
	m_szParticleRenderableResource = szRenderableObject;
    if (m_ParticleSystemType == ePST_OBJ)
    {
        if (m_szParticleRenderableResource != szRenderableObject)
        {
            //创建粒子对象
            m_szParticleRenderableResource = szRenderableObject;
            char szName[256] = { 0 };
            size_t size = m_vtParticlePool.size();
            for( size_t i = 0; i < size; i++ )
            {
                if (m_ParticleSystemType == ePST_OBJ)
                {
                    sprintf(szName, "%s_%d", m_strName.c_str(), i);
                    m_vtParticlePool[i]->CreateRenderable(szName, m_szParticleRenderableResource.c_str());
                }
            }
        }
        
    }
}

void CParticleSystem::SetParticleSystemType(ParticleSystemType pst)
{
    if (m_ParticleSystemType != pst)
    {
        m_ParticleSystemType = pst;
        if (m_ParticleSystemType == ePST_OBJ && m_szParticleRenderableResource.length() > 0)
        {
            char szName[256] = { 0 };
            size_t size = m_vtParticlePool.size();
            for( size_t i = 0; i < size; i++ )
            {
                if (m_ParticleSystemType == ePST_OBJ)
                {
                    sprintf(szName, "%s_%d", m_strName.c_str(), i);
                    m_vtParticlePool[i]->CreateRenderable(szName, m_szParticleRenderableResource.c_str());
                }
            }
        }
    }
    
}
*/

IMaterial* CParticleSystem::GetMaterial()
{
    if (m_pParticleSystemResource->GetParticleSystemType() == ePST_QUAD)
    {
        return m_pMaterial;
    }
    /*
    else if (m_ParticleSystemType == ePST_OBJ && m_pRenderableObject)
    {
        return m_pRenderableObject->GetMaterial();
    }
    */

    return m_pMaterial;
}

bool CParticleSystem::IsTransparent()
{
    IMaterial* pMaterial = GetMaterial();
    if (pMaterial)
    {
        return pMaterial->IsTransparent();
    }

    return true;
}

bool CParticleSystem::IsCompleted()
{
    if (m_fEmitEndTime > 0)
    {
        if (m_fRenderTimeElapsed > m_fEmitEndTime)
        {
            return m_lstActiveParticles.size() == 0;
        }
    }
    else if (m_bPauseEmitParticle)
    {
        return m_lstActiveParticles.size() == 0;
    }

    return false;
}

void CParticleSystem::Start()
{
    m_fRenderTimeElapsed = 0;
    Reset();
}

void CParticleSystem::SetEmitEndTime(float fTime)
{
    m_fEmitEndTime = fTime;
}

void CParticleSystem::UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    Renderable::UpdateToRenderQueue(pCamera, pQueue);
    if (m_pTrail)
    {
        m_pTrail->UpdateToRenderQueue(pCamera, pQueue);
    }
}

void CParticleSystem::OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    if (m_pParticleSystemResource->m_ParticleSystemType == ePST_QUAD)
    {
        Renderable::OnUpdateToRenderQueue(pCamera, pQueue);
    }
    else
    {
        if (!m_bPaused)
        {
            float fTimeElapse = ENGINE_INST->GetTimeSinceLastFrame();

            //判断当前活动粒子是否过期
            Expire(fTimeElapse);

            // 触发影响器
            TriggerAffectors(fTimeElapse);

            // 计算粒子的位置
            ApplyMotion(fTimeElapse);

            // 触发发射器
            if (m_fEmitEndTime > 0)
            {
                if (m_fRenderTimeElapsed <= m_fEmitEndTime)
                {
                    TriggerEmitters(fTimeElapse);
                }

            }
            else
            {
                TriggerEmitters(fTimeElapse);
            }

            // 更新粒子拖尾
            UpdateTrail(fTimeElapse);

            // 计算粒子包围盒
            UpdateBoundingBox();

            PARTICLE_LIST_ITER itEnd = m_lstActiveParticles.end();
            for (PARTICLE_LIST_ITER it = m_lstActiveParticles.begin(); it != itEnd; ++it)
            {
                (*it)->CaculateRenderableTransform();
                Renderable* pRenderable = (*it)->GetRenderable();
                if (pRenderable)
                {
                    pRenderable->UpdateToRenderQueue(pCamera, pQueue);
                }
                
            }
        }
    }
}

void CParticleSystem::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_pBillboardSet->GetVertexBuffer()->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
        m_pPrimitive->SetUseUserPrimitive(true);
    }

    u32 nVertexCount = m_lstActiveParticles.size() * 4;
    u32 nFaceCount = m_lstActiveParticles.size() * 2;
    m_pPrimitive->SetPrimitiveCount(nFaceCount);
    m_pPrimitive->SetVerticesNum(nVertexCount);
    m_pPrimitive->m_pIndexBuffer = m_pBillboardSet->GetIndexBuffer(); //m_pParticleSystemResource->m_pBillboardSet->GetIndexBuffer();
    m_pPrimitive->m_pVertexBuffer = m_pBillboardSet->GetVertexBuffer(); //m_pParticleSystemResource->m_pBillboardSet->GetVertexBuffer();
}

bool CParticleSystem::Prepare()
{
    if (!m_bPrepared)
    {
        bool bReady = false;
        if (m_pParticleSystemResource && m_pParticleSystemResource->IsLoaded())
        {
            if (m_pParticleSystemResource->IsReady())
            {
                bReady = true;
            }
            else
            {
                bReady = m_pParticleSystemResource->Cache();
            }
        }

        if (bReady)
        {
            m_bLocalSpace = m_pParticleSystemResource->IsLocalSpace();
            if (m_vtParticleEmitter.empty())
            {
                m_pParticleSystemResource->GetEmittersAndAffectors(m_vtParticleEmitter, m_vtParticleAffector);
            }
            
            if (m_pMaterial == NULL)
            {
                m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Particle");
            }
            //m_pParticleSystemResource->GetMaterial();
            //m_bShareParentMaterial = true;
            //创建粒子池或粒子对像等，或者是创建拖尾
            if (m_pParticleSystemResource->HasTrail())
            {
                SetupRibbonTrail();
            }

            bReady = Renderable::Prepare();
        }

        m_bPrepared = bReady;

        if (m_bPrepared)
        {
            GPUVariant var;
            ITexture* pTexture = m_pParticleSystemResource->GetParticleTexture();
            if (pTexture && pTexture->IsLoaded())
            {
                if (!pTexture->IsReady())
                {
                    m_bPrepared = pTexture->Cache();
                }
            }

            if (m_bPrepared)
            {
                var.SetTexture(pTexture);
                m_pMaterial->SetMaterialParam("SampParticleSystem", var);
            }
            
            /*
            CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
            pTechnique->SetBlendOpt((BlendOpt)EngineStringConverter::GetEngineParamByName(m_pParticleSystemResource->GetBlendMode()));
            pTechnique->SetBlendSrc((RenderStateBlendValue)EngineStringConverter::GetEngineParamByName(m_pParticleSystemResource->GetSrcBlend()));
            pTechnique->SetBlendDest((RenderStateBlendValue)EngineStringConverter::GetEngineParamByName(m_pParticleSystemResource->GetDestBlend()));
            */
        }
    }
    

    return m_bPrepared;
}

bool CParticleSystem::PrepareImpl()
{
    return true;
}

void CParticleSystem::CreateDefault()
{
    Clear();
    m_pParticleSystemResource = new ParticleSystemResource;
    m_pParticleSystemResource->SetQuota(100);
    m_pParticleSystemResource->SetDefaultWidth(10.0f);
    m_pParticleSystemResource->SetDefaultHeight(10.0f);
    m_pParticleSystemResource->SetBillBoardType(BBT_POINT);
    m_pParticleSystemResource->SetIsLoaded(true);
    m_pParticleSystemResource->SetIsReady(true);
    //m_pParticleSystemResource->CreateBuffers();

    IMaterial* pMaterial = MATERIAL_MANAGER->CreateFromTemplate("Particle");
    if (pMaterial == NULL)
    {
        return;
    }

    //m_pParticleSystemResource->SetMaterial(pMaterial);
    //PARTICLESYSTEM_MANAGER->AddParticleSystemResource(m_pParticleSystemResource);
    RESOURCE_MANAGER->AddResource(m_pParticleSystemResource);
}

void CParticleSystem::SetTrailTex(const char* szTexFile )
{
    if ( m_pTrail )
    {
        //if (m_pTrail->GetMaterial())
        //{
        //    m_pTrail->GetMaterial()->SetTextureUnitTex( 0, szTexFile );
        //}
    }
    m_pParticleSystemResource->SetTrailTex(szTexFile);
    m_bCreateTrail = false;
}

void CParticleSystem::SetTrailTexCoordDir(u32 dir)
{
    m_pParticleSystemResource->SetTrailTexCoordDir(dir);
    m_bCreateTrail = false;
}
