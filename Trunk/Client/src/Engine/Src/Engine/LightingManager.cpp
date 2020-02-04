#include "stdafx.h"
#include "LightingManager.h"
//#include "GBufferRenderProcess.h"
//#include "RenderTargetLightBuffer.h"
#include "Engine.h"
#include "IMaterial.h"
#include "ISceneManager.h"
//#include "Technique.h"
//#include "Pass.h"
#include "MaterialManager.h"
#include "FullScreenQuad.h"

LightingManager::LightingManager() : 
m_pDeferredLightingGBuffer(NULL)
, m_pDeferredLightingLightBuffer(NULL)
{

}

LightingManager::~LightingManager()
{

}

void LightingManager::Clear()
{
    m_currentLights.clear();
    //if (m_pLightTarget)
    //{
    //    ENGINE_INST->DestroyRenderTarget(m_pLightTarget);
    //    m_pLightTarget = NULL;
    //}

    //if (m_pRenderTargetGBuffer)
    //{
    //    ENGINE_INST->DestroyRenderTarget(m_pRenderTargetGBuffer);
    //    m_pRenderTargetGBuffer = NULL;
    //}

	if (m_pDeferredLightingGBuffer)
	{
		//ENGINE_INST->RemoveCustomRenderProcess(m_pDeferredLightingGBuffer);
		m_pDeferredLightingGBuffer->Clear();
	}

	if (m_pDeferredLightingLightBuffer)
	{
		//ENGINE_INST->RemoveCustomRenderProcess(m_pDeferredLightingLightBuffer);
	}
}

void LightingManager::Update()
{
    if (ENGINE_INST->IsDeferredLighting())
    {
		CreateLightTargetAndGBuffer();
    }
    m_currentLights.clear();
}

void LightingManager::AddCurrentLight(CLight* pLight)
{
    m_currentLights.insert(pLight);
}

void LightingManager::EnableDeferredLighting(bool bEnable)
{
    if (bEnable)
    {
		CreateLightTargetAndGBuffer();

		m_pDeferredLightingGBuffer->SetActive(true);
		m_pDeferredLightingLightBuffer->SetActive(true);

		//ENGINE_INST->AddCustomRenderProcess(m_pDeferredLightingGBuffer);
		//ENGINE_INST->AddCustomRenderProcess(m_pDeferredLightingLightBuffer);
    }
	else
	{
		if (m_pDeferredLightingGBuffer)
		{
			m_pDeferredLightingGBuffer->SetActive(false);
		}

		if (m_pDeferredLightingLightBuffer)
		{
			m_pDeferredLightingLightBuffer->SetActive(false);
		}
	}
}

void LightingManager::CreateLightTargetAndGBuffer()
{
	/*
	if (m_pLightTarget == NULL)
	{
		int nWidth, nHeight;
		ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
		m_pLightTarget = new RenderTargetLightBuffer;
		m_pLightTarget->Initialize(nWidth, nHeight);
	}

	if (m_pRenderTargetGBuffer == NULL)
	{
		int nWidth, nHeight;
		ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
		m_pRenderTargetGBuffer = new GBufferRenderProcess;
		m_pRenderTargetGBuffer->Initialize(nWidth, nHeight);
	}
	*/

	if (m_pDeferredLightingGBuffer == NULL)
	{
		m_pDeferredLightingGBuffer = new DeferredLightingGBuffer;
	}

	if (m_pDeferredLightingLightBuffer == NULL)
	{
		m_pDeferredLightingLightBuffer = new DeferredLightingLightBuffer;
	}
}
/*
void LightingManager::SaveRenderTargetToFile()
{
    if (m_pRenderTargetGBuffer)
    {
        m_pRenderTargetGBuffer->GetRenderTexture()->SaveToFile("D:\\DepthBuffer.dds");
        (static_cast<GBufferRenderProcess*>(m_pRenderTargetGBuffer))->GetNormalRenderTarget()->GetRenderTexture()->SaveToFile("D:\\NormalBuffer.bmp");
    }

    if (m_pLightTarget)
    {
        m_pLightTarget->GetRenderTexture()->SaveToFile("D:\\LightBuffer.bmp");
    }
}
*/
DeferredLightingGBuffer::DeferredLightingGBuffer() : m_pDepthTarget(NULL)
, m_pNormalTarget(NULL)
, m_pMaterial(NULL)
//, m_pTechniqueClearGBuffer(NULL)
, m_pDepthStencil(NULL)
{
	m_quad.SetFullWindow(true);
}

DeferredLightingGBuffer::~DeferredLightingGBuffer()
{
	
}

