#include "stdafx.h"
#include "GBufferRenderProcess.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "MaterialManager.h"
//#include "RenderControl.h"
//#include "SpeedTreeInstManager.h"
#include "TextureMgr.h"
//#include "Technique.h"
#include "NormalSceneRenderProcess.h"
//#include "SpeedTreeEngine.h"
#include "RenderTarget.h"

GBufferRenderProcess::GBufferRenderProcess() : m_pRTTNormal(NULL)
, m_pMaterial(NULL)
, m_pRTTDepth(NULL)
//, m_pTechnique(NULL)
//, m_pTechniqueClearGBuffer(NULL)
{
    m_strName = "GBufferRenderTarget";
    m_quad.SetFullWindow(true);
	//m_bUseDepthSurface = true;
}

GBufferRenderProcess::~GBufferRenderProcess()
{

}

bool GBufferRenderProcess::Initialize(int nWidth, int nHeight)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    //if (m_pRenderTexture == NULL)
    //{
    //    m_pRenderTexture = TEXTURE_MANAGER->CreateTexture(m_strName.c_str(), TextureParam(nWidth, nHeight, 0, 1,
    //        eRSU_RenderTarget, RPF_R32F, TT_TEXTURE, true));
    //    //pRenderer->CreateFloatTexture(m_strName.c_str(), nWidth, nHeight, true, 1, true);
    //    m_pRenderTexture->SetResetOperation(true, 1.0f);
    //}
    if (m_pRTTNormal == NULL)
    {
        m_pRTTNormal = pRenderer->CreateTextureRenderTarget("NormalBufferRenderTarget", nWidth, nHeight, RPF_R32F);
    }

	if (m_pRTTDepth == NULL)
	{
		m_pRTTDepth = pRenderer->CreateTextureRenderTarget("DepthBufferRenderTarget", nWidth, nHeight, RPF_R32F);
	}

    m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/LightPrePass");
    if (m_pMaterial)
    {
        //m_pTechnique = m_pMaterial->GetTechnique("RenderToGBuffer");
        //m_pTechniqueClearGBuffer = m_pMaterial->GetTechnique("ClearGBuffer");
    }
    

    return true;
}

void GBufferRenderProcess::Clear()
{
    //RenderTarget::Clear();
    MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    if (m_pRTTNormal)
    {
        ENGINE_INST->GetRenderer()->DestroyTextureRenderTarget(m_pRTTNormal);
        m_pRTTNormal = NULL;
    }

	if (m_pRTTDepth)
	{
		ENGINE_INST->GetRenderer()->DestroyTextureRenderTarget(m_pRTTDepth);
		m_pRTTDepth = NULL;
	}
}
/*
void GBufferRenderProcess::RenderImpl()
{
    //Update();
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	pRenderer->SetTexture( 11, NULL );

    if (m_pMaterial)
    {
        
        if (!m_pMaterial->IsReady())
        {
            m_pMaterial->Cache();
            //m_pTechnique = m_pMaterial->GetTechnique("RenderToGBuffer");
            m_pTechniqueClearGBuffer = m_pMaterial->GetTechnique("ClearGBuffer");
        }

        if (m_pMaterial->IsReady())
        {
            
            ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();


            //pRenderer->SetRenderTarget(this);
            //pRenderer->SetRenderTarget(m_pRTTNormal);

			RenderTarget* pTargets[2];
			pTargets[0] = this;
			pTargets[1] = m_pRTTNormal;
			pRenderer->SetRenderTargets(2, pTargets);

            pRenderer->ClearScene( m_clrBackGround );
            pRenderer->BeginScene();
            // 渲染可视范围内的对像


            //首先要清空两个rendertarget
            for (int i = 0; i < m_pTechniqueClearGBuffer->GetPassesNum(); ++i)
            {
                CPass* pPass = m_pTechniqueClearGBuffer->GetPass(i);
                //pRenderer->SetTexture(0, NULL);
                pPass->ApplyRenderStates();
                m_quad.PostRender(NULL);
            }

            CRenderQueue* pQueue = pSceneMgr->GetRenderQueue();

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

                        pRenderable->RenderToGBuffer(pSceneMgr->GetCamera());
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

                        pRenderable->RenderToGBuffer(m_pCamera);
                    }
                }

            //}
            //pSceneMgr->RenderScene();
			//m_pRTTNormal->GetRenderTexture()->SaveToFile("D:/GBufferNormal.png");
			//m_pRenderTexture->SaveToFile("D:/GBufferDepth.dds");
            pRenderer->SetRenderTargets(2, NULL);

            pRenderer->EndScene();

			//begine add by hjj 2011-8-19
			pRenderer->SetTexture( 11, this->m_pRenderTexture );
			pRenderer->SetTextureBorderColor( 11, 0xffffffff );
			pRenderer->SetTextureFilter(11, TFT_Point);
			//end add

			
        }
        
    }
    
}
*/
CCamera* GBufferRenderProcess::GetCamera()
{
    ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();
    return pSceneMgr->GetCamera();
}

void GBufferRenderProcess::Render()
{

}
