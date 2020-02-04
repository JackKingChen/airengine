#include "stdafx.h"
#include "MathUtil.h"
#include "PostScreenCompositor.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "PostProcessManager.h"
#include "ISceneManager.h"
#include "RenderTargetScreenCompositor.h"
#include "Primitive.h"
//#include "Technique.h"
#include "GPUVariant.h"

//PostCompositorManualGPUDataSource::PostCompositorManualGPUDataSource()
//{
//    m_vtBlurSamplerOffset.resize(15);
//    m_vtBlurSamplerWeight.resize(15);
//}
//
//PostCompositorManualGPUDataSource::~PostCompositorManualGPUDataSource()
//{
//
//}
//
//void PostCompositorManualGPUDataSource::BuildManualConstantDefType(GPUConstantDefinition* pGPUDef)
//{
//    if (pGPUDef->strName == "vSampleOffsets")
//    {
//        m_vtBlurSamplerOffset.resize(pGPUDef->nArraySize);
//        pGPUDef->nManualUsageType = ePSMT_BlurSamplerOffset;
//    }
//    else if (pGPUDef->strName == "fSampleWeights")
//    {
//        m_vtBlurSamplerWeight.resize(pGPUDef->nArraySize);
//        pGPUDef->nManualUsageType = ePSMT_BlurSamplerWeight;
//    }
//}
//
//void PostCompositorManualGPUDataSource::UpdateGPUParametesImpl(GPUParameters* pParameters, GPUConstantDefinition* pGPUDef)
//{
//    switch (pGPUDef->nManualUsageType)
//    {
//        //case eSMT_ViewToLightProjMatrix:
//        //    break;;
//    case ePSMT_BlurSamplerOffset:
//        for (int i = 0; i < m_vtBlurSamplerOffset.size(); ++i)
//        {
//            pParameters->SetConstant(pGPUDef->nPhysicalIndex + i * 4, m_vtBlurSamplerOffset[i]);
//        }
//        
//        break;
//    case ePSMT_BlurSamplerWeight:
//        for (int i = 0; i < m_vtBlurSamplerWeight.size(); ++i)
//        {
//            pParameters->SetConstant(pGPUDef->nPhysicalIndex + i * 4, m_vtBlurSamplerWeight[i]);
//        }
//        
//        break;
//    }
//}


PostScreenCompositor::PostScreenCompositor() : m_pScreenRenderTarget(NULL), m_pPreTexture(NULL)
, m_pPostEffect(NULL)
//, m_pPostManualGPUDataSource(NULL)
, m_bHorizontalBlur(false)
//, m_bUseSceneRenderTarget(false)
, m_bUseScreenRenderTarget(false)
{
    //m_szType = "PostScreenCompositor";
    m_bFullWindow = true;
}

PostScreenCompositor::~PostScreenCompositor()
{

}

void PostScreenCompositor::PreRender(CCamera* pCamera)
{
    //更新所有特效的gpu参数
    //ScreenCompositor::PreRender(pCamera);
	//PostCompositorManualGPUDataSource* pPostManualGPUSource;
	IRenderer* pRenderer = RENDERER;
    int nWidth;
    int nHeight;
    ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
	switch (m_pscType)
    {
    case ePPT_COLOR_BlurH:
		UpdateBlurGPUData(true);
		break;
	case ePPT_COLOR_BlurV:
        UpdateBlurGPUData(false);
        break;
	case ePPT_COLOR_BloomBlurH:
		UpdateBlurGPUData(true);
        break;
	case ePPT_COLOR_BloomBlurV:
		UpdateBlurGPUData(false);
		break;
	case ePPT_COLOR_NormalCombine:
        //pRenderer->SetTexture(1, POSTPROCESS_MANAGER->GetSceneRenderTarget());
		pRenderer->SetTextureFilter(1, TFT_Linear);
        break;
    case ePPT_COLOR_ScreenCombine:
        //pRenderer->SetTexture(1, POSTPROCESS_MANAGER->GetSceneRenderTarget());
        pRenderer->SetTextureFilter(1, TFT_Linear);
        break;
    case ePPT_COLOR_SoftLightCombine:
        //pRenderer->SetTexture(1, POSTPROCESS_MANAGER->GetSceneRenderTarget());
        pRenderer->SetTextureFilter(1, TFT_Linear);
        break;
    case ePPT_COLOR_ColorBurn:
        //pRenderer->SetTexture(1, POSTPROCESS_MANAGER->GetSceneRenderTarget());
        pRenderer->SetTextureFilter(1, TFT_Linear);
        break;
    case ePPT_COLOR_DownFilter4:
        UpdateDownFilter4GPUData();
        break;
	case ePPT_Distortion:
		//pRenderer->SetTexture(0, POSTPROCESS_MANAGER->GetSceneRenderTarget());
		pRenderer->SetTextureFilter(0, TFT_Linear);
		break;
    }
    BuildScreenSizeData();
}
/*
void PostScreenCompositor::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleStrip);
        m_pPrimitive->SetUseUserPrimitive(true);
        m_pPrimitive->SetPrimitiveCount(2);
        m_pPrimitive->m_pVertexData = &m_renderQuad[0];
        m_pPrimitive->m_nVerticesStride = sizeof(RHWVERTEX);
        m_pPrimitive->m_pSrcRenderable = this;
		m_pPrimitive->SetVerticesNum(4);
    }
}
*/
void PostScreenCompositor::Clear()
{
    if (m_pScreenRenderTarget)
    {
        //ENGINE_INST->GetRenderer()->DestroyTextureRenderTarget(m_pScreenRenderTarget);
        m_pScreenRenderTarget->Clear();
        SafeDelete(m_pScreenRenderTarget);
    }

    //if (m_pPostManualGPUDataSource)
    //{
    //    SafeDelete(m_pPostManualGPUDataSource);
    //}

    ScreenCompositor::Clear();

    //CompositortList_Iter itEnd = m_lstCompositors.end();
    //for (CompositortList_Iter it = m_lstCompositors.begin(); it != itEnd; ++it)
    //{
    //    (*it)->Clear();
    //    SafeDelete(*it);
    //}
}