void DeferredLightingGBuffer::Clear()
{
	if (m_pMaterial)
	{
		MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
	}

    if (m_pNormalTarget)
    {
        //RENDERER->DestroyTextureRenderTarget(m_pNormalTarget);
		m_pNormalTarget->Clear();
		SafeDelete(m_pNormalTarget);
    }
    
    if (m_pDepthTarget)
    {
        //RENDERER->DestroyTextureRenderTarget(m_pDepthTarget);
		m_pDepthTarget->Clear();
		SafeDelete(m_pDepthTarget);
    }

}

void DeferredLightingGBuffer::Render()
{
	if (m_pMaterial == NULL)
	{
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/LightPrePass");
		if (m_pMaterial)
		{
			//m_pTechnique = m_pMaterial->GetTechnique("RenderToGBuffer");
			//m_pTechniqueClearGBuffer = m_pMaterial->GetTechnique("ClearGBuffer");
		}
	}

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	if (m_pDepthTarget == NULL)
	{
		int nWidth = 0;
		int nHeight = 0;
		ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
		m_pDepthTarget = TextureRenderTarget::CreateRenderTarget("LPPGBufferDepth", nWidth, nHeight, RPF_R32F);//pRenderer->CreateTextureRenderTarget("LPPGBufferDepth", nWidth, nHeight, RPF_R32F);

		m_pDepthTarget->SetFullScreen(true);
		m_pDepthTarget->SetIndex(0);
	}

	if (m_pNormalTarget == NULL)
	{
		int nWidth = 0;
		int nHeight = 0;
		ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
		m_pNormalTarget = TextureRenderTarget::CreateRenderTarget("LPPGBufferNormal", nWidth, nHeight, RPF_A8R8G8B8); //pRenderer->CreateTextureRenderTarget("LPPGBufferNormal", nWidth, nHeight, RPF_A8R8G8B8);

		m_pNormalTarget->SetFullScreen(true);
		m_pNormalTarget->SetIndex(1);

		if (m_pDepthStencil == NULL)
		{
			m_pDepthStencil = RENDERER->CreateDepthStencil(nWidth, nHeight, RPF_D24S8, false);
		}
	}

	pRenderer->SetTexture( 11, NULL );

	if (m_pMaterial)
	{
		if (!m_pMaterial->IsReady())
		{
			m_pMaterial->Cache();
			//m_pTechnique = m_pMaterial->GetTechnique("RenderToGBuffer");
			//m_pTechniqueClearGBuffer = m_pMaterial->GetTechnique("ClearGBuffer");
		}

		if (m_pMaterial->IsReady())
		{

			ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();

			CCamera* pCamera = pSceneMgr->GetCamera();

			pCamera->Update();
			ENGINE_INST->SetActiveCamera(pCamera);
			//pRenderer->SetRenderTarget(this);
			//pRenderer->SetRenderTarget(m_pRTTNormal);

			TextureRenderTarget* pTargets[2];
			pTargets[0] = m_pDepthTarget;
			pTargets[1] = m_pNormalTarget;
			pRenderer->SetRenderTargetsEx(2, pTargets, m_pDepthStencil);

			//pRenderer->ClearScene( 0x00000000 );
			pRenderer->BeginScene();
			// 渲染可视范围内的对像


			//首先要清空两个rendertarget
			//for (int i = 0; i < m_pTechniqueClearGBuffer->GetPassesNum(); ++i)
			//{
			//	CPass* pPass = m_pTechniqueClearGBuffer->GetPass(i);
			//	//pRenderer->SetTexture(0, NULL);
			//	pPass->ApplyRenderStates();
			//	m_quad.PostRender(NULL);
			//}

			CRenderQueue* pQueue = NULL;//pSceneMgr->GetRenderQueue();

			CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();

			//for (int i = 0; i < m_pTechnique->GetPassesNum(); ++i)
			//{
			//    CPass* pPass = m_pTechnique->GetPass(i);

			//先渲染地形

			RenderQueueGroup* pGroup = pQueue->GetRenderQueueGroup(eRGT_Terrain);

			if (pGroup)
			{
				ListIterator<RenderQueueGroup::RenderableList> lstRenderableIter = pGroup->GetRenderableListItorObject();

				while (!lstRenderableIter.IsEnd())
				{
					Renderable* pRenderable = lstRenderableIter.GetNext();
					//pRenderable->CommitTransform();
					//pRenderer->SetWorldTransform(&pRenderable->GetWorldMatrix());

					IMaterial* pMaterial = pRenderable->GetMaterial();
					if (pMaterial)
					{
						pGPUDataParam->SetMaterialShiness(pMaterial->GetShininess());
					}

					//pPass->ApplyShaders();

					//pRenderer->SetVertexBuffer(0, pRenderable->GetVertexBuffer());
					//pRenderer->SetIndexBuffer(pRenderable->GetIndexBuffer());
					//pRenderable->PostRender(pSceneMgr->GetCamera());

					//pRenderable->RenderToGBuffer(pSceneMgr->GetCamera());
				}
			}


			//轮到 
			pGroup = pQueue->GetRenderQueueGroup(eRGT_Solid);
			if (pGroup)
			{
				ListIterator<RenderQueueGroup::RenderableList> lstRenderableIter = pGroup->GetRenderableListItorObject();
				while (!lstRenderableIter.IsEnd())
				{
					Renderable* pRenderable = lstRenderableIter.GetNext();
					//pRenderable->CommitTransform();
					pRenderer->SetWorldTransform(&pRenderable->GetWorldMatrix());

					IMaterial* pMaterial = pRenderable->GetMaterial();
					if (pMaterial)
					{
						pGPUDataParam->SetMaterialShiness(pMaterial->GetShininess());
					}

					//pRenderable->RenderToGBuffer(pCamera);
				}
			}

			//}
			//pSceneMgr->RenderScene();
			//m_pRTTNormal->GetRenderTexture()->SaveToFile("D:/GBufferNormal.png");
			//m_pRenderTexture->SaveToFile("D:/GBufferDepth.dds");
			pRenderer->SetRenderTargets(2, NULL);

			pRenderer->EndScene();

			//begine add by hjj 2011-8-19
			pRenderer->SetTexture( 11, m_pDepthTarget->GetTexture() );
			pRenderer->SetTextureBorderColor( 11, 0xffffffff );
			pRenderer->SetTextureFilter(11, TFT_Point);
			//end add

			pRenderer->SetTexture(0, m_pDepthTarget->GetTexture());
			pRenderer->SetTexture(1, m_pNormalTarget->GetTexture());
		}
	}
}

