//#include "stdafx.h"
#include "RenderTargetScene.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "RenderTargetScene.h"
#include "SceneNode.h"
#include "Shadowmap.h"
#include "SceneDebug.h"

RenderTargetScene::RenderTargetScene()
{
    m_strName = "RenderTargetScene";
	m_bUseDepthSurface = true;
}

RenderTargetScene::~RenderTargetScene()
{

}

// 初始化rtt
bool			RenderTargetScene::Initialize(int nWidth, int nHeight)
{
    RenderTarget::Initialize( nWidth, nHeight );
    m_pRenderTexture->SetResetOperation(true, 1.0f);
    return true;
}

// 开始渲染
void			RenderTargetScene::RenderImpl()
{
    // 渲染前的预处理
    //DoPreRender();

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();

    // 渲染场景物体到反射RTT里面
	RenderTarget* pTargets[1];
	pTargets[0] = this;
    pRenderer->SetRenderTargets(1, pTargets);
    //pRenderer->ClearScene( m_clrBackGround );
    pRenderer->BeginScene();
    
    pSceneMgr->RenderScene();
    pRenderer->EndScene();
    //pRenderer->SetRenderTarget(0);

    // 渲染后的处理
    //DoAfterRender();
}


NormalSceneRenderProcess::NormalSceneRenderProcess(eRenderFlow flow) : m_RenderFlow(flow), m_pCamera(NULL), m_pRenderTarget(NULL), m_bNeedDeleteCamera(true)
, m_pDepthStencil(NULL), m_pRenderWindow(NULL)
{

}

NormalSceneRenderProcess::~NormalSceneRenderProcess()
{
    if (m_bNeedDeleteCamera)
    {
        SafeDelete(m_pCamera);
    }
}

void NormalSceneRenderProcess::Render()
{
    if (m_pProcessUpdateFunc)
    {
        m_pProcessUpdateFunc(this);
    }
    ENGINE_INST->SetActiveCamera(m_pCamera);
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();

	if (!m_vecClipPlanes.empty())
	{
		pRenderer->SetClipPlanes(m_vecClipPlanes);
		pRenderer->GetGPUDataParam()->SetClipEnable(true);
	}
	else
		pRenderer->GetGPUDataParam()->SetClipEnable(false);

    m_lstVisableNodes.clear();

    pSceneMgr->Cull(m_pCamera, m_lstVisableNodes, m_RenderFlow);

    Matrix4f matProj, matView;
    if (m_RenderFlow != FLOW_REFLECTION)
    {
        m_pCamera->BuildViewMatrix();
    }
    
    m_pCamera->BuildProjectionMatrix();
    m_pCamera->GetViewMatrix(&matView);
    m_pCamera->GetProjectionMatrix(&matProj);
    //pRenderer->SetViewTransform(&matView);
    //pRenderer->SetProjectionTransform(&matProj);

    if (ENGINE_INST->IsShadowmapEnable() && m_RenderFlow == FLOW_SCENE)
    {
        Shadowmap* pShodowmap = SHADOWMAP;
        AABBBox3Df casterAABB;
        ComputeCasterAABB(casterAABB, matView);
        pShodowmap->SetCasterAABB(casterAABB);
        pShodowmap->Render();
    }

    pRenderer->SetViewTransform(&matView);
    pRenderer->SetProjectionTransform(&matProj);

    AIRList<CSceneNode*>::iterator itEnd = m_lstVisableNodes.end();
    //for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    //{
    //    (*it)->AddAttachObjToRenderQueue(&renderQueue);
    //}
	ENGINE_INST->TriggerEvent(Event_SceneRender, pSceneMgr, 0, 0);

    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != itEnd; ++it)
    {
        (*it)->FillRenderContext(m_pCamera, &m_renderQueue, m_RenderFlow);
    }

	SCENE_DEBUGER->Render(m_pCamera, &m_renderQueue, m_RenderFlow);

#if AIR_TARGET_PLATFORM != AIR_PLATFORM_IOS
    if (m_pRenderWindow)
    {
        RenderTarget* pTargets[1];
        pTargets[0] = pRenderer->GetRenderWindow();

        pRenderer->SetRenderTargets(1, pTargets);
        
    }
    else
    {
	    TextureRenderTarget* pTargets[1];
	    pTargets[0] = m_pRenderTarget;
	    pRenderer->SetRenderTargetsEx(1, pTargets, m_pDepthStencil);

        /*
        CRenderQueue* pQueue = pSceneMgr->GetRenderQueue();

        for (int i = 0; i < (int)RenderGroup_Type::eRGT_Transparent; ++i)
        {
            RenderQueueGroup* pGroup = pQueue->GetRenderQueueGroup((RenderGroup_Type)i);
            if (pGroup)
            {
                ListIterator<RenderQueueGroup::RenderableList> lstRenderableIter = pGroup->GetRenderableListItorObject();
                while (!lstRenderableIter.IsEnd())
                {
                    Renderable* pRenderable = lstRenderableIter.GetNext();

                    pRenderable->Render(m_pCamera, m_RenderFlow);
                }
            }
        }
        */
    }
#endif

    if (m_renderQueue.GetRenderContextsNum() > 0)
    {
        pRenderer->ClearScene( 0x00000000 );
        pRenderer->BeginScene();
        m_renderQueue.DrawContexts();

        pRenderer->EndScene();
    }
    
    pRenderer->GetGPUDataParam()->SetClipEnable(false);
	

    //m_pRenderTarget->SaveToFile((AIRString("D:/RenderTarget") + m_pRenderTarget->GetName() + ".bmp").c_str());
}

void NormalSceneRenderProcess::AddClipPlane(const Plane3Df& plane)
{
	m_vecClipPlanes.push_back(plane);
}

void NormalSceneRenderProcess::ComputeCasterAABB(AABBBox3Df& aabbCaster, const Matrix4f& matView)
{
    bool bIsFirst = true;
    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
    {
        
        CSceneNode* pNode = *it;

        if (pNode->GetCullFlag() & FLOW_SHADOWMAP)
        {
            if (bIsFirst)
            {
                aabbCaster = pNode->GetWorldBoundingBox();
                matView.TransformBox(aabbCaster);
                bIsFirst = false;
            }
            else
            {
                AABBBox3Df aabb = pNode->GetWorldBoundingBox();
                matView.TransformBox(aabb);
                aabbCaster.AddInternalBox(aabb);
            }
        }
        
    }
}