void PostScreenCompositor::SaveToFile(const char* szFilename)
{
    if (m_pScreenRenderTarget)
    {
        //m_pSceneRenderTarget->GetRenderTexture()->SaveToFile(szFilename);
        if (m_pPreTexture)
        {
            m_pPreTexture->SaveToFile(szFilename);
        }
    }
}

void PostScreenCompositor::SetActive(bool bActive)
{
    ScreenCompositor::SetActive(bActive);
}

void PostScreenCompositor::Process(TextureRenderTarget* pRTTInput, float fScale, float fUVScale)
{
    //设置屏幕放大比例
    m_fScale = fScale;
	m_fUVScale = fUVScale;
    //if (Prepare())
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();

        m_pPreTexture = pRTTInput->GetTexture(); //->GetRenderTexture();

		TextureRenderTarget* pTargets[1];
		pTargets[0] = m_pScreenRenderTarget;
        pRenderer->SetRenderTargetsEx(1, pTargets, NULL);

        pRenderer->BeginScene();        

		pRenderer->SetTexture(0, m_pPreTexture);
        
        Draw();


		pRenderer->SetTexture(0, 0);

        pRenderer->EndScene();
		pRenderer->SetRenderTargets(1, 0);
    }

}

/*
void PostScreenCompositor::SetUseScreenRenderTarget(bool bUse)
{
    m_bUseScreenRenderTarget = bUse;
    if (m_bUseScreenRenderTarget)
    {
        if (m_pScreenRenderTarget == NULL)
        {
            m_pScreenRenderTarget = new RenderTargetScreenCompositor;
            m_pScreenRenderTarget->Initialize(m_rcScreen.GetWidth(), m_rcScreen.GetHeight());
            (static_cast<RenderTargetScreenCompositor*>(m_pScreenRenderTarget))->SetScreenCompositor(this);
        }
    }
}
*/



//RenderTarget* PostScreenCompositor::GetOutputRenderTarget()
//{
//    if (m_lstCompositors.size() == 0)
//    {
//        return GetRenderTarget();
//    }
//    CompositortList::reverse_iterator it = m_lstCompositors.rbegin();
//    return (*it)->GetRenderTarget();
//}

void PostScreenCompositor::Create(int nPosX, int nPosY, int nWidth, int nHeight )
{
    ScreenCompositor::Create(ePPT_None, nPosX, nPosY, nWidth, nHeight);
    if (m_pScreenRenderTarget == NULL)
    {
		IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        m_pScreenRenderTarget = TextureRenderTarget::CreateRenderTarget("ScreenRenderTarget", nWidth, nHeight, RPF_A8R8G8B8); //pRenderer->CreateTextureRenderTarget("ScreenRenderTarget", nWidth, nHeight, RPF_A8R8G8B8); //new RenderTargetScreenCompositor;
    }
    
}