DeferredLightingLightBuffer::DeferredLightingLightBuffer() : m_pMaterial(NULL)
, m_pFullScreenQuad(NULL)
//, m_pDirectionalLightTechnique(NULL)
//, m_pPointLightTechnique(NULL)
, m_pLightTarget(NULL)
{
	 m_pFullScreenQuad = new FullScreenQuad;
}

DeferredLightingLightBuffer::~DeferredLightingLightBuffer()
{

}

void DeferredLightingLightBuffer::Render()
{
	RENDERER->SetTexture(10, 0);
	if (m_pMaterial == NULL)
	{
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/LightPrePass");
		if (m_pMaterial && m_pMaterial->IsReady())
		{
			//m_pPointLightTechnique = m_pMaterial->GetTechnique("LPPPointLightRenderToLightBuffer");
			//m_pDirectionalLightTechnique = m_pMaterial->GetTechnique("LightPrePassDirectionalLightRenderToLightBuffer");
			//m_pTechniqueClearGBuffer = m_pMaterial->GetTechnique("ClearGBuffer");
		}
	}

	if (m_pLightTarget == NULL)
	{
		IRenderer* pRenderer = ENGINE_INST->GetRenderer();
		int nWidth = 0;
		int nHeight = 0;
		ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
		m_pLightTarget = TextureRenderTarget::CreateRenderTarget("DeferredLightingLightTarget", nWidth, nHeight, RPF_A8R8G8B8);//pRenderer->CreateTextureRenderTarget("DeferredLightingLightTarget", nWidth, nHeight, RPF_A8R8G8B8);
		m_pLightTarget->SetFullScreen(true);
	}

    /*
	if (m_pMaterial)
	{
		if (!m_pMaterial->IsReady())
		{
			m_pMaterial->Cache();
			//m_pPointLightTechnique = m_pMaterial->GetTechnique("LPPPointLightRenderToLightBuffer");
			//m_pDirectionalLightTechnique = m_pMaterial->GetTechnique("LightPrePassDirectionalLightRenderToLightBuffer");
		}

		if (m_pMaterial->IsReady())
		{
			IRenderer* pRenderer = ENGINE_INST->GetRenderer();
			TextureRenderTarget* pTargets[1];
			pTargets[0] = m_pLightTarget;
			//ITexture* pTextureDepth = LIGHTING_MANAGER->GetRenderTargetGBuffer()->GetRenderTexture();
			//ITexture* pTextureNormal = (static_cast<GBufferRenderProcess*>(LIGHTING_MANAGER->GetRenderTargetGBuffer()))->GetNormalRenderTarget()->GetRenderTexture();

			pRenderer->SetRenderTargetsEx(1, pTargets, NULL);
			pRenderer->ClearScene(0x00000000);
			pRenderer->BeginScene();

			SetIterator<LightingManager::LightList> lightListIter = LIGHTING_MANAGER->GetLightListIter();

			//for (size_t i = 0; i < m_pTechnique->GetPassesNum(); ++i)



			//ITexture* pTextureDepth = LIGHTING_MANAGER->GetRenderTargetGBuffer()->GetRenderTexture();
			//ITexture* pTextureNormal = (static_cast<GBufferRenderProcess*>(LIGHTING_MANAGER->GetRenderTargetGBuffer()))->GetNormalRenderTarget()->GetRenderTexture();

			CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();
			//pRenderer->SetTexture(0, pTextureDepth);
			//pRenderer->SetTexture(1, pTextureNormal);
			ISceneManager* pCurScene = ENGINE_INST->GetSceneMgr();
			CCamera* pCamera = pCurScene->GetCamera();
			pGPUDataParam->SetCameraFarPlane(pCamera->GetViewFar());
			//pPass->GetTextureUnit(1)->ApplyTextureUnit(pPass, pTextureNormal);

			pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
			const Matrix4f& matView = pCurScene->GetCamera()->GetViewMatrix();
			Vector3Df cameraPosVS;
			matView.TransformCoord(pCamera->GetPosition(), cameraPosVS);
			pGPUDataParam->SetCameraPosition(cameraPosVS);

			//渲染directional light
			CPass* pPass = NULL;
			CLight* pSunLight = pCurScene->GetSunLight();
			if (pSunLight->GetDirection() != Vector3Df::ZERO)
			{
				//Matrix4f matWorld;
				//matWorld.Identity();
				//int nViewportWidth, nViewportHeight;
				//ENGINE_INST->GetBackBufferSize(nViewportWidth, nViewportHeight);
				//matWorld.SetScale(nViewportWidth * 2);
				//matWorld.SetTranslation(pCamera->GetLookAt().x, pCamera->GetLookAt().y, pCamera->GetViewNear());
				//pRenderer->SetWorldTransform(&matWorld);

				CPass* pPass = m_pDirectionalLightTechnique->GetPass(0);
				const Vector3Df* pWorldSpaceCorners = pCurScene->GetCamera()->GetFrustum()->GetWorldSpaceCorners();
				for (int i = 0; i < 4; ++i)
				{
					matView.TransformCoord(pWorldSpaceCorners[i + 4], m_farPlaneCorners[i]);
					//m_farPlaneCorners[i] = pWorldSpaceCorners[i + 4];
				}

				Vector3Df sunDirInView;
				matView.TransformNormal(sunDirInView, pSunLight->GetDirection());

				Matrix4f matWVP;
				int nWidth, nHeight;
				ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
				//matWVP.BuildProjectionMatrixOrthoOffCenterLH(0, nWidth, nHeight, 0, 0, 1);
				//pGPUDataParam->SetWorldViewProjectMatrix(matWVP);

				pGPUDataParam->SetSunDirection(sunDirInView);
				pGPUDataParam->SetSunLightColor(pSunLight->GetDiffuse());
				pGPUDataParam->SetFarPlaneCorners(m_farPlaneCorners);
				pPass->ApplyRenderStates();
				//m_quad.PostRender(NULL);    //平行光这种效果比较好

				m_pFullScreenQuad->Render(NULL);
			}

			Matrix4f matLight;   //灯光矩阵
			matLight.Identity();
			pPass = m_pPointLightTechnique->GetPass(0);
			while (!lightListIter.IsEnd())
			{
				CLight* pLight = lightListIter.GetNext();
				//设置gpu参数
				matLight.SetTranslation(pLight->GetDerivedPosition());
				matLight.SetScale(pLight->GetRange());
				pRenderer->SetWorldTransform(&matLight);
				pGPUDataParam->SetLightPosition(pLight->GetDerivedPosition());
				pGPUDataParam->SetLightColor(pLight->GetDiffuse());
				pGPUDataParam->SetLightRange(pLight->GetRange());
				//pGPUDataParam->SetMaterialShiness(255.0f);
				pPass->ApplyRenderStates();
				//pRenderer->SetVertexBuffer(0, m_billBoard.GetVertexBuffer());
				//pRenderer->SetIndexBuffer(NULL);
				m_billBoard.PostRender(pCurScene->GetCamera());
				//pLight->PostRender(NULL);
				//m_quad.PostRender(NULL);
			}

			pRenderer->EndScene();
		}
	}
    */

	//m_pRenderTexture->SaveToFile("D:/LightBuffer.png");
	RENDERER->SetRenderTargets(1, 0);

	RENDERER->SetTexture(10, m_pLightTarget->GetTexture());
}

void DeferredLightingLightBuffer::Clear()
{
    if (m_pLightTarget)
    {
        //RENDERER->DestroyTextureRenderTarget(m_pLightTarget);
		m_pLightTarget->Clear();
		SafeDelete(m_pLightTarget);
    }
}
