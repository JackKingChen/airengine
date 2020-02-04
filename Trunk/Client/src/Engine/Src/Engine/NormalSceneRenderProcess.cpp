//#include "stdafx.h"
#include "NormalSceneRenderProcess.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "NormalSceneRenderProcess.h"
#include "SceneNode.h"
#include "Shadowmap.h"
#include "SceneDebug.h"
/*
NormalSceneRenderProcess::NormalSceneRenderProcess()
{
    m_strName = "NormalSceneRenderProcess";
	m_bUseDepthSurface = true;
}

NormalSceneRenderProcess::~NormalSceneRenderProcess()
{

}

// ³õÊ¼»¯rtt
bool			NormalSceneRenderProcess::Initialize(int nWidth, int nHeight)
{
    RenderTarget::Initialize( nWidth, nHeight );
    m_pRenderTexture->SetResetOperation(true, 1.0f);
    return true;
}
*/

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

	pRenderer->SetAmbientLight(pSceneMgr->GetAmbientColor());

	CGPUDataParamSource* pGPUData = pRenderer->GetGPUDataParam();
	pGPUData->SetCameraPosition(m_pCamera->GetPosition());
	//pGPUData->SetViewMatrix(matView);
	//pGPUData->SetProjectionMatrix(matProj);
	pGPUData->SetAmbientColor(pSceneMgr->GetAmbientColor());
	pGPUData->SetFogColor(pSceneMgr->GetFogColor());
	pGPUData->SetFogStart(pSceneMgr->GetFogStart());
	pGPUData->SetFogEnd(pSceneMgr->GetFogEnd());
	pGPUData->SetFogDensity(pSceneMgr->GetFogDensity());
	pGPUData->SetFogType(pSceneMgr->GetFogMode());
	pGPUData->SetSunDirection(pSceneMgr->GetSunLightDirection());
	pGPUData->SetSunLightColor(pSceneMgr->GetSunLight()->GetDiffuse());
	pGPUData->SetCameraFarPlane(m_pCamera->GetViewFar());

	//ÉèÖÃÎí
	pRenderer->SetFog(pSceneMgr->GetFogMode(), pSceneMgr->GetFogColor(), pSceneMgr->GetFogDensity(), pSceneMgr->GetFogStart(), pSceneMgr->GetFogEnd());

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
	pRenderer->SetViewTransform(&matView);
	pRenderer->SetProjectionTransform(&matProj);
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

    if (m_pRenderWindow)
    {
        RenderTarget* pTargets[1];
        pTargets[0] = m_pRenderWindow;

        pRenderer->SetRenderTargets(1, pTargets);
        
    }
    else
    {
	    TextureRenderTarget* pTargets[1];
	    pTargets[0] = m_pRenderTarget;
		if (m_pDepthStencil == NULL)
		{
			if (m_pRenderTarget->IsUseDepthSurface())
			{
				m_pDepthStencil = pRenderer->CreateDepthStencil(m_pRenderTarget->GetWidth(), m_pRenderTarget->GetHeight(), RPF_D24S8, false);
			}
		}
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

    if (m_renderQueue.GetRenderContextsNum() > 0)
    {
        //pRenderer->ClearScene( 0x00000000 );
        pRenderer->BeginScene();
        m_renderQueue.DrawContexts();

        pRenderer->EndScene();
    }
    
    pRenderer->GetGPUDataParam()->SetClipEnable(false);
	
	if (m_pRenderWindow)
	{
		m_pRenderWindow->Render();
	}
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
