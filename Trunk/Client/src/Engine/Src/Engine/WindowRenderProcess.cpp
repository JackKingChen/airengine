#include "WindowRenderProcess.h"
#include "RenderTarget.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "Shadowmap.h"
#include "SceneDebug.h"
#include "SceneNode.h"
#include "NormalSceneRenderProcess.h"
#include "ScreenCompositor.h"

WindowRenderProcess::WindowRenderProcess() : m_RenderFlow(FLOW_SCENE), m_bNeedDeleteCamera(false)
, m_pRenderWindow(NULL)
, m_pCamera(NULL)
, m_pDepthStencil(NULL)
, m_pSceneMgr(NULL)
, m_pNormalSceneProcess(NULL)
, m_bPostProcessEnable(false)
, m_pRTTScene(NULL)
, m_pScreenCompositor(NULL)
//, m_pRenderFunc(NULL)
{
	memset(m_vtPostProcess, 0, sizeof(m_vtPostProcess));
}

WindowRenderProcess::~WindowRenderProcess()
{
	if (m_bNeedDeleteCamera)
	{
		SafeDelete(m_pCamera);
	}

	if (m_pRTTScene)
	{
		m_pRTTScene->Clear();
		SafeDelete(m_pRTTScene);
	}

	if (m_pNormalSceneProcess)
	{
		m_pNormalSceneProcess->Clear();
		SafeDelete(m_pNormalSceneProcess);
	}

	for (size_t i = 0; i < ePP_MAX; ++i)
	{
		if (m_vtPostProcess[i])
		{
			m_vtPostProcess[i]->Clear();
			SafeDelete(m_vtPostProcess[i]);
		}
	}

	if (m_pScreenCompositor)
	{
		m_pScreenCompositor->Clear();
		SafeDelete(m_pScreenCompositor);
	}
}

