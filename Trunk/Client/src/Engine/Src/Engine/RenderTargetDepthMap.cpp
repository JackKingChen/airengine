#include "stdafx.h"
#include "RenderTargetDepthMap.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "MaterialManager.h"
//#include "Skybox.h"
#include "TimeBaseProfile.h"
#include "TextureMgr.h"
//#include "Technique.h"

CCamera* RenderTargetDepthMap::GetCamera()
{
    ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();
    return pSceneMgr->GetCamera();
}

void RenderTargetDepthMap::RenderImpl()
{
	//if ( ++m_nCurIgnorFrames < m_nIgnorFramesCount )
	//{
	//	return;
	//}
	//else
	//{
	//	m_nCurIgnorFrames = 0;
	//}

    /*
	Update();

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();

	pRenderer->SetProjectionTransform(&(pSceneMgr->GetCamera()->GetProjectionMatrix()));
	pRenderer->SetViewTransform(&(pSceneMgr->GetCamera()->GetViewMatrix()));
	pRenderer->SetRenderTarget(this);
    */
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();
	pRenderer->ClearScene( m_clrBackGround );
	pRenderer->BeginScene();

	/*CRenderQueue* pRenderQueue = pSceneMgr->GetRenderQueue();

    
	CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
	for (size_t i = 0; i < pTechnique->GetPassesNum(); ++i)
	{
		CPass* pPass = pTechnique->GetPass(i);
		pPass->SetFogEnable( false );

		//CSkyBox* pSkyBox = (CSkyBox*)pSceneMgr->GetSkybox();
		//if ( pSkyBox )
		//{
		//	pSkyBox->RenderToDepthMap(pSceneMgr->GetCamera(), pPass);
		//}

		//ÏÈäÖÈ¾µØÐÎ
		RenderQueueGroup* pGroup = pRenderQueue->GetRenderQueueGroup(eRGT_Terrain);
		if (pGroup)
		{
            ListIterator<RenderQueueGroup::RenderableList> lstRenderableIter = pGroup->GetRenderableListItorObject();

			while (!lstRenderableIter.IsEnd())
			{
				Renderable* pRenderable = lstRenderableIter.GetNext();
				//pRenderable->CommitTransform();
                pRenderer->SetWorldTransform(&pRenderable->GetWorldMatrix());

				//pRenderable->RenderToDepthMap(pSceneMgr->GetCamera(), pPass);
			}
		}

		pGroup = pRenderQueue->GetRenderQueueGroup(eRGT_Solid);
		if (pGroup)
		{
            ListIterator<RenderQueueGroup::RenderableList> lstRenderableIter = pGroup->GetRenderableListItorObject();
			while (!lstRenderableIter.IsEnd())
			{
				Renderable* pRenderable = lstRenderableIter.GetNext();
				//pRenderable->CommitTransform();
                pRenderer->SetWorldTransform(&pRenderable->GetWorldMatrix());

				//pRenderable->RenderToDepthMap(pSceneMgr->GetCamera(), pPass);
			}
		}
	}
    */

	pRenderer->EndScene();

	pRenderer->SetRenderTargets(1, 0);

}

void RenderTargetDepthMap::Clear()
{
	RenderTarget::Clear();
	MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
	m_pMaterial = NULL;
	//m_pTechniqueRenderToDepthMap = NULL;
}

bool RenderTargetDepthMap::Initialize(int nWidth, int nHeight)
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	if (m_pRenderTexture == NULL)
	{
		m_pRenderTexture = TEXTURE_MANAGER->CreateTexture(m_strName.c_str(), TextureParam(nWidth, nHeight, 0, 1,
            eRSU_RenderTarget, RPF_R32F, TT_TEXTURE, true));
            //pRenderer->CreateFloatTexture(m_strName.c_str(), nWidth, nHeight, true, 1, true);
		m_pRenderTexture->SetResetOperation(true, 1.0f);
	}

	m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/RenderToDepthMap");
	if (m_pMaterial)
	{
		//m_pTechniqueRenderToDepthMap = m_pMaterial->GetTechnique("Default");
	}

	return true;
}

//CTechnique* RenderTargetDepthMap::GetTechniqueRenderToDepthMap()
//{
//	return m_pTechniqueRenderToDepthMap;
//}

/*
void RenderTargetDepthMap::UpdateImp()
{

}
*/