void PostScreenCompositor::SetPostEffect(PostEffect* pPostEffect)
{
    m_pPostEffect = pPostEffect;
    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    }
	switch (m_pscType)
	{
	case ePPT_COLOR_Monochrome:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Monochrome");
		}
		break;
	case ePPT_COLOR_NormalCombine:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/NormalCombine");
		}
		break;
	case ePPT_COLOR_ToneMap:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/ToneMap");
		}
		break;
	case ePPT_COLOR_BrightPass:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/BrightPass");
		}
		break;
	case ePPT_COLOR_BlurH:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Blur");
		}
			break;
	case ePPT_COLOR_BlurV:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Blur");
		}
		break;
	case ePPT_COLOR_BloomBlurH:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/BloomBlur");
		}
		break;
	case ePPT_COLOR_BloomBlurV:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/BloomBlur");
		}
		break;

	case ePPT_COLOR_DownFilter4:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/DownFilter4");
		}
		break;
	case ePPT_COLOR_UpFilter4:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/UpFilter4");
		}
		break;
    case ePPT_COLOR_ScreenCombine:
        {
            m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/ScreenCombine");
        }
        break;
    case ePPT_COLOR_SoftLightCombine:
        {
            m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/SoftLightCombine");
        }
        break;
    case ePPT_COLOR_ColorBurn:
        {
            m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/ColorBurnCombine");
        }
        break;
	case ePPT_Distortion:
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Distortion");
		}
		break;
	}
    //switch (pPostEffect->GetPostEffectType())
    //{
    //case ePET_Distortion:
    //    {
    //        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Distortion");
    //    }
    //    break;

    //case ePET_Bloom:
    //    {
    //        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Bloom");
    //    }
    //    break;

    //case ePET_Blur:
    //    {
    //        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Blur");
    //    }
    //    break;

    //case ePET_RayBlur:
    //    {
    //        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/RayBlur");
    //    }
    //    break;
    //}


    if (m_pMaterial && m_pMaterial->IsReady())
    {
        //m_pMaterial->SetFogEnable(false);
        //CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
        ////IShader* pShader = pTechnique->GetPass(0)->GetShader();
        //IGPUProgram* pVertexShader = pTechnique->GetPass(0)->GetVertexShader();
        ////PostCompositorManualGPUDataSource* pPostManualGPUSource = POSTPROCESS_MANAGER->GetPostManualGPUDataSource();
        ////pVertexShader->AddManualGPUDataParamSource(pPostManualGPUSource);
        //IGPUProgram* pPixelShader = pTechnique->GetPass(0)->GetPixelShader();
        ////pPixelShader->AddManualGPUDataParamSource(pPostManualGPUSource);
        //m_bPrepared = true;
    }
}
/*
bool PostScreenCompositor::Prepare()
{
    if (!m_bPrepared)
    {
        if (m_pMaterial)
        {
            if (m_pMaterial->IsReady())
            {
                m_pMaterial->SetFogEnable(false);
                
                m_bPrepared = true;
            }
            else
            {
                m_pMaterial->Cache();
            }
        }
    }

    return m_bPrepared;
}
*/
void PostScreenCompositor::RebuildRenderData(void)
{
	BuildScreenSizeData();
}

void PostScreenCompositor::BuildScreenSizeData()
{
	LPRHWVERTEX pVertex = &m_renderQuad[0];
    int nWidth, nHeight;
    ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
	float fPixelOffset = RENDERER->GetPixelOffset();
    float fWidth = nWidth * m_fScale;
    float fHeight = nHeight * m_fScale;
	pVertex[0].color = m_color;
	pVertex[0].rhw = 1.0f;
	pVertex[0].tu = 0.0f;
	pVertex[0].tv = 0.0f;
	pVertex[0].x =  -fPixelOffset;
	pVertex[0].y =  -fPixelOffset;
	pVertex[0].z = 1.0f;

	pVertex[1].color = m_color;
	pVertex[1].rhw = 1.0f;
	pVertex[1].tu = m_fUVScale;
	pVertex[1].tv = 0.0f;
	pVertex[1].x = fWidth - fPixelOffset;
	pVertex[1].y =  -fPixelOffset;
	pVertex[1].z = 1.0f;

	pVertex[2].color = m_color;
	pVertex[2].rhw = 1.0f;
	pVertex[2].tu = 0.0f;
	pVertex[2].tv = m_fUVScale;
	pVertex[2].x =  -fPixelOffset;
	pVertex[2].y = fHeight - fPixelOffset;
	pVertex[2].z = 1.0f;

	pVertex[3].color = m_color;
	pVertex[3].rhw = 1.0f;
	pVertex[3].tu = m_fUVScale;
	pVertex[3].tv = m_fUVScale;
	pVertex[3].x = fWidth - fPixelOffset;
	pVertex[3].y = fHeight - fPixelOffset;
	pVertex[3].z = 1.0f;
}