void WindowRenderProcess::Render()
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	//ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();
	//if (m_pCamera == NULL)
	if (!m_bPostProcessEnable)
	{
		m_pCamera = m_pSceneMgr->GetCamera();

		pRenderer->SetAmbientLight(m_pSceneMgr->GetAmbientColor());

		CGPUDataParamSource* pGPUData = pRenderer->GetGPUDataParam();
		pGPUData->SetCameraPosition(m_pCamera->GetPosition());
		//pGPUData->SetViewMatrix(matView);
		//pGPUData->SetProjectionMatrix(matProj);
		pGPUData->SetAmbientColor(m_pSceneMgr->GetAmbientColor());
		pGPUData->SetFogColor(m_pSceneMgr->GetFogColor());
		pGPUData->SetFogStart(m_pSceneMgr->GetFogStart());
		pGPUData->SetFogEnd(m_pSceneMgr->GetFogEnd());
		pGPUData->SetFogDensity(m_pSceneMgr->GetFogDensity());
		pGPUData->SetFogType(m_pSceneMgr->GetFogMode());
		pGPUData->SetSunDirection(m_pSceneMgr->GetSunLightDirection());
		pGPUData->SetSunLightColor(m_pSceneMgr->GetSunLight()->GetDiffuse());
		pGPUData->SetCameraFarPlane(m_pCamera->GetViewFar());

		//设置雾
		pRenderer->SetFog(m_pSceneMgr->GetFogMode(), m_pSceneMgr->GetFogColor(), m_pSceneMgr->GetFogDensity(), m_pSceneMgr->GetFogStart(), m_pSceneMgr->GetFogEnd());

		if (!m_vecClipPlanes.empty())
		{
			pRenderer->SetClipPlanes(m_vecClipPlanes);
			pRenderer->GetGPUDataParam()->SetClipEnable(true);
		}
		else
			pRenderer->GetGPUDataParam()->SetClipEnable(false);

		m_lstVisableNodes.clear();

		m_pSceneMgr->Cull(m_pCamera, m_lstVisableNodes, m_RenderFlow);

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
		ENGINE_INST->SetActiveCamera(m_pCamera);

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
		ENGINE_INST->TriggerEvent(Event_SceneRender, m_pSceneMgr, 0, 0);

		for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != itEnd; ++it)
		{
			(*it)->FillRenderContext(m_pCamera, &m_renderQueue, m_RenderFlow);
		}

		SCENE_DEBUGER->Render(m_pCamera, &m_renderQueue, m_RenderFlow);

		if (m_pRenderWindow)
		{
			RenderTarget* pTargets[1];
			pTargets[0] = m_pRenderWindow;
			pRenderer->SetCurrentRenderWindow((RenderWindow*)m_pRenderWindow);
			pRenderer->SetRenderTargets(1, pTargets);

		}
		//else
		//{
		// TextureRenderTarget* pTargets[1];
		// pTargets[0] = m_pRenderTarget;
		// pRenderer->SetRenderTargetsEx(1, pTargets, m_pDepthStencil);
		//}

		if (m_renderQueue.GetRenderContextsNum() > 0)
		{
			//pRenderer->ClearScene( 0x00000000, true, true );
			pRenderer->BeginScene();
			m_renderQueue.DrawContexts();

			pRenderer->EndScene();
		}

		pRenderer->GetGPUDataParam()->SetClipEnable(false);
	}
	else
	{
		if (m_pNormalSceneProcess == NULL)
		{
			m_pNormalSceneProcess = new NormalSceneRenderProcess(FLOW_SCENE);
			m_pNormalSceneProcess->SetCamera(m_pSceneMgr->GetCamera(), false);
		}

		if (m_pRTTScene == NULL)
		{
			m_pRTTScene = TextureRenderTarget::CreateRenderTarget("RTTScene", m_pRenderWindow->GetWidth(), m_pRenderWindow->GetHeight(), RPF_A8R8G8B8);
			m_pRTTScene->SetUseDepthSurface(true);
		}

		m_pNormalSceneProcess->AttachRenderTarget(m_pRTTScene);
		pRenderer->SetTexture(0, NULL);
		pRenderer->SetTexture(1, NULL);
		m_pNormalSceneProcess->Render();

		ITexture* pInputTexture = m_pRTTScene->GetTexture();
		//pInputTexture->SaveToFile("D:\\postprocess.png");
		
		for (size_t i = 0; i < ePP_MAX; ++i)
		{
			if (m_vtPostProcess[i] && m_vtPostProcess[i]->IsActive())
			{
				m_vtPostProcess[i]->SetInputTexture(pInputTexture);
				m_vtPostProcess[i]->SetSceneTexture(m_pRTTScene->GetTexture());
				m_vtPostProcess[i]->Render();
				pInputTexture = m_vtPostProcess[i]->GetOutputTexture();
			}
		}
		//pInputTexture->SaveToFile("D:\\postprocess.png");

		if (m_pRenderWindow)
		{
			RenderTarget* pTargets[1];
			pTargets[0] = m_pRenderWindow;
			pRenderer->SetCurrentRenderWindow((RenderWindow*)m_pRenderWindow);
			pRenderer->SetRenderTargets(1, pTargets);
		}
		pRenderer->BeginScene();

		if (m_pScreenCompositor == NULL)
		{
			m_pScreenCompositor = new ScreenCompositor();
			m_pScreenCompositor->Create(ePPT_None, 0, 0, m_pRenderWindow->GetWidth(), m_pRenderWindow->GetHeight());
		}
		m_pScreenCompositor->SetInputTexture(pInputTexture);
		//pInputTexture->SaveToFile("D:\\postprocess.png");
		m_pScreenCompositor->Draw();
		pRenderer->EndScene();
	}

	
	//最后把pInputTexture贴到屏幕上

	ENGINE_INST->ExcuteGUIRender();
	
	if (m_pRenderWindow)
	{
		m_pRenderWindow->Render();
	}
}

void WindowRenderProcess::ComputeCasterAABB(AABBBox3Df& aabbCaster, const Matrix4f& matView)
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

WindowRenderProcess* WindowRenderProcess::CreateWindowRenderProcess(const char* szName)
{
	WindowRenderProcess* pProcess = ENGINE_INST->GetWindowRenderProcess(szName);
	if (pProcess == NULL)
	{
		pProcess = new WindowRenderProcess;
		pProcess->SetName(szName);
		ENGINE_INST->AddWindowRenderProcess(pProcess);
	}

	return pProcess;
}

void WindowRenderProcess::EnablePostProcess(PostProcessType type, bool bEnable)
{
	if (m_vtPostProcess[type] == NULL)
	{
		switch (type)
		{
		case ePP_Monochrome:
			break;
		case ePP_Glow:
			m_vtPostProcess[type] = new PostGlowRenderProcess;
			break;
		case ePP_SoftLight:
			m_vtPostProcess[type] = new PostSoftlightRenderProcess;
			break;
		}
	}

	if (m_vtPostProcess[type])
	{
		m_vtPostProcess[type]->SetActive(bEnable);
	}

}

void WindowRenderProcess::DisableAllPostProcess()
{
	for (size_t i = 0; i < ePP_MAX; ++i)
	{
		if (m_vtPostProcess[i])
		{
			m_vtPostProcess[i]->SetActive(false);
		}
	}
